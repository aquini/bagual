#ifndef __TERMINAL_H
#define __TERMINAL_H

void terminal_putchar(char c);
void terminal_initialize(void);
void terminal_writestring(const char* data);
void cprintf(char *fmt, ...);

#endif /* __TERMINAL_H */
