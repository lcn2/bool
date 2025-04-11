# bool

Produce the logical op (and, or, xor, nand, nor xnor) of files.

* add - local and files

* or - local or files

* xor - local xor files

* nand - local nand files

* nor - local nor files

* xnor - local xnor files


# To install

```sh
make clobber all
sudo make install clobber
```


# Examples

```sh
/usr/local/src/bin/xor foo bar | hexdump
```


# To use

## add

```
/usr/local/bin/and [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

and version: 1.4.1 2025-04-11
```


## or

```
/usr/local/bin/or [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

or version: 1.4.1 2025-04-11
```


## xor

```
/usr/local/bin/xor [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

xor version: 1.4.1 2025-04-11
```


## nand

```
/usr/local/bin/nand [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

nand version: 1.4.1 2025-04-11
```


## nor

```
/usr/local/bin/nor [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

nor version: 1.4.1 2025-04-11
```


## xnor

```
/usr/local/bin/xnor [-h] level] [-V] [-n]
    [-c] [-o offset] file ...

    -h            print help message and exit
    -V            print version string and exit

    file ...      file(s) to process, - ==> read stdin

Exit codes:
    0         all OK
    1         failed to open or read a file
    2         -h and help string printed or -V and version string printed
    3         command line error
    4         program name isn't: and, or, xor, nand, nor or xnor
 >= 10        internal error

xnor version: 1.4.1 2025-04-11
```


# Reporting Security Issues

To report a security issue, please visit "[Reporting Security Issues](https://github.com/lcn2/bool/security/policy)".
