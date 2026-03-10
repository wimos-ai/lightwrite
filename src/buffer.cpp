#include "buffer.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>

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

void Buffer::up()
{
    if (cursor > 0)
    {
        cursor--;
    }
}

void Buffer::down()
{

    if (cursor < (lines.size() - 1))
    {
        cursor++;
    }
}

void Buffer::left()
{
    if (this->get_cursor_row() > 0)
    {
        lines[cursor].cursor--;
    }
}

void Buffer::scroll(int nlines)
{
    int cf = std::clamp((int)cursor + nlines, 0, (int)lines.size() - 1);
    cursor = cf;
}

std::vector<Buffer::Line>::iterator Buffer::get_active_line()
{
    return lines.begin() + cursor;
}

std::pair<std::vector<Buffer::Line>::iterator, std::vector<Buffer::Line>::iterator> Buffer::get_render_zone(int nlines)
{
    if (nlines <= 0)
    {
        return std::make_pair(lines.end(), lines.end());
    }

    if (nlines >= lines.size())
    {
        nlines = lines.size();
    }
    auto begin = get_active_line();
    auto end = get_active_line();
    auto count{0};
    while (count < nlines)
    {
        if (count < nlines && end != lines.end())
        {
            end++;
            count++;
        }
        if (count < nlines && begin != lines.begin())
        {
            begin--;
            count++;
        }
    }

    return std::make_pair(begin, end);
}

void Buffer::right()
{
    if (this->get_cursor_row() < lines[cursor].buffer.size())
    {
        lines[cursor].cursor++;
    }
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
    b.lines = std::vector<Line>(lines.size());
    for (size_t i = 0; i < lines.size(); i++)
    {
        b.lines[i].buffer = std::move(lines[i]);
    }
    if (b.lines.size() == 0)
    {
        b.lines.emplace_back();
    }
    return b;
}

bool Buffer::write(const char *filename)
{
    std::ofstream of(filename);

    for (const auto &line : this->lines)
    {
        of << line.buffer << std::endl;
    }

    return true;
}
