/*
 * and - produce the logical op of multiple files
 *
 * @(#) $Revision$
 * @(#) $Id$
 * @(#) $Source$
 *
 * Copyright (c) 1997 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * usage:
 *	and file [file2 ...]
 *	or file [file2 ...]
 *	xor file [file2 ...]
 *	nand file [file2 ...]
 *	nor file [file2 ...]
 *	xnor file [file2 ...]
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

char *program;		/* our name */

char in[BUFSIZ];	/* read buffer */
char out[BUFSIZ];	/* write buffer (logical operation of all reads) */

#define AND 1	/* logical and operation */
#define OR 2	/* logical or operation */
#define XOR 3	/* logical xor operation */
#define NAND 4	/* logical nand operation */
#define NOR 5	/* logical nor operation */
#define XNOR 6	/* logical xnor operation */


main(int argc, char *argv[])
{
    FILE **stream;	/* array of file descriptors */
    int opencnt;	/* number of files still open */
    int readcnt;	/* number of chars read into the in buffer */
    int writecnt;	/* number of chars written from the out buffer */
    int outcnt;		/* chars in the out buffer that have been processed */
    int boolcnt;	/* chars in the in buffer that may be processed */
    int op;		/* operation type: AND, OR, ... XNOR */
    char *p;
    int i;
    int j;

    /*
     * parse args
     */
    program = argv[0];
    p = strrchr(program, '/');
    if (p == NULL) {
	p = program;
    } else {
	++p;
    }
    if (strcmp(p, "and") == 0) {
	op = AND;
    } else if (strcmp(p, "or") == 0) {
	op = OR;
    } else if (strcmp(p, "xor") == 0) {
	op = XOR;
    } else if (strcmp(p, "nand") == 0) {
	op = NAND;
    } else if (strcmp(p, "nor") == 0) {
	op = NOR;
    } else if (strcmp(p, "xnor") == 0) {
	op = XNOR;
    } else {
	fprintf(stderr,
	  "%s: program name isn't: and, or, xor, nand, nor or xnor\n", program);
	exit(1);
    }
    if (argc < 2) {
	fprintf(stderr, "usage: %d file [file2 ...]\n", program);
	exit(2);
    }

    /*
     * open each file
     */
    stream = (FILE **)malloc((argc-1) * sizeof(FILE *));
    if (stream == NULL) {
	fprintf(stderr, "%s: malloc failure of %d FILE*'s\n", program, argc-1);
	exit(3);
    }
    for (i=0; i < argc-1; ++i) {
	/* - means stdin */
	if (strcmp(argv[i+1], "-") == 0) {
	    stream[i] = stdin;	
	} else {
	    stream[i] = fopen(argv[i+1], "r");
	    if (stream[i] == NULL) {
		fprintf(stderr, "%s: fopen of %s: ", program, argv[i+1]);
		perror(NULL);
		exit(4);
	    }
	}
    }

    /*
     * read blocks in each file until all are EOF
     */
    opencnt = argc - 1;
    do {

	/* clear the next output buffer */
	memset(out, 0, sizeof(out));
	outcnt = 0;

	/*
	 * read each open file
	 */
	for (i=0; i < argc-1; ++i) {

	    /* skip already closed files */
	    if (stream[i] == NULL) {
		continue;
	    }

	    /* 
	     * read a block from the file 
	     */
	    clearerr(stream[i]);
	    readcnt = fread(in, 1, BUFSIZ, stream[i]);
	    if (readcnt <= 0) {

		/* EOF or file error */
		if (ferror(stream[i])) {
		    fprintf(stderr, "%s: read error %s: ", program, argv[i+1]);
		    perror(NULL);
		}

		/* close off file due to EOF or error */
		(void) fclose(stream[i]);
		stream[i] = NULL;
		--opencnt;
		continue;
	    }

	    /*
	     * logically operate on out chars that already exist
	     */
	    boolcnt = ((outcnt <= readcnt) ? outcnt : readcnt);
	    switch (op) {
	    case AND:
		for (j=0; j < boolcnt; ++j) {
		    out[j] &= in[j];
		}
		break;
	    case OR:
		for (j=0; j < boolcnt; ++j) {
		    out[j] |= in[j];
		}
		break;
	    case XOR:
		for (j=0; j < boolcnt; ++j) {
		    out[j] ^= in[j];
		}
		break;
	    case NAND:
		for (j=0; j < boolcnt; ++j) {
		    out[j] = ~(out[j] & in[j]);
		}
		break;
	    case NOR:
		for (j=0; j < boolcnt; ++j) {
		    out[j] = ~(out[j] | in[j]);
		}
		break;
	    case XNOR:
		for (j=0; j < boolcnt; ++j) {
		    out[j] = ~(out[j] ^ in[j]);
		}
		break;
	    }

	    /*
	     * if we have out chars that have not been processed,
	     * then we simply transfer the first buffer chars to them
	     */
	    if (outcnt < readcnt) {
		for (j=outcnt; j < readcnt; ++j) {
		    out[j] = in[j];
		}
		outcnt = readcnt;
	    }
	}

	/*
	 * write out all processed data to stdout
	 */
	if (outcnt > 0) {
	    clearerr(stdout);
	    writecnt = fwrite(out, 1, outcnt, stdout);
	    if (writecnt != outcnt) {
		
		/* stop on EOF or write error */
		if (ferror(stdout)) {
		    fprintf(stderr, "%s: write error: ", program);
		    perror(NULL);
		} else {
		    fprintf(stderr,
		      "%s: short write %d < %d: ", program, writecnt, outcnt);
		}
		exit(5);
	    }
	}
    } while (opencnt > 0);

    /*
     * all done
     */
    exit(0);
}
