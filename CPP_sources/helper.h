#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string>

void clear_buffer(uint32_t size, uint8_t *buffer);
bool bittest(int32_t value, int32_t bit);
uint8_t BYTE2(uint32_t value);
int get_distance(int delta_x, int delta_y);
long get_file_size(FILE *fp);
std::string to_lower(const std::string &str);
std::string to_upper(const std::string &str);

template<class T> T set_bit(T number, uint32_t bit)
{
    return number | 1UL << bit;
}

template<class T> T reset_bit(T number, uint32_t bit)
{
    return number & ~(1UL << bit);
}

template<class T> bool check_bit(T number, uint32_t bit)
{
    return (number >> bit) & 1U;
}
