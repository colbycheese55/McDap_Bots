#pragma once
#include <stdint.h>
void DBG_init(void);
void DBG_putc(char c);
void DBG_write(const char *s);
void DBG_hex(const uint8_t *b, uint32_t n);

