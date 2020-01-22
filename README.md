# hashutils [![Build Status](https://travis-ci.org/hgruniaux/hashutils.svg?branch=master)](https://travis-ci.org/hgruniaux/hashutils)

`hashutils` provides multi-platform command-line programs to hash and 
checksum files using multiple different algorithms from the C++ library
[Crypto++](https://cryptopp.com/). Each command-line program takes the
name of the used hash algorithm (e.g. `md5` or `sha256`), and implements
the same interface as the `sha1sum` program found on some POSIX systems.

## Implemented hash algorithms

- MD2 (program `md2`)
- MD4 (program `md4`)
- MD5 (program `md5`)
- SHA-1 (program `sha1`)
- SHA-224 (program `sha224`)
- SHA-256 (program `sha256`)
- SHA-384 (program `sha384`)
- SHA-512 (program `sha512`)
- SHAKE-128 (program `shake128`)
- SHAKE-256 (program `shake256`)

## Usage

### Generate hash from files

```shell
# Generates the MD5 hash from the given file
$ md5 myfile.data
771f5924706521c73464341fc48afb05  myfile.data
```

```shell
# Generates the SHA-256 hash from the given file read in binary mode
$ sha256 -b myfile.data
771f5924706521c73464341fc48afb05 *myfile.data
```

```shell
# Same as the previous example
$ md5 --binary myfile.data
771f5924706521c73464341fc48afb05 *myfile.data
```

```shell
# Generates the SHA-1 hash from the given files and output the result to
# the 'MD5SUM' file (if your shell support redirection)
$ sha1 myfile1.txt myfile2.html myfile3.png > MD5SUM
```

### Check files integrities

If you have a file `MD5SUM` file containing:
```
0cc175b9c0f1b6a831c399e269772661  .clang-format
92eb5ffee6ae2fec3ad71c777531578f *.editorconfig
4a8a08f09d37b73795649038408b5f33  CMakeLists.txt
8277e0910d750195b448797616e091ad *.travis.yml
```

You can do:
```shell
# Or md5 --check MD5SUM
$ md5 -c MD5SUM
.clang-format: SUCCESS
.editorconfig: SUCCESS
CMakeLists.txt: FAIL
.travis.yml: SUCCESS
```

Which means that the `CMakeLists.txt` file do not pass the integrety test.

## Add a hash algorithm

To add support a new hash algorithm supported by the [Crypto++](https://cryptopp.com/)
library is very easy.

- Add a new file to `src/` with the following content:
    ```cpp
    #include "utils.hpp"

    #include <cryptopp/The Correct Header Here.h>

    PROGRAM("The Algorithm Name Here");
    AUTHORS("Your Name Here");
    VERSION("1.0.0");
    DESCRIPTION("The Program Description Here");
    IMPLEMENTATION(The Crypto++ class implementing the hash algorithm here)
    ```
  See the other files in `src/` for examples.
- Replace the placeholders by the correct values
- Add an entry in the `CMakeLists.txt` file
- End!

## License

This project itself is released into the public domain. However, the underlying 
C++ library [Crypto++](https://cryptopp.com/) binaries is license under the
Boost Software License 1.0.
