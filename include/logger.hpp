#pragma once

#include "SDL.h"
#include "SDL_ttf.h"

enum Log_Level
{
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,

    LOG_LEVEL_MAX_TAG
};

void logger_print(Log_Level level, const char *file, int line, const char *message, ...);

#ifndef LOG_FATAL
#define LOG_FATAL(...) logger_print(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#endif // !LOG_FATAL

#ifndef LOG_ERROR
#define LOG_ERROR(...) logger_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#endif // !LOG_ERROR

#ifndef LOG_SDL_ERROR
#define LOG_SDL_ERROR(A, ...)                                                                \
    do                                                                                       \
    {                                                                                        \
        if (A != 0)                                                                          \
        {                                                                                    \
            logger_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, "%s: %s", #A, SDL_GetError()); \
        }                                                                                    \
    } while (0)
#endif // !LOG_ERROR

#ifndef LOG_TTF_ERROR
#define LOG_TTF_ERROR(A, ...)                                                                \
    do                                                                                       \
    {                                                                                        \
        if (A != 0)                                                                          \
        {                                                                                    \
            logger_print(LOG_LEVEL_ERROR, __FILE__, __LINE__, "%s: %s", #A, TTF_GetError()); \
        }                                                                                    \
    } while (0)
#endif // !LOG_ERROR

#ifndef LOG_WARN
#define LOG_WARN(...) logger_print(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#endif // !LOG_WARN

#ifdef _DEBUG
#ifndef LOG_DEBUG
#define LOG_DEBUG(...) logger_print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#endif // !LOG_DEBUG
#else
#define LOG_DEBUG(...)
#endif // _DEBUG

#ifndef LOG_INFO
#define LOG_INFO(...) logger_print(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#endif // !LOG_INFO
