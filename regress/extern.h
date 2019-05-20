/*	$OpenBSD$	*/

/*
 * Copyright (c) 2019 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2016 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "jsmn.h"

#ifndef EXTERN_H
#define EXTERN_H

struct	jsmnp;

/*
 * A node in the JSMN parse tree.
 * Each of this corresponds to an object in the original JSMN token
 * list, although the contents have been extracted properly.
 */
struct	jsmnn {
	struct parse	*p; /* parser object */
	union {
		char *str; /* JSMN_PRIMITIVE, JSMN_STRING */
		struct jsmnp *obj; /* JSMN_OBJECT */
		struct jsmnn **array; /* JSMN_ARRAY */
	} d;
	size_t		 fields; /* entries in "d" */
	jsmntype_t	 type; /* type of node */
};

/*
 * Objects consist of node pairs: the left-hand side (before the colon)
 * and the right-hand side---the data.
 */
struct	jsmnp {
	struct jsmnn	*lhs; /* left of colon */
	struct jsmnn	*rhs; /* right of colon */
};

/* JSON parsing routines */
struct jsmnn	*json_parse(const char *, size_t);
void		 json_free(struct jsmnn *);
struct jsmnn	*json_getarrayobj(struct jsmnn *);
struct jsmnn	*json_getarray(struct jsmnn *, const char *);
struct jsmnn	*json_getobj(struct jsmnn *, const char *);
char		*json_getstr(struct jsmnn *, const char *);

#endif /* EXTERN_H */
