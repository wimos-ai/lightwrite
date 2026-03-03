#include "buffer.hpp"

#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

std::ostream &operator<<(std::ostream &os, const Buffer::Line &a)
{
    os << "{buffer: [" << a.buffer << "] size: " << a.buffer.size() << " cursor: " << a.cursor << "}";
    return os;
}

TEST(ConvTest_LN, BasicConstruction)
{
    const char *msg = "Hello";
    Buffer::Line line(msg);

    Buffer::Line l2;
    l2.buffer = msg;
    l2.cursor = 0;

    EXPECT_EQ(line, l2);
}

TEST(ConvTest_LN, InsertCursor_CZero)
{
    const char *msg = "Hello";
    const char *msg2 = "World";

    Buffer::Line line(msg);
    line.ins_cursor(msg2);

    EXPECT_EQ(line.buffer, "WorldHello");
    EXPECT_EQ(line.cursor, strlen(msg2));
}

TEST(ConvTest_LN, InsertCursor_CNZero)
{
    const char *msg = "Hello";
    const char *msg2 = "World";
    Buffer::Line line(msg);
    line.cursor += 3;
    line.ins_cursor(msg2);

    EXPECT_EQ(line.buffer, "HelWorldlo");
    EXPECT_EQ(line.cursor, strlen(msg2) + 3);
}

TEST(ConvTest_LN, InsertCursor_End)
{
    const char *msg = "Hello";
    Buffer::Line line(msg);
    line.cursor += strlen(msg);

    const char *msg2 = "World";
    line.ins_cursor(msg2);

    EXPECT_EQ(line.buffer, "HelloWorld");
    EXPECT_EQ(line.cursor, strlen(msg2) + strlen(msg));
}

TEST(ConvTest_LN, DelCursor_CZ)
{
    const char *msg = "Hello";
    Buffer::Line line2(msg);
    line2.del_cursor();

    EXPECT_EQ(line2.buffer, "Hello");
}

TEST(ConvTest_LN, DelCursor_CNZ)
{
    const char *msg = "Hello";
    Buffer::Line line(msg);
    line.cursor += 1;
    line.del_cursor();

    EXPECT_EQ(line.buffer, "ello");
}

TEST(ConvTest_LN, DelCursor_END)
{
    const char *msg = "Hello";
    Buffer::Line line(msg);

    line.cursor += strlen(msg);

    line.del_cursor();

    EXPECT_EQ(line.buffer, "Hell");
}

TEST(ConvTest_LN, DEL)
{
    const char *msg = "Hello";
    Buffer::Line line(msg);

    line.del();

    EXPECT_EQ("ello", line.buffer);
}

std::ostream &operator<<(std::ostream &os, const Buffer &b)
{
    os << "{\n";
    os << "lines: " << '\n';
    for (const auto &line : b.lines)
    {
        os << line << '\n';
    }
    os << "cursor: " << b.cursor;
    os << "}\n";
    return os;
}



TEST(ConvTest_Buffer, Buffer_push_ln)
{
    Buffer ctxt2;
    ctxt2.push_line();

    EXPECT_EQ(ctxt2.lines.size(), 2);
}


TEST(ConvTest_Buffer, Buffer_to_from_file)
{
    auto msg = "Hello";
    Buffer b1;
    b1.ins_cursor(msg);

    auto fname = "./file_test.txt";
    FILE *f = fopen(fname, "w+");
    b1.write(f, fname);
    fclose(f);

    f = fopen(fname, "r");
    Buffer b2{Buffer::read(f)};
    EXPECT_EQ(b1.lines.size(), b2.lines.size());

    for (size_t i = 0; i < b1.lines.size(); i++)
    {
        EXPECT_EQ(b1.lines[i].buffer, b2.lines[i].buffer);
    }
}