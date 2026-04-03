#include "logger.hpp"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <chrono>

static const char level_str[LOG_LEVEL_MAX_TAG][32] = {
    "\033[1;31m[FATAL]: ",
    "\033[1;35m[ERROR]: ",
    "\033[1;33m[WARNING]: ",
    "\033[1;32m[DEBUG]: ",
    "\033[1;37m[INFO]: "};

static const auto exe_start_time = std::chrono::steady_clock::now();

void logger_print(Log_Level level, const char *file, int line, const char *message, ...)
{
    const size_t message_len = 10000;
    char msg[message_len]{};

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(msg, message_len, message, arg_ptr);
    va_end(arg_ptr);

    auto time = std::chrono::steady_clock::now();
    std::chrono::duration<double> dt = time - exe_start_time;

    printf("%s at %fs in %s:%d %s\033[0m\n", level_str[level], dt.count(), file, line, msg);
}
