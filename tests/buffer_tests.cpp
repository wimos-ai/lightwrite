#include "buffer.hpp"

#include <cstring>
#include <iostream>

#include <gtest/gtest.h>



TEST(ConvTest_Buffer, Buffer_push_ln)
{
    Buffer ctxt2;
    ctxt2.push_line();
    auto [a, b] = ctxt2.get_render_zone(1000);

    EXPECT_EQ(std::distance(a, b), 2);
}
