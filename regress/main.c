/*	$OpenBSD$	*/

/*
 * Copyright (c) 2019 Reyk Floeter <reyk@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fts.h>
#include <fnmatch.h>

#include "hpack.h"
#include "extern.h"

static int	 encode_huffman(const char *);
static int	 decode_huffman(const char *);

int	 verbose;
int	 encode;

static void
log(int level, const char *fmt, ...)
{
	va_list	ap;

	if (verbose < level)
		return;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

static const char *
json_uascii_decode(char *str)
{
	char		*p, *q;
	char		 hex[5];
	unsigned long	 x;

	hex[4] = '\0';
	p = q = str;

	while (*p != '\0') {
		if (*p != '\\') {
			*q = *p;
			p++;
			q++;
			continue;
		}
		switch (*(p + 1)) {
		case 'u':
			/* Encoding character is followed by four hex chars */
			if (!(isxdigit((unsigned char)p[2]) &&
			    isxdigit((unsigned char)p[3]) &&
			    isxdigit((unsigned char)p[4]) &&
			    isxdigit((unsigned char)p[5])))
				return (NULL);

			hex[0] = p[2];
			hex[1] = p[3];
			hex[2] = p[4];
			hex[3] = p[5];

			/* We don't support non-ASCII chars */
			if ((x = strtoul(hex, NULL, 16)) > 0x7f)
				return (NULL);
			*q = (char)x;
			p += 5;
			break;
		case '"':
			*q = '"';
			p++;
			break;
		default:
			*q = *p;
			break;
		}
		p++;
		q++;
	}
	*q = '\0';

	return (str);
}

static int
hpack_headerblock_cmp(struct hpack_headerblock *a,
    struct hpack_headerblock *b)
{
	struct hpack_header	*ha, *hb;

	for (ha = TAILQ_FIRST(a), hb = TAILQ_FIRST(b);
	    !(ha == NULL && hb == NULL);
	    ha = TAILQ_NEXT(ha, hdr_entry),
	    hb = TAILQ_NEXT(hb, hdr_entry)) {
#define ONE_NULL(_a, _b)	(		\
	((_a) != NULL && (_b) == NULL) ||	\
	((_a) == NULL && (_b) != NULL)		\
)
		if (ONE_NULL(ha, hb) ||
		    ONE_NULL(ha->hdr_name, hb->hdr_name) ||
		    ONE_NULL(ha->hdr_value, hb->hdr_value))
			return (-1);
#undef ONE_NULL
		if (ha->hdr_name != NULL &&
		    strcmp(ha->hdr_name, hb->hdr_name) != 0)
			return (-2);
		if (ha->hdr_value != NULL &&
		    strcmp(ha->hdr_value, hb->hdr_value) != 0)
			return (-3);
	}

	return (0);
}

static int
hpack_headerblock_print(const char *prefix, struct hpack_headerblock *hdrs)
{
	struct hpack_header	*hdr;

	if (hdrs == NULL)
		return (0);
	if (TAILQ_EMPTY(hdrs)) {
		log(2, "%s: empty headers\n", prefix);
		return (-1);
	}

	TAILQ_FOREACH(hdr, hdrs, hdr_entry) {
		if (hdr->hdr_name == NULL || hdr->hdr_value == NULL) {
			if (prefix != NULL)
				log(2, "%s invalid header: %s: %s\n", prefix,
				    hdr->hdr_name == NULL ?
				    "(null)" : hdr->hdr_name,
				    hdr->hdr_value == NULL ?
				    "(null)" : hdr->hdr_value);
			return (-1);
		}
		if (prefix != NULL)
			log(2, "%s %s: %s\n", prefix,
			    hdr->hdr_name, hdr->hdr_value);
	}

	return (0);
}

static int
x2i(const char *s)
{
	char	ss[3];

	ss[0] = s[0];
	ss[1] = s[1];
	ss[2] = 0;

	if (!isxdigit(s[0]) || !isxdigit(s[1])) {
		log(2, "string needs to be specified in hex digits\n");
		return (-1);
	}
	return ((int)strtoul(ss, NULL, 16));
}

static ssize_t
parsehex(const char *hex, unsigned char *buf, size_t len)
{
	ssize_t		  datalen;
	unsigned int	  i;
	int		  c;

	memset(buf, 0, len);
	if (strncmp(hex, "0x", 2) == 0)
		hex += 2;
	datalen = strlen(hex) / 2;
	if (datalen > (ssize_t)len)
		return (-1);

	for (i = 0; i < datalen; i++) {
		if ((c = x2i(hex + 2 * i)) == -1)
			return (-1);
		buf[i] =  (unsigned char)c;
	}

	return (datalen);
}

static int
parse_data(unsigned char *buf, size_t len,
    struct hpack_headerblock *test, struct hpack_table *hpack)
{
	struct hpack_headerblock	*hdrs = NULL;
	int				 ret = -1;

	if ((hdrs = hpack_decode(buf, len, hpack)) == NULL) {
		log(2, "hpack_decode\n");
		goto fail;
	}

	if (test != NULL && hpack_headerblock_print(NULL, test) == -1) {
		log(2, "test headers invalid\n");
		goto fail;
	}
	if (hpack_headerblock_print(NULL, hdrs) == -1) {
		log(2, "parsed headers invalid\n");
		goto fail;
	}
	if (test != NULL && (ret = hpack_headerblock_cmp(hdrs, test)) != 0) {
		log(2, "test headers mismatched (returned %d)\n", ret);
		ret = -1;
		goto fail;
	}

	ret = 0;
 fail:
	if (ret != 0) {
		hpack_headerblock_print(">>> header:", test);
		hpack_headerblock_print("<<< parsed:", hdrs);
	}
	hpack_headerblock_free(hdrs);
	return (ret);
}

static int
parse_hex(const char *hex, struct hpack_headerblock *test,
    struct hpack_table *hpack)
{
	unsigned char			 buf[8192];
	ssize_t				 len;

	if ((len = parsehex(hex, buf, sizeof(buf))) == -1) {
		log(2, "wire format is not a hex string\n");
		return (-1);
	}

	if (parse_data(buf, len, test, hpack) == -1)
		return (-1);

	log(2, ">>> wire: %s\n", hex);
	return (0);
}

static ssize_t
parse_input(const char *name, size_t init_table_size)
{
	struct hpack_table	*hpack = NULL;
	FILE			*fp;
	char			 buf[BUFSIZ];
	ssize_t			 ok = 0, ret = -1;

	if (encode)
		return (-1);
	if (strcmp("-", name) == 0)
		fp = stdin;
	else if ((fp = fopen(name, "r")) == NULL)
		return (-1);

	if ((hpack = hpack_table_new(init_table_size)) == NULL)
		goto done;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		buf[strcspn(buf, "\r\n")] = '\0';
		if (parse_hex(buf, NULL, hpack) == -1) {
			log(1, "hex HPACK decoding failed\n");
			goto done;
		}
		ok++;
	}

	ret = ok;
 done:
	if (fp != NULL && fp != stdin)
		fclose(fp);
	hpack_table_free(hpack);

	return (ret);
}

static int
parse_raw(const char *name, size_t init_table_size)
{
	char				 buf[65535], *ptr = NULL, *k, *v;
	struct hpack_table		*hpack = NULL;
	struct hpack_headerblock	*hdrs = NULL;
	FILE				*fp;
	int				 ret = -1;
	size_t				 len;

	if (strcmp("-", name) == 0)
		fp = stdin;
	else if ((fp = fopen(name, "r")) == NULL)
		goto done;
	if ((hpack = hpack_table_new(init_table_size)) == NULL)
		goto done;
	if (encode) {
		if ((hdrs = hpack_headerblock_new()) == NULL)
			goto done;
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			buf[strcspn(buf, "\r\n")] = '\0';
			k = buf;
			if ((v = strchr(k + 1, ':')) != NULL) {
				*v++ = '\0';
				v += strspn(v, " \t");
			} else if (isupper(buf[0])) {
				/* cheap way to test for the method */
				buf[strcspn(buf, " \t")] = '\0';
				k = ":method";
				v = buf;
			} else
				v = "";
			log(2, "adding header '%s: %s'\n", k, v);
			if (hpack_header_add(hdrs,
			    k, v, HPACK_INDEX) == NULL)
				goto done;
		}
		if ((ptr = hpack_encode(hdrs, &len, hpack)) == NULL) {
			log(1, "raw HPACK decoding failed\n");
			goto done;
		}
	} else {
		if ((len = fread(buf, 1, sizeof(buf), fp)) < 1) {
			if (feof(fp))
				ret = 0;
			goto done;
		}
		ptr = buf;
	}

	if ((hdrs = hpack_decode(ptr, len, hpack)) == NULL) {
		log(1, "raw HPACK decoding failed\n");
		goto done;
	}

	ret = 0;
 done:
	if (ptr != buf)
		free(ptr);
	if (fp != NULL && fp != stdin)
		fclose(fp);
	hpack_headerblock_free(hdrs);
	hpack_table_free(hpack);

	return (ret);
}

static int
parse_dir(char *argv[], size_t init_table_size)
{
	struct hpack_table		*hpack = NULL, *hpack2 = NULL;
	struct hpack_headerblock	*test = NULL;
	FTS				*fts;
	FTSENT				*ftsp = NULL;
	char				*str = NULL, *wire = NULL, *tblsz;
	FILE				*fp;
	off_t				 size;
	int				 ret = -1;
	struct jsmnn			*json = NULL, *cases, *obj, *hdr, *hdrs;
	size_t				 i = 0, j, k;
	ssize_t				 ok = 0;
	const char			*errstr = NULL;
	size_t				 table_size, file_table_size, len;

	if (encode)
		return (-1);
	if ((fts = fts_open(argv, FTS_COMFOLLOW|FTS_NOCHDIR,
	    NULL)) == NULL) {
		errstr = "failed to open directory";
		goto done;
	}

	while ((ftsp = fts_read(fts)) != NULL) {
		if (ftsp->fts_info != FTS_F)
			continue;

		file_table_size = init_table_size;

		if (fnmatch("*.hpacktest", ftsp->fts_name,
		    FNM_PATHNAME) != FNM_NOMATCH) {
			if ((ok = parse_input(ftsp->fts_accpath,
			    file_table_size)) < 0) {
				errstr = "hex input file parsing failed";
				goto done;
			}
			goto next;
		} else if (fnmatch("*.hpackraw", ftsp->fts_name,
		    FNM_PATHNAME) != FNM_NOMATCH) {
			if (parse_raw(ftsp->fts_accpath,
			    file_table_size) == -1) {
				errstr = "raw input file parsing failed";
				goto done;
			}
			ok = 1;
			goto next;
		} else if (fnmatch("headers_??.txt", ftsp->fts_name,
		    FNM_PATHNAME) != FNM_NOMATCH) {
			if (encode_huffman(ftsp->fts_accpath) == -1) {
				errstr = "huffman encoding failed";
				goto done;
			}
			ok = 1;
			goto next;
		} else if (fnmatch("story_*.json", ftsp->fts_name,
		    FNM_PATHNAME) == FNM_NOMATCH)
			continue;
		size = ftsp->fts_statp->st_size;
		ok = 0;

		if ((fp = fopen(ftsp->fts_accpath, "r")) == NULL)
			continue;

		if ((str = malloc(size)) == NULL) {
			fclose(fp);
			continue;
		}
		if ((off_t)fread(str, 1, size, fp) != size) {
			fclose(fp);
			free(str);
			continue;
		}
		fclose(fp);

		if ((json = json_parse(str, size)) == NULL) {
			errstr = "json parsing failed";
			goto done;
		}
		if ((cases = json_getarray(json, "cases")) == NULL) {
			errstr = "no test cases found";
			goto done;
		}

		for (i = 0; i < cases->fields; i++) {
			if ((obj =
			    json_getarrayobj(cases->d.array[i])) == NULL)
				continue;
			if ((wire = json_getstr(obj, "wire")) == NULL) {
				/* ignore test case without wire definition */
				ret = 0;
				goto done;
			}
			if ((tblsz = json_getstr(obj,
			    "header_table_size")) != NULL) {
				table_size = strtonum(tblsz,
				    0, LONG_MAX, &errstr);
				free(tblsz);
				if (errstr != NULL)
					goto done;
			} else
				table_size = file_table_size;
			if ((hdrs = json_getarray(obj, "headers")) == NULL) {
				errstr = "no headers found";
				goto done;
			}
			if ((test = hpack_headerblock_new()) == NULL)
				goto done;
			for (j = 0; j < hdrs->fields; j++) {
				if ((hdr = json_getarrayobj(hdrs->d.array[j]))
				    == NULL)
					continue;
				for (k = 0; k < hdr->fields; k++) {
					if (hdr->d.obj[k].lhs->type !=
					    JSMN_STRING &&
					    hdr->d.obj[k].lhs->type !=
					    JSMN_PRIMITIVE)
						continue;
					if (json_uascii_decode(
					    hdr->d.obj[k].rhs->d.str) == NULL) {
						errstr = "invalid JSON value";
						goto done;
					}
					if (hpack_header_add(test,
					    hdr->d.obj[k].lhs->d.str,
					    hdr->d.obj[k].rhs->d.str, 0) == NULL) {
						errstr = "failed to add header";
						goto done;
					}
				}
			}

			if (hpack == NULL) {
				if (table_size > file_table_size)
					file_table_size = table_size;
				if ((hpack =
				    hpack_table_new(file_table_size)) == NULL) {
					errstr = "failed to get HPACK table";
					goto done;
				}
				if ((hpack2 =
				    hpack_table_new(file_table_size)) == NULL) {
					errstr = "failed to get HPACK table";
					goto done;
				}
			}

			if (parse_hex(wire, test, hpack) == -1) {
				errstr = "failed to parse HPACK";
				goto done;
			}

			if (hpack_table_size(hpack) > table_size) {
				errstr = "invalid table size";
				goto done;
			}

			/* Test encoding by re-encoding of the header */
			free(wire);
			if ((wire = hpack_encode(test, &len, hpack2)) == NULL) {
				errstr = "re-encoding failed";
				goto done;
			}
			if (parse_data(wire, len, test, hpack2) == -1) {
				errstr = "re-decoding failed";
				goto done;
			}

			ok++;
			hpack_headerblock_free(test);
			test = NULL;
			free(wire);
			wire = NULL;
		}

		i = 0;
		hpack_table_free(hpack);
		hpack_table_free(hpack2);
		hpack = hpack2 = NULL;
		json_free(json);
		json = NULL;
		free(str);
		str = NULL;

 next:
		log(1, "SUCCESS: %s: %zu tests\n", ftsp->fts_path, ok);
	}

	ret = 0;
 done:
	if (errstr != NULL && ftsp != NULL && i != 0)
		log(1, "FAILED: %s: %s in test %zu\n",
		    ftsp->fts_path, errstr, i);
	else if (errstr != NULL && ftsp != NULL)
		log(1, "FAILED: %s: %s\n",
		    ftsp->fts_path, errstr);
	else if (errstr != NULL)
		log(1, "FAILED: %s\n", errstr);
	free(wire);
	hpack_table_free(hpack);
	hpack_table_free(hpack2);
	hpack_headerblock_free(test);
	json_free(json);
	free(str);
	fts_close(fts);

	return (ret);
}

static int
encode_huffman(const char *name)
{
	char				 buf[65535];
	char				*enc = NULL, *dec = NULL;
	FILE				*fp;
	int				 ret = -1;
	size_t				 len = 0, enclen = 0, declen = 0;

	if (strcmp("-", name) == 0)
		fp = stdin;
	else if ((fp = fopen(name, "r")) == NULL)
		goto done;
	if ((len = fread(buf, 1, sizeof(buf), fp)) < 1) {
		if (feof(fp))
			ret = 0;
		goto done;
	}
	if ((enc = hpack_huffman_encode(buf, len, &enclen)) == NULL)
		goto done;
	if ((dec = hpack_huffman_decode(enc, enclen, &declen)) == NULL)
		goto done;
	if (memcmp(dec, buf, len) != 0)
		goto done;

	ret = 0;
 done:
	log(2, "%s: huffman lengths: raw input %zu,"
	    " encoded output %zu, decoded %zu\n",
	    ret == 0 ? "SUCCESS": "FAILED" , len, enclen, declen);
	if (fp != NULL && fp != stdin)
		fclose(fp);
	free(enc);
	free(dec);

	return (ret);
}

static int
decode_huffman(const char *name)
{
	char				 buf[65535];
	char				*enc = NULL, *dec = NULL;
	FILE				*fp;
	int				 ret = -1;
	size_t				 len = 0, enclen = 0, declen = 0;

	if (strcmp("-", name) == 0)
		fp = stdin;
	else if ((fp = fopen(name, "r")) == NULL)
		goto done;
	if ((len = fread(buf, 1, sizeof(buf), fp)) < 1) {
		if (feof(fp))
			ret = 0;
		goto done;
	}
	if ((dec = hpack_huffman_decode(buf, len, &declen)) == NULL)
		goto done;
	if ((enc = hpack_huffman_encode(dec, declen, &enclen)) == NULL)
		goto done;
	if (memcmp(enc, buf, len) != 0)
		goto done;

	ret = 0;
 done:
	log(2, "%s: huffman lengths: raw input %zu,"
	    " decoded output %zu, encoded %zu\n",
	    ret == 0 ? "SUCCESS": "FAILED" , len, declen, enclen);
	if (fp != NULL && fp != stdin)
		fclose(fp);
	free(enc);
	free(dec);

	return (ret);
}

static __dead void
usage(void)
{
	extern char	*__progname;

	fprintf(stderr, "usage: %s [-d|e file] [-h hex] [-p input-file]"
	    " [-r raw-file] [-x hex] [dir ...]\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	const char	*hex = NULL, *input = NULL, *raw = NULL;
	const char	*huffenc = NULL, *huffdec = NULL;
	int		 ch, ret;

	if (hpack_init() == -1)
		return (1);

	while ((ch = getopt(argc, argv, "d:Ee:h:i:r:v")) != -1) {
		switch (ch) {
		case 'd':
			huffdec = optarg;
			break;
		case 'E':
			encode = 1;
			break;
		case 'e':
			huffenc = optarg;
			break;
		case 'h':
			hex = optarg;
			break;
		case 'i':
			input = optarg;
			break;
		case 'r':
			raw = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (huffdec != NULL)
		ret = decode_huffman(huffdec);
	else if (huffenc != NULL)
		ret = encode_huffman(huffenc);
	else if (hex != NULL)
		ret = parse_hex(hex, NULL, NULL);
	else if (input != NULL)
		ret = parse_input(input, 4096);
	else if (raw != NULL)
		ret = parse_raw(raw, 4096);
	else if (argc > 0)
		ret = parse_dir(argv, 4096);
	else
		usage();
	if (ret == -1)
		return (1);

	return (0);
}
