#pragma once

#include <stdint.h>

class cube_4{
    static volatile uint8_t layer_counter;
    static volatile uint8_t pwm_counter;
    static volatile uint8_t buffer[64];
    static void set_cathode(uint8_t layer);
    static volatile uint8_t pwm_resolution;
    static void _start();

public:
    void start();
    void clear();
    void set_all(uint8_t value);
    void set(unsigned int x, unsigned int y, unsigned int z, uint8_t value);
    void set_pwm_resolution(uint8_t resolution);
    void get_pwm_resolution(uint8_t resolution);
    uint8_t* get_buffer();
    static inline void timer_isr();
};
