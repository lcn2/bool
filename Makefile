#!/bin/make
#  @(#} $Revision: 1.20 $
#  @(#} RCS control in //prime.corp/usr/local/src/cmd/bool/Makefile
#
# bool - boolean file operations

SHELL = /bin/sh
DEST = /usr/local/bin
RM = /bin/rm -f
CP = /bin/cp
MV = /bin/mv
LN = /bin/ln
CHMOD = /bin/chmod
CFLAGS = -g
#CFLAGS = -O2 -n32

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
