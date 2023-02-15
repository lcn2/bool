#!/usr/bin/make
#
# bool - boolean file operations
#
# @(#) $Revision: 1.3 $
# @(#) $Id: Makefile,v 1.3 1999/09/20 18:43:48 chongo Exp $
# @(#) $Source: /usr/local/src/bin/bool/RCS/Makefile,v $
#
# Copyright (c) 1997 by Landon Curt Noll.  All Rights Reserved.
#
# Permission to use, copy, modify, and distribute this software and
# its documentation for any purpose and without fee is hereby granted,
# provided that the above copyright, this permission notice and text
# this comment, and the disclaimer below appear in all of the following:
#
#       supporting documentation
#       source copies
#       source works derived from this source
#       binaries derived from this source or from derived source
#
# LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
# EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

SHELL = /bin/sh
DEST = /usr/local/bin
RM = /bin/rm -f
CP = /bin/cp
MV = /bin/mv
LN = /bin/ln
CHMOD = /bin/chmod
#CFLAGS = -g
CFLAGS = -O2

PROG = and
LINKS = or xor nand nor xnor
TARGETS = ${PROG} ${LINKS}

all: ${TARGETS}

and: and.c
	${CC} ${CFLAGS} -o $@ $?

or: and
	${LN} -f $? $@

xor: and
	${LN} -f $? $@

nand: and
	${LN} -f $? $@

nor: and
	${LN} -f $? $@

xnor: and
	${LN} -f $? $@

install: all
	${RM} ${DEST}/${PROG}.new
	${CP} ${PROG} ${DEST}/${PROG}.new
	${CHMOD} 0555 ${DEST}/${PROG}.new
	${MV} -f ${DEST}/${PROG}.new ${DEST}/${PROG}
	@for i in ${LINKS}; do \
	    echo "${LN} -f ${DEST}/${PROG} ${DEST}/$$i"; \
	    ${LN} -f ${DEST}/${PROG} ${DEST}/$$i; \
	done

clean:
	${RM} *.o

clobber: clean
	${RM} ${TARGETS}
