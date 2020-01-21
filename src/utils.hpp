/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <https://unlicense.org>
 */

#ifndef __COREUTILS_UTILS_HPP__
#define __COREUTILS_UTILS_HPP__
#pragma once

// Some hashing algorithms used are obsolete from a security or efficiency 
// point of view (e.g. MD5, MD4). Crypto++ issues a warning for these algorithms
// if the following macro is not defined, and the Weak:: namespace is not used.
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/files.h>
#include <cryptopp/hex.h>

// Declares the program name (as displayed in the version message).
#define PROGRAM(name) constexpr auto kMetaDataProgram = name
// Declares the program authors (as displayed in the version message).
#define AUTHORS(authors) constexpr auto kMetaDataAuthors = authors
// Declares the program version (as displayed in the version message).
#define VERSION(version) constexpr auto kMetaDataVersion = version
// Declares the program description (as displayed in the help message).
#define DESCRIPTION(description) constexpr auto kMetaDataDescription = description;

// Changes the console output format to the error format (red foreground).
// If 'error_stream', this change the format of the standard error output,
// otherwise, this change the format of the standard output.
void console_format_error(bool error_stream = false);
// Changes the console output format to the success format (green foreground).
// If 'error_stream', this change the format of the standard error output,
// otherwise, this change the format of the standard output.
void console_format_sucess(bool error_stream = false);
// Changes the console output format to the fail format (red foreground).
// If 'error_stream', this change the format of the standard error output,
// otherwise, this change the format of the standard output.
void console_format_fail(bool error_stream = false);
// Resets the console output format.
// If 'error_stream', this change the format of the standard error output,
// otherwise, this change the format of the standard output.
void console_format_reset(bool error_stream = false);

// Prints '{filename}: SUCCESS', using the correct colors, and putting
// a new-line.
void print_check_success(const std::string& filename);
// Prints '{filename}: FAIL', using the correct colors, and putting
// a new-line.
void print_check_fail(const std::string& filename);
// Prints the hash digest with its associated filename.
void print_digest(const std::string& digest, const std::string& filename, bool binary_mode, bool new_line);
// Prints an error message with the correct colors.
void print_error(const std::string& message);

// Prints the help message.
void print_help(const char* program, const char* description);
// Prints the version message.
void print_version(const char* program, const char* version, const char* authors);

#define PRINT_HELP() print_help(argv[0], kMetaDataDescription)
#define PRINT_VERSION() print_version(kMetaDataProgram, kMetaDataVersion, kMetaDataAuthors)

struct checksum_entry_t {
    std::string filename;
    std::vector<CryptoPP::byte> checksum;
    bool binary_mode;
}; /* struct checksum_entry_t */

std::vector<checksum_entry_t> read_cheksum(const char* path, std::size_t digest_size);

inline bool check_option(const char* arg, const char* short_opt, const char* long_opt)
{
    return std::strcmp(arg, short_opt) == 0 || std::strcmp(arg, long_opt) == 0;
}

template <typename Algorithm>
inline bool check_digest(const char* checksum_file)
{
    using namespace CryptoPP;

    bool success = true;

    auto entries = read_cheksum(checksum_file, Algorithm::DIGESTSIZE);
    for (const auto& entry : entries) {
        if (entry.checksum.size() != Algorithm::DIGESTSIZE) {
            success = false;
            print_check_fail(entry.filename);
            continue;
        }

        std::vector<CryptoPP::byte> digest;
        Algorithm hash;
        digest.resize(Algorithm::DIGESTSIZE);
        FileSource(entry.filename.data(), /* pumpAll= */ true, new HashFilter(hash, new ArraySink(digest.data(), digest.size())), entry.binary_mode);

        if (std::memcmp(entry.checksum.data(), digest.data(), digest.size()) == 0) {
            print_check_success(entry.filename);
        } else {
            success = false;
            print_check_fail(entry.filename);
            continue;
        }
    }

    return success;
}

template <typename Algorithm>
inline void dump_digest(const char* path, bool binary_mode, bool new_line)
{
    using namespace CryptoPP;

    std::string digest;
    std::string output;
    Algorithm hash;
    HexEncoder encoder(new StringSink(output), /* uppercase= */ false);
    FileSource(path, /* pumpAll= */ true, new HashFilter(hash, new StringSink(digest)), binary_mode);
    StringSource(digest, /* pumpAll= */ true, new Redirector(encoder));

    print_digest(output, path, binary_mode, new_line);
}

#define IMPLEMENTATION(algorithm)                                              \
    int main(int argc, char* argv[])                                           \
    {                                                                          \
        bool check_mode = false;                                               \
        const char* checksum_file = nullptr;                                   \
        bool binary_mode = false;                                              \
        int start_argc = 1;                                                    \
        if (argc < 2) {                                                        \
            return EXIT_FAILURE;                                               \
        } else {                                                               \
            if (check_option(argv[1], "-h", "--help")) {                       \
                PRINT_HELP();                                                  \
            } else if (check_option(argv[1], "-v", "--version")) {             \
                PRINT_VERSION();                                               \
            } else if (check_option(argv[1], "-c", "--check")) {               \
                if (argc == 3) {                                               \
                    check_mode = true;                                         \
                    checksum_file = argv[2];                                   \
                } else {                                                       \
                    PRINT_HELP();                                              \
                }                                                              \
            } else if (check_option(argv[1], "-b", "--binary")) {              \
                binary_mode = true;                                            \
                start_argc = 2;                                                \
            }                                                                  \
        }                                                                      \
                                                                               \
        if (check_mode) {                                                      \
            if (!check_digest<algorithm>(checksum_file)) {                     \
                return EXIT_FAILURE;                                           \
            }                                                                  \
        } else {                                                               \
            for (int i = start_argc; i < argc; ++i) {                          \
                dump_digest<algorithm>(argv[i], binary_mode, i != (argc - 1)); \
            }                                                                  \
        }                                                                      \
                                                                               \
        return EXIT_SUCCESS;                                                   \
    }

#endif /* !__COREUTILS_UTILS_HPP__ */
