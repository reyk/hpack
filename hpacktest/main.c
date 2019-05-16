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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <err.h>

#include "hpack.h"

static int
x2i(const char *s)
{
	char	ss[3];

	ss[0] = s[0];
	ss[1] = s[1];
	ss[2] = 0;

	if (!isxdigit(s[0]) || !isxdigit(s[1])) {
		warnx("string needs to be specified in hex digits");
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

int
main(int argc, char *argv[])
{
	struct hpack_headerlist	*hdrs;
	struct hpack_header	*hdr;
	unsigned char		 buf[1024];
	const char		*hex;
	ssize_t			 len;

	if (argc > 2) 
		errx(1, "usage: %s 0x...", argv[0]);
	else if (argc == 2)
		hex = argv[1];
	else {
		/* HPACK-encoded test string */
		hex = "4186a0e41d139d098284874085f2b24a84ff8849509089951a6dcf";
	}

	if ((len = parsehex(hex, buf, sizeof(buf))) == -1)
		errx(1, "usage: %s 0x...", argv[0]);

	if (hpack_init() == -1)
		errx(1, "hpack_init");

	if ((hdrs = hpack_decode(buf, len, NULL)) == NULL)
		errx(1, "hpack_decode");

	TAILQ_FOREACH(hdr, hdrs, hdr_entry) {
		if (hdr->hdr_name == NULL || hdr->hdr_value == NULL)
			errx(1, "invalid header");
		printf("%s: %s\n", hdr->hdr_name, hdr->hdr_value);
	}
	hpack_headerlist_free(hdrs);

	return (0);
}
