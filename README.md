# UNCRC

## Description

The The `uncrc` program matches the ending to the text file to match the checksum program matches the ending to the text file to match the checksum.

## Building

```sh
$ mkdir build
$ cd build
$ cmake ..
$ make
```

> Attention, high CPU load!

## How to use

### Print usage

```sh
$ ./uncrc

Usage: uncrc [<s:unhash>] <s:origin> [<u:leavel>]

$
```

### Show file checksum

```sh
$ ./uncrc file

[crc32           = e6483c91]

$
```

### Find checksum 

> The file "current_file.crc" will be created in which a sequence of bytes will be added, giving the checksum coincidence with the original

```sh
$ ./uncrc current_file origin_file

[unhash filename = "../current"]
[ogigin filename = "../origin"]
[input leavel    = 4]
[needed crc32    = 2be5399b]
[currnt crc32    = e6483c91]
[threads         = 6]
..................................
..................................
..................................
..................................
..................................
..................................
................................
[found index     = 0]
[append length   = 4]
[new file name   = "../current.crc"]
[result          = done]

$
```

## Additional Information

The program is written for use with BASH scripts.
To safely pad the script with bytes at the end of the file,
which may be ambiguously perceived by the interpreter,
should be commented out with a multi-line comment the remaining
part of the file.

```sh
$ cat ../test_script.sh

#!/bin/bash
...
clear
exit 0
: <<'END'

$
```



