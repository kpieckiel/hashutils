cmake_minimum_required(VERSION 3.8)

project(hashutils)

set(CMAKE_CXX_STANDARD 11)

find_package(CryptoPP REQUIRED)
link_libraries("cryptopp-static")
include_directories(${CryptoPP_INCLUDE_DIRS})

add_executable("md2" "src/utils.cpp" "src/md2.cpp")
add_executable("md4" "src/utils.cpp" "src/md4.cpp")
add_executable("md5" "src/utils.cpp" "src/md5.cpp")

add_executable("sha1" "src/utils.cpp" "src/sha1.cpp")
add_executable("sha224" "src/utils.cpp" "src/sha224.cpp")
add_executable("sha256" "src/utils.cpp" "src/sha256.cpp")
add_executable("sha384" "src/utils.cpp" "src/sha384.cpp")
add_executable("sha512" "src/utils.cpp" "src/sha512.cpp")

add_executable("sha3-224" "src/utils.cpp" "src/sha3_224.cpp")
add_executable("sha3-256" "src/utils.cpp" "src/sha3_256.cpp")
add_executable("sha3-384" "src/utils.cpp" "src/sha3_384.cpp")
add_executable("sha3-512" "src/utils.cpp" "src/sha3_512.cpp")

add_executable("shake128" "src/utils.cpp" "src/shake128.cpp")
add_executable("shake256" "src/utils.cpp" "src/shake256.cpp")
