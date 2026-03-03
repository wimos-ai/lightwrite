#include "buffer_.h"
#include "buffer.hpp"

#include <cstring>
#include <iostream>

#include <gtest/gtest.h>

std::ostream &operator<<(std::ostream &os, const Line &a)
{
    os << "{buffer: [" << a.buffer << "] size: " << a.size << " cursor: " << a.cursor << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const Buffer::Line &a)
{
    os << "{buffer: [" << a.buffer << "] size: " << a.buffer.size() << " cursor: " << a.cursor << "}";
    return os;
}

bool operator==(const Line &a, const Buffer::Line &b)
{
    if (a.size != b.buffer.size())
    {
        return false;
    }
    if (a.cursor != b.cursor)
    {
        return false;
    }

    auto str_eq = strncmp(a.buffer, b.buffer.c_str(), b.buffer.size());
    if (str_eq != 0)
    {
        return false;
    }
    return true;
}

bool operator==(const Buffer::Line &b, const Line &a)
{
    return a == b;
}

Line line_from_chars(const char *msg)
{
    Line line;
    std::strcpy(line.buffer, msg);
    line.cursor = 0;
    line.size = strlen(msg);
    return line;
}

TEST(ConvTest_LN, BasicConstruction)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);

    Buffer::Line line2(msg);

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, InsertCursor_CZero)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    Buffer::Line line2(msg);

    const char *msg2 = "World";
    line_ins_cursor(&line, msg2);
    line2.ins_cursor(msg2);

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, InsertCursor_CNZero)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    line.cursor += 3;
    Buffer::Line line2(msg);
    line2.cursor += 3;

    const char *msg2 = "World";
    line_ins_cursor(&line, msg2);
    line2.ins_cursor(msg2);

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, InsertCursor_End)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    line.cursor += strlen(msg);
    Buffer::Line line2(msg);
    line2.cursor += strlen(msg);

    const char *msg2 = "World";
    line_ins_cursor(&line, msg2);
    line2.ins_cursor(msg2);

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, DelCursor_CZ)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    Buffer::Line line2(msg);

    line_del_cursor(&line);
    line2.del_cursor();

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, DelCursor_CNZ)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    Buffer::Line line2(msg);

    line.cursor += 1;
    line2.cursor += 1;

    line_del_cursor(&line);
    line2.del_cursor();

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, DelCursor_END)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    Buffer::Line line2(msg);

    line.cursor += strlen(msg);
    line2.cursor += strlen(msg);

    line_del_cursor(&line);
    line2.del_cursor();

    EXPECT_EQ(line, line2);
}

TEST(ConvTest_LN, DEL)
{
    const char *msg = "Hello";
    Line line = line_from_chars(msg);
    Buffer::Line line2(msg);

    line_del(&line);
    line2.del();

    EXPECT_EQ(line, line2);
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

std::ostream &operator<<(std::ostream &os, const Buffer_Context &b)
{
    os << "{\n";
    os << "lines: " << '\n';
    for (std::size_t i = 0; i < b.size; i++)
    {
        os << b.lines[i] << '\n';
    }
    os << "cursor: " << b.cursor;
    os << "}\n";
    return os;
}

bool operator==(const Buffer_Context &a, const Buffer b)
{
    if (a.cursor != b.cursor)
    {
        return false;
    }
    if (a.size != b.lines.size())
    {
        return false;
    }
    for (size_t i = 0; i < b.lines.size(); i++)
    {
        if (a.lines[i] != b.lines[i])
        {
            return false;
        }
    }
    return true;
}

TEST(ConvTest_Buffer, Buffer_Construct)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);

    Buffer ctxt2;

    EXPECT_EQ(ctxt, ctxt2);
}

TEST(ConvTest_Buffer, Buffer_push_ln)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);
    buffer_push_line(&ctxt);

    Buffer ctxt2;
    ctxt2.push_line();

    EXPECT_EQ(ctxt, ctxt2);
}

TEST(ConvTest_Buffer, Buffer_Del)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);
    buffer_del(&ctxt);

    Buffer ctxt2;
    ctxt2.del();

    EXPECT_EQ(ctxt, ctxt2);
}

TEST(ConvTest_Buffer, Buffer_Del_Cursor)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);
    buffer_del_cursor(&ctxt);

    Buffer ctxt2;
    ctxt2.del_cursor();

    EXPECT_EQ(ctxt, ctxt2);
}

TEST(ConvTest_Buffer, Buffer_Ins_Cursor)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);
    auto msg = "Hello";
    buffer_ins_cursor(&ctxt, msg);

    Buffer ctxt2;
    ctxt2.ins_cursor(msg);

    EXPECT_EQ(ctxt, ctxt2);
}

TEST(ConvTest_Buffer, Buffer_GetCursorRow)
{
    Buffer_Context ctxt;
    buffer_init(&ctxt);
    auto msg = "Hello";
    buffer_ins_cursor(&ctxt, msg);

    Buffer ctxt2;
    ctxt2.ins_cursor(msg);
    EXPECT_EQ(buffer_get_cursor_row(&ctxt), ctxt2.get_cursor_row());

    EXPECT_EQ(ctxt, ctxt2);
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