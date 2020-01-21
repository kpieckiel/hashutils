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

#include "utils.hpp"

#ifdef _WIN32
#include <Windows.h>

WORD wOldColorAttrs;

void console_format_error(bool error_stream)
{
    HANDLE hOut = GetStdHandle(error_stream ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    GetConsoleScreenBufferInfo(hOut, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;

    SetConsoleTextAttribute(hOut, FOREGROUND_RED);
}
void console_format_sucess(bool error_stream)
{
    HANDLE hOut = GetStdHandle(error_stream ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

    GetConsoleScreenBufferInfo(hOut, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;

    SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
}
void console_format_fail(bool error_stream)
{
    console_format_error(error_stream);
}
void console_format_reset(bool error_stream)
{
    HANDLE hOut = GetStdHandle(error_stream ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hOut, wOldColorAttrs);
}
#else
#include <unistd.h>

#define OUTPUT(output)                                   \
    if (!isatty(fileno(error_stream ? stderr : stdout))) \
        return;                                          \
    if (error_stream)                                    \
        std::cerr << (output);                           \
    else                                                 \
        std::cout << (output);

void console_format_error(bool error_stream)
{
    OUTPUT("\x1b[1;31m");
}
void console_format_sucess(bool error_stream)
{
    OUTPUT("\x1b[32m");
}
void console_format_fail(bool error_stream)
{
    OUTPUT("\x1b[31m");
}
void console_format_reset(bool error_stream)
{
    OUTPUT("\x1b[0m");
}
#endif

void print_check_success(const std::string& file)
{
    std::cout << file << ": ";
    console_format_sucess();
    std::cout << "SUCCESS";
    console_format_reset();
    std::cout << std::endl;
}
void print_check_fail(const std::string& file)
{
    std::cout << file << ": ";
    console_format_fail();
    std::cout << "FAIL";
    console_format_reset();
    std::cout << std::endl;
}
void print_digest(const std::string& digest, const std::string& file, bool binary_mode, bool new_line)
{
    std::cout << digest;

    if (binary_mode) {
        std::cout << " *";
    } else {
        std::cout << "  ";
    }

    std::cout << file;
    if (new_line) {
        std::cout << "\n";
    }
}
void print_error(const std::string& message)
{
    console_format_error(true /* error_stream */);
    std::cerr << "error:";
    console_format_reset(true /* error_stream */);
    std::cerr << " " << message << std::endl;
    std::exit(EXIT_FAILURE);
}

void print_help(const char* program, const char* description)
{
    std::cout << description << "\n\n"
              << "Usage\n"
              << "    " << program << " [options] [filenames]...\n"
              << "    " << program << " -c [checksum file]\n"
              << "    " << program << " --check [checksum file]\n"
              << "\n"
              << "Options\n"
              << "    -h, --help          displays this message\n"
              << "    -v, --version       displays this program version\n"
              << "    -b, --binary        reads input files in binary mode\n"
              << "    -c, --check         checking mode\n"
              << std::endl;
    std::exit(EXIT_FAILURE);
}
void print_version(const char* program, const char* version, const char* authors)
{
    std::cout << program << " v" << version << "\n"
              << "Released into the public domain\n"
              << "Written by " << authors << "\n"
              << "Compiled with Crypto++ "
              << (CRYPTOPP_VERSION / 100) << "." << (CRYPTOPP_VERSION % 100 / 10) << std::endl;
    std::exit(EXIT_FAILURE);
}

std::vector<checksum_entry_t> read_cheksum(const char* path, std::size_t digest_size)
{
    std::vector<checksum_entry_t> entries;
    std::ifstream file;
    file.open(path);
    if (file.is_open()) {
        int line_number = 1;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            bool binary_mode = false;
            std::string filename;
            std::string checksum;
            std::size_t i;
            for (i = 0; i < line.size(); ++i) {
                if (!std::isxdigit(line[i])) {
                    checksum = line.substr(0, i);
                    if (checksum.empty()) {
                        print_error("no checksum at line " + std::to_string(line_number));
                    } else if (checksum.size() != digest_size * 2) {
                        print_error("invalid checksum at line " + std::to_string(line_number));
                    }

                    break;
                }
            }

            if (i + 1 > line.size()) {
                print_error("invalid format at line " + std::to_string(line_number));
            }

            if (line[i] == ' ') {
                if (line[i + 1] == '*') {
                    binary_mode = true;
                } else if (line[i + 1] != ' ') {
                    print_error("invalid format at line " + std::to_string(line_number) + " in '");
                }
            }

            filename = line.substr(i + 2);
            if (filename.empty()) {
                print_error("invalid filename at line " + std::to_string(line_number));
            }

            std::vector<CryptoPP::byte> checksum_bytes;
            CryptoPP::HexDecoder decoder;
            decoder.Put((CryptoPP::byte*)checksum.data(), checksum.size());
            decoder.MessageEnd();
            checksum_bytes.resize(decoder.MaxRetrievable());
            decoder.Get(checksum_bytes.data(), checksum_bytes.size());

            checksum_entry_t entry;
            entry.filename = filename;
            entry.checksum = checksum_bytes;
            entry.binary_mode = binary_mode;
            entries.push_back(entry);

            ++line_number;
        }
    } else {
        print_error("could not open `" + std::string(path) + "`");
    }

    return entries;
}
