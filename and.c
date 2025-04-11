/*
 * and - produce the logical op (and, or, xor, nand, nor xnor) of files
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
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/errno.h>


/*
 * official version
 */
#define VERSION "1.4.1 2025-04-11"          /* format: major.minor YYYY-MM-DD */

#define AND 1	/* logical and operation */
#define OR 2	/* logical or operation */
#define XOR 3	/* logical xor operation */
#define NAND 4	/* logical nand operation */
#define NOR 5	/* logical nor operation */
#define XNOR 6	/* logical xnor operation */


/*
 * usage message
 */
static const char * const usage =
  "usage: %s [-h] level] [-V] [-n]\n"
        "    [-c] [-o offset] file ...\n"
        "\n"
        "    -h            print help message and exit\n"
        "    -V            print version string and exit\n"
        "\n"
        "    file ...      file(s) to process, - ==> read stdin\n"
        "\n"
        "Exit codes:\n"
        "    0         all OK\n"
	"    1         failed to open or read a file\n"
        "    2         -h and help string printed or -V and version string printed\n"
        "    3         command line error\n"
	"    4         program name isn't: and, or, xor, nand, nor xnor\n"
        " >= 10        internal error\n"
        "\n"
        "%s version: %s\n";


/*
 * static declarations
 */
static char *program = NULL;    /* our name */
static char *prog = NULL;       /* basename of program */
static const char * const version = VERSION;
/**/
static char in[BUFSIZ];	    /* read buffer */
static char out[BUFSIZ];    /* write buffer (logical operation of all reads) */


int
main(int argc, char *argv[])
{
    FILE **stream;	/* array of file descriptors */
    int opencnt;	/* number of files still open */
    int readcnt;	/* number of chars read into the in buffer */
    int writecnt;	/* number of chars written from the out buffer */
    int outcnt;		/* chars in the out buffer that have been processed */
    int boolcnt;	/* chars in the in buffer that may be processed */
    int op;		/* operation type: AND, OR, ... XNOR */
    int i;
    int j;

    /*
     * parse args
     */
    program = argv[0];
    prog = strrchr(program, '/');
    if (prog == NULL) {
	prog = program;
    } else {
	++prog;
    }
    program = argv[0];
    while ((i = getopt(argc, argv, ":hV")) != -1) {
        switch (i) {

        case 'h':                   /* -h - print help message and exit */
	    fprintf(stderr, usage, program, prog, version);
            exit(2); /* ooo */
            /*NOTREACHED*/

	 case 'V':                   /* -V - print version string and exit */
            (void) printf("%s\n", version);
            exit(2); /* ooo */
            /*NOTREACHED*/

	case ':':
            (void) fprintf(stderr, "%s: ERROR: requires an argument -- %c\n", program, optopt);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/

        case '?':
            (void) fprintf(stderr, "%s: ERROR: illegal option -- %c\n", program, optopt);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/

        default:
            fprintf(stderr, "%s: ERROR: invalid -flag\n", program);
	    fprintf(stderr, usage, program, prog, version);
            exit(3); /* ooo */
            /*NOTREACHED*/
        }
    }
    /* skip over command line options */
    argv += optind;
    argc -= optind;
    /* check the arg count */
    if (argc < 1) {
        fprintf(stderr, "%s: ERROR: expected at least 1 arg, found: %d\n", program, argc);
	fprintf(stderr, usage, program, prog, version);
        exit(3); /* ooo */
        /*NOTREACHED*/
    }


    /*
     * determine boolean operation
     */
    if (strcmp(prog, "and") == 0) {
	op = AND;
    } else if (strcmp(prog, "or") == 0) {
	op = OR;
    } else if (strcmp(prog, "xor") == 0) {
	op = XOR;
    } else if (strcmp(prog, "nand") == 0) {
	op = NAND;
    } else if (strcmp(prog, "nor") == 0) {
	op = NOR;
    } else if (strcmp(prog, "xnor") == 0) {
	op = XNOR;
    } else {
	fprintf(stderr,
	  "%s: program name isn't: and, or, xor, nand, nor or xnor\n", program);
	exit(4);
    }

    /*
     * open each file
     */
    stream = (FILE **)malloc((argc+1) * sizeof(FILE *));    /* +1 for paranoia */
    if (stream == NULL) {
	fprintf(stderr, "%s: malloc failure of %d FILE*'s\n", program, argc-1);
	exit(10);
    }
    for (i=0; i < argc; ++i) {
	/* - means stdin */
	if (strcmp(argv[i], "-") == 0) {
	    stream[i] = stdin;
	} else {
	    stream[i] = fopen(argv[i], "r");
	    if (stream[i] == NULL) {
		fprintf(stderr, "%s: fopen of %s: ", program, argv[i+1]);
		perror(NULL);
		exit(1);
	    }
	}
    }

    /*
     * read blocks in each file until all are EOF
     */
    opencnt = argc;
    do {

	/* clear the next output buffer */
	memset(out, 0, sizeof(out));
	outcnt = 0;

	/*
	 * read each open file
	 */
	for (i=0; i < argc; ++i) {

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
		exit(11);
	    }
	}
    } while (opencnt > 0);

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0);
}
