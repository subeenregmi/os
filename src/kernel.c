#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error                                                                         \
    "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t)uc | (uint16_t)color << 8;
}

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

void terminal_initialize(void) {
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(uint8_t color) { terminal_color = color; }

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}

void clear_row(uint8_t row) {
  const size_t start = row * 80;
  for (size_t i = 0; i < VGA_WIDTH; i++) {
    terminal_buffer[start + i] = 0;
  }
}

void copy_row(uint8_t src, uint8_t dest) {
  if (src >= VGA_HEIGHT || dest >= VGA_HEIGHT) {
    terminal_buffer[0] = vga_entry('X', VGA_COLOR_RED);
    return;
  }

  const size_t src_index = src * 80;
  const size_t dest_index = dest * 80;

  for (uint8_t i = 0; i < VGA_WIDTH; i++) {
    terminal_buffer[dest_index + i] = terminal_buffer[src_index + i];
  }
}

void terminal_putchar(char c) {
  if (c == '\n') {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      clear_row(0);
      for (size_t i = 0; i < VGA_HEIGHT - 1; i++) {
        copy_row(i + 1, i);
      }
      clear_row(VGA_HEIGHT - 1);
      terminal_row--;
    }
    return;
  }

  terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
  if (++terminal_column == VGA_WIDTH) {
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT) {
      clear_row(0);
      for (size_t i = 0; i < VGA_HEIGHT - 1; i++) {
        copy_row(i + 1, i);
      }
      clear_row(VGA_HEIGHT - 1);
      terminal_row--;

      // terminal_buffer[terminal_row * 80 + terminal_column] =
      // vga_entry('X', VGA_COLOR_GREEN);
    }
  }
}

void terminal_write(const char *data, size_t size) {
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char *data) {
  terminal_write(data, strlen(data));
}

void kernel_main(void) {
  /* Initialize terminal interface */
  terminal_initialize();

  /* Newline support is left as an exercise. */
  terminal_writestring("This is line 1.\n"
                       "This is line 2.\n"
                       "This is line 3.\n"
                       "This is line 4.\n"
                       "This is line 5.\n"
                       "This is line 6.\n"
                       "This is line 7.\n"
                       "This is line 8.\n"
                       "This is line 9.\n"
                       "This is line 10.\n"
                       "This is line 11.\n"
                       "This is line 12.\n"
                       "This is line 13.\n"
                       "This is line 14.\n"
                       "This is line 15.\n"
                       "This is line 16\n"
                       "This is line 17.\n"
                       "This is line 18.\n"
                       "This is line 19.\n"
                       "This is line 20.\n"
                       "This is line 21.\n"
                       "This is line 22.\n"
                       "This is line 23.\n"
                       "This is line 24.\n"
                       "This is line 25.\n"
                       "This is line 26.\n\n"
                       "This is line 28.\n\n");
}
