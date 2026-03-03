#include "filemanager.hpp"
#include "filemanager.h"

#include <cstring>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(FileManagerTest, testFileFinding)
{
    FileManager mgr{"."};
    File_Manager mgr2;
    fileman_init(&mgr2);

    std::vector<std::string> fnames;
    for (const auto &f : mgr.files)
    {
        fnames.emplace_back(f.name);
    }

    std::vector<std::string> fnames2;
    for (size_t i = 0; i < mgr2.size; i++)
    {
        fnames2.emplace_back(mgr2.files[i].name);
    }
    std::sort(fnames.begin(), fnames.end());
    std::sort(fnames2.begin(), fnames2.end());

    EXPECT_EQ(fnames, fnames2);
}
