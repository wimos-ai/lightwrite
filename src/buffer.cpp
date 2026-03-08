#include "buffer.hpp"
#include <cstring>
#include <fstream>

void Buffer::Line::ins_cursor(const char *text)
{
    buffer = buffer.insert(cursor, text);
    cursor += std::strlen(text);
}

void Buffer::Line::del_cursor()
{
    if (this->cursor > 0 && this->cursor <= buffer.size())
    {
        buffer.erase(this->cursor - 1, 1);
        this->cursor -= 1;
    }
}

Buffer::Line::Line(const char *msg) : buffer(msg)
{
}

void Buffer::Line::del()
{
    if (buffer.size() > 0)
    {
        buffer.erase(0, 1);
        this->cursor--;
    }
}

Buffer::Buffer()
{
    lines.push_back({});
}

void Buffer::push_line()
{
    lines.push_back({});
    cursor++;
}

void Buffer::ins_cursor(const char *text)
{
    lines[cursor].ins_cursor(text);
}

void Buffer::del_cursor()
{
    lines[cursor].del_cursor();
}

void Buffer::del()
{
    lines[cursor].del();
}

size_t Buffer::get_cursor_row()
{
    return lines[cursor].cursor;
}

Buffer Buffer::read(const char *filename)
{
    std::vector<std::string> lines;
    std::string line;
    std::ifstream fi(filename);
    while (std::getline(fi, line))
    {
        lines.emplace_back(std::move(line));
    }

    Buffer b;
    b.lines = std::vector<Line>(lines.size()+1);
    for (size_t i = 0; i < lines.size(); i++)
    {
        b.lines[i].buffer = std::move(lines[i]);
    }
    return b;
}

bool Buffer::write(const char *filename)
{
    std::ofstream of(filename);

    for (const auto& line : this->lines)
    {
        of << line.buffer << std::endl;
    }    

    return true;
}
