#include "buffer.hpp"
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iterator>

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

std::ostream &operator<<(std::ostream &os, const Buffer::Line &a)
{
    os << "{buffer: [" << a.buffer << "] size: " << a.buffer.size() << " cursor: " << a.cursor << "}";
    return os;
}

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

size_t Buffer::Line::size() const
{
    return buffer.size();
}

Buffer::Line::operator std::string_view() const
{
    return buffer;
}

void Buffer::Line::reset_cursor()
{
    cursor = 0;
}

void Buffer::Line::end_cursor()
{
    cursor = buffer.size();
}

Buffer::Line::Line(const char *msg) : buffer(msg)
{
}

void Buffer::Line::del()
{
    if (buffer.size() > 0 && cursor <= buffer.size())
    {
        buffer.erase(this->cursor, 1);
    }
}

std::pair<std::string_view, std::string_view::iterator> Buffer::Line::get_render_window(int num_chars) const
{
    if (buffer.size() == 0)
    {
        std::string_view sv;
        return {sv, sv.end()};
    }

    if (num_chars > buffer.size())
    {
        std::string_view sv{buffer};
        return {sv, sv.begin() + cursor};
    }

    auto cursor_itr = this->buffer.begin() + this->cursor;
    auto begin = cursor_itr;
    auto end = cursor_itr;

    auto count{0};
    while (count < num_chars)
    {
        if (count < num_chars && end != buffer.end())
        {
            end++;
            count++;
        }
        if (count < num_chars && begin != buffer.begin())
        {
            begin--;
            count++;
        }
    }

    std::string_view sv{begin, end};
    auto d_c = std::distance(begin, cursor_itr);
    return {sv, sv.begin() + d_c};
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
