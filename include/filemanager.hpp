#pragma once

#include <cstdio>
#include <string>
#include <vector>
#include <dirent.h>

class FileManager
{
public:
    FileManager(const char *dir);

public:
    bool create_f(const char *name);

public:
    class File
    {
    public:
        File() = default;
        inline File(const char *name_, FILE *file) : name(name_), ptr(file) {};

    public:
        std::string name;
        FILE *ptr{nullptr};
    };

    std::vector<File> files;
};