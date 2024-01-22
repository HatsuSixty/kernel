#include <stdint.h>
#include <stddef.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GREY,
    VGA_COLOR_DARK_GREY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_LIGHT_BROWN,
    VGA_COLOR_WHITE,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(char const* str)
{
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

class Terminal {
private:
    size_t terminalRow;
    size_t terminalColumn;
    uint8_t terminalColor;
    uint16_t* terminalBuffer;
public:
    Terminal();
    void setColor(uint8_t color);
    void putEntryAt(char c, uint8_t color, size_t x, size_t y);
    void putChar(char c);
    void terminalWrite(const char* data, size_t size);
    void writeString(const char* data);
};

Terminal::Terminal()
{
    terminalRow = 0;
    terminalColumn = 0;
    terminalColor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminalBuffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminalBuffer[index] = vga_entry(' ', terminalColor);
        }
    }
}

void Terminal::setColor(uint8_t color)
{
    terminalColor = color;
}

void Terminal::putEntryAt(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminalBuffer[index] = vga_entry(c, color);
}

void Terminal::putChar(char c)
{
    if (c == '\n') {
        terminalColumn = 0;
        if (++terminalRow == VGA_HEIGHT) {
            terminalRow = 0;
        }
    } else {
        if (terminalColumn >= VGA_WIDTH) {
            terminalColumn = 0;
            if (++terminalRow == VGA_HEIGHT) {
                terminalRow = 0;
            }
        }

        if (terminalRow < VGA_HEIGHT) {
            putEntryAt(c, terminalColor, terminalColumn, terminalRow);
            terminalColumn += 1;
        }
    }
}

void Terminal::terminalWrite(const char* data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        putChar(data[i]);
}

void Terminal::writeString(const char* data)
{
    terminalWrite(data, strlen(data));
}

extern "C" [[noreturn]] void init()
{
    Terminal terminal;

    terminal.writeString("Welcome to Simple Kernel!\n");
    terminal.writeString("This is a very useful log\n");
    terminal.writeString("Very cool");
    terminal.writeString(" log\n");
    terminal.writeString("More logs\nM O R E   L O G S\n");

    asm("cli");
    while(true) asm("hlt");
}
