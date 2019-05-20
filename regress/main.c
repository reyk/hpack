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
		printf("%s: empty headers\n", prefix);
		return (-1);
	}

	TAILQ_FOREACH(hdr, hdrs, hdr_entry) {
		if (hdr->hdr_name == NULL || hdr->hdr_value == NULL) {
			if (prefix != NULL)
				printf("%s invalid header: %s: %s\n", prefix,
				    hdr->hdr_name == NULL ?
				    "(null)" : hdr->hdr_name,
				    hdr->hdr_value == NULL ?
				    "(null)" : hdr->hdr_value);
			return (-1);
		}
		if (prefix != NULL)
			printf("%s %s: %s\n", prefix,
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
		printf("string needs to be specified in hex digits\n");
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
parse_hpack(const char *hex, struct hpack_headerblock *test,
    struct hpack_table *hpack)
{
	struct hpack_headerblock	*hdrs = NULL;
	unsigned char			 buf[8192];
	ssize_t				 len;
	extern const char		*__progname;
	int				 ret = -1;

	if ((len = parsehex(hex, buf, sizeof(buf))) == -1) {
		printf("wire format is not a hex string\n");
		goto fail;
	}
	if ((hdrs = hpack_decode(buf, len, hpack)) == NULL) {
		printf("hpack_decode\n");
		goto fail;
	}
	if (hpack_headerblock_print(NULL, test) == -1) {
		printf("test headers invalid\n");
		goto fail;
	}
	if (hpack_headerblock_print(NULL, hdrs) == -1) {
		printf("parsed headers invalid\n");
		goto fail;
	}
	if (hpack_headerblock_cmp(hdrs, test) != 0) {
		printf("test headers mismatched\n");
		goto fail;
	}

	ret = 0;
 fail:
	if (ret != 0) {
		printf(">>> wire: %s\n", hex);
		hpack_headerblock_print(">>> header:", test);
		hpack_headerblock_print("<<< parsed:", hdrs);
	}
	hpack_headerblock_free(hdrs);
	return (ret);
}

static int
parse_tests(char *argv[])
{
	struct hpack_table		*hpack = NULL;
	struct hpack_headerblock	*test = NULL;
	FTS				*fts;
	FTSENT				*ftsp = NULL;
	char				*str = NULL, *wire = NULL, *tblsz;
	FILE				*fp;
	off_t				 size;
	int				 ret = -1;
	struct jsmnn			*json = NULL, *cases, *obj, *hdr, *hdrs;
	size_t				 i = 0, j, k, ok = 0;
	const char			*errstr = NULL;
	size_t				 table_size, init_table_size;

	if ((fts = fts_open(argv, FTS_COMFOLLOW|FTS_NOCHDIR,
	    NULL)) == NULL) {
		errstr = "failed to open directory";
		goto done;
	}

	while ((ftsp = fts_read(fts)) != NULL) {
		if (ftsp->fts_info != FTS_F)
			continue;

		if (fnmatch("*.json", ftsp->fts_name,
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

		/* The default table size of the test cases is 4096 */
		init_table_size = 4096;

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
				table_size = init_table_size;
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
					    hdr->d.obj[k].rhs->d.str) == NULL) {
						errstr = "failed to add header";
						goto done;
					}
				}
			}

			if (hpack == NULL) {
				if (table_size > init_table_size)
					init_table_size = table_size;
				if ((hpack =
				    hpack_table_new(init_table_size)) == NULL) {
					errstr = "failed to get HPACK table";
					goto done;
				}
			}

			if (parse_hpack(wire, test, hpack) == -1) {
				errstr = "failed to parse HPACK";
				goto done;
			}

			if (hpack_table_size(hpack) > table_size) {
				errstr = "invalid table size";
				goto done;
			}

			ok++;
			hpack_headerblock_free(test);
			test = NULL;
			free(wire);
			wire = NULL;
		}

		i = 0;
		json_free(json);
		json = NULL;
		free(str);
		str = NULL;
		hpack_table_free(hpack);
		hpack = NULL;

		printf("SUCCESS: %s: %zu tests\n", ftsp->fts_path, ok);
	}

	ret = 0;
 done:
	if (errstr != NULL && ftsp != NULL && i != 0)
		printf("FAILED: %s: %s in test %zu\n",
		    ftsp->fts_path, errstr, i);
	else if (errstr != NULL && ftsp != NULL)
		printf("FAILED: %s: %s\n",
		    ftsp->fts_path, errstr);
	else if (errstr != NULL)
		printf("FAILED: %s\n", errstr);
	free(wire);
	hpack_table_free(hpack);
	hpack_headerblock_free(test);
	json_free(json);
	free(str);
	fts_close(fts);

	return (ret);
}

int
main(int argc, char *argv[])
{
	const char		*hex;

	if (hpack_init() == -1)
		return (1);

	if (argc > 1) {
		argv++;
		if (parse_tests(argv) == -1)
			return (1);
		return (0);
	}

	/* HPACK-encoded test string */
	hex = "4186a0e41d139d098284874085f2b24a84ff8849509089951a6dcf";

	if (parse_hpack(hex, NULL, NULL) == -1)
		return (1);

	return (0);
}
