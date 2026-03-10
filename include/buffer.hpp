#pragma once

#include <string>
#include <vector>

class Buffer
{
public:
    Buffer();
    Buffer(const Buffer &) = default;
    Buffer(Buffer &&) = default;
    Buffer &operator=(const Buffer &other) = default;
    Buffer &operator=(Buffer &&other) = default;

public:
    class Line
    {
    public:
        Line() = default;
        Line(const char *msg);

    public:
        auto operator<=>(const Line &) const = default;

    public:
        void ins_cursor(const char *text);
        void del_cursor();
        void del();

    public:
        std::string buffer;
        std::size_t cursor{0};
    };

public:
    void push_line();
    void ins_cursor(const char *text);
    void del_cursor();
    void del();
    size_t get_cursor_row();

public:
    auto operator<=>(const Buffer &) const = default;

    Line &get()
    {
        return lines[cursor];
    }

    const Line &get() const
    {
        return lines[cursor];
    }

    void up();

    void down();

    void right();

    void left();

    void scroll(int nlines);

public:
    std::vector<Line>::iterator get_active_line();
    std::pair<std::vector<Line>::iterator, std::vector<Line>::iterator> get_render_zone(int nlines);

public:
    static Buffer read(const char *filename);
    bool write(const char *filename);

public:
    std::vector<Line> lines;
    std::size_t cursor{0};
};
