HPACKSRCDIR=		${.CURDIR}/..
HPACKTESTDIR+=		${.CURDIR}/hpack-test-samples
HPACKTESTDIR+=		${.CURDIR}/hpack-test-case

.PATH:	${HPACKSRCDIR}

PROG=			hpacktest
SRCS+=			main.c jsmn.c json.c
CFLAGS+=		-DJSMN_PARENT_LINKS

REGRESS_TARGETS?=	test

test: ${PROG}
	./${PROG} -v ${HPACKTESTDIR}

.include <bsd.regress.mk>
