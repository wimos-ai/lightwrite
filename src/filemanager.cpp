#include "filemanager.hpp"
#include "logger.h"

#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

FileManager::FileManager(const char *dir)
{
    fs::path root{dir};

    for (const auto &entry : fs::directory_iterator(root))
    {
        if (entry.is_regular_file())
        {
            fs::path relative = fs::relative(entry.path(), root);
            files.emplace_back();
            files.back().name = relative;
            files.back().ptr = fopen(relative.c_str(), "r");
        }
    }
}

bool FileManager::create_f(const char *name)
{
    if (!name || strlen(name) == 0)
    {
        return false;
    }

    for (const auto &f : this->files)
    {
        if (f.name == name)
        {
            return false;
        }
    }

    FILE *fp = fopen(name, "w+");
    if (!fp)
    {
        LOG_ERROR("Failed to create: %s", name);
        return false;
    }

    this->files.emplace_back(name, fp);
    return true;
}