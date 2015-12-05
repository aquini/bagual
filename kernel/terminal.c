#include "stdlib.h"
#include "stdarg.h"
#include "types.h"
#include "i386.h"

#define VIDEO_CTRL_PORT		0x000003D4
#define VIDEO_DATA_PORT		0x000003D5
#define VIDEO_MEM_ADDR		0x000B8000

/* Hardware text mode color constants. */
enum vga_color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;


static void terminal_update_cursor(size_t x, size_t y)
{
	uint16_t cursor_loc = y * VGA_WIDTH + x;

	outb(VIDEO_CTRL_PORT, 14);
	outb(VIDEO_DATA_PORT, cursor_loc >> 8);
	outb(VIDEO_CTRL_PORT, 15);
	outb(VIDEO_DATA_PORT, cursor_loc);
}

void terminal_clrscr(uint16_t *buffer)
{
	uint32_t flags = local_irq_save();

	memsetw((unsigned short *)buffer,
		(0x20 | (terminal_color << 8)), VGA_WIDTH * VGA_HEIGHT);

	local_irq_restore(flags);
}

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = (uint16_t *)VIDEO_MEM_ADDR;
	terminal_clrscr(terminal_buffer);
	terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_scroll(void)
{
	size_t x, y;
	/* moves the screen text buffer back by a line */
	for (x = 0; x < ((VGA_HEIGHT - 1) * VGA_WIDTH); x++)
		terminal_buffer[x] = terminal_buffer[x + VGA_WIDTH];

	/* wipe out the last line of our current screen text buffer */
	for (y = ((VGA_HEIGHT - 1) * VGA_WIDTH);
	     y < (VGA_HEIGHT * VGA_WIDTH); y++)
		terminal_buffer[y] = make_vgaentry(0x20, terminal_color);

	terminal_row = VGA_HEIGHT - 1;
}

void terminal_putchar(char c)
{
	switch (c) {
	case '\n':
		terminal_row++;
	case '\r':
		terminal_column = 0;
		break;
	case '\t':
		terminal_column += 8;
		break;
	default:
		terminal_putentryat(c, terminal_color,
				    terminal_column, terminal_row);

		if (terminal_column++ == VGA_WIDTH) {
			terminal_column = 0;
			terminal_row++;
		}
	}

	if (terminal_row == VGA_HEIGHT)
		terminal_scroll();

	terminal_update_cursor(terminal_column, terminal_row);
}

void terminal_writestring(const char* data)
{
	size_t i, datalen = strlen(data);
	for (i = 0; i < datalen; i++)
		terminal_putchar(data[i]);
}

void cprintf(char *fmt, ...)
{
        char buf[1024];
        va_list args;

        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

	terminal_writestring(buf);
}
