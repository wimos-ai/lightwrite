#include "buffer.hpp"
#include <cstring>

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

Buffer Buffer::read(FILE *file)
{
    char *line = NULL;
    size_t length = 0;
    std::vector<std::string> lines;
    while (getline(&line, &length, file) != -1)
    {
        lines.emplace_back(line);
        if (lines.back().size() > 0 && lines.back().back() == '\n')
        {
            lines.back().pop_back();
        }

        free(line);
        line = nullptr;
    }
    if (line != nullptr)
    {
        free(line);
    }

    Buffer b;
    b.lines = std::vector<Line>(lines.size());
    for (size_t i = 0; i < lines.size(); i++)
    {
        b.lines[i].buffer = std::move(lines[i]);
    }
    return b;
}

bool Buffer::write(FILE *file, const char *filename)
{
    // closing unopened files will seg-fault
    if (file)
    {
        fclose(file);
    }

    // Open the file for writing (and create it if it doesn't already exist)
    file = fopen(filename, "w+");
    // Check for errors anyways, since it may want to create a file in write-protected area or any other error may occour.
    if (!file)
    {
        printf("Failed to open/create: %s\n", filename);
        return false;
    }
    size_t i;
    for (i = 0; i < this->lines.size() - 1; ++i)
    {
        const char *line = this->lines[i].buffer.c_str();

        fprintf(file, "%s\n", line);
    }
    fprintf(file, "%s", this->lines[i].buffer.c_str());

    return true;
}
