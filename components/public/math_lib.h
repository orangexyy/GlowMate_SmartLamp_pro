#ifndef math_lib_h
#define math_lib_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MK_PI                         3.141592f
#define SOME_LARGE_VALUE              1.0E+38

// Useful macros
#define clear_vector(a)               memset(a, 0, sizeof(a))
#define clear_vector_float(a)         memset(a, 0.0, sizeof(float) * N_AXIS)
#define max(a, b)                     (((a) > (b)) ? (a) : (b))
#define min(a, b)                     (((a) < (b)) ? (a) : (b))
#define isequal_position_vector(a, b) !(memcmp(a, b, sizeof(float) * N_AXIS))

// Bit field and masking macros
#define bit(n)                        (1 << n)
#define bit_true(x, mask)             (x) |= (mask)
#define bit_false(x, mask)            (x) &= ~(mask)
#define bit_istrue(x, mask)           ((x & mask) != 0)
#define bit_isfalse(x, mask)          ((x & mask) == 0)

// Bit field and masking macros
// bitnum_to_mask(n) is similar to bit(n) as defined in Arduino.h.
// We define our own version so we can apply the static_cast, thus making it work with scoped enums,
// using a different name to avoid name conflicts and include ordering issues with Arduino.h
#define bitnum_to_mask(n)             (1 << (unsigned int)n)

#define set_bits(target, mask)        (target) |= (mask)
#define clear_bits(target, mask)      (target) &= ~(mask)
#define bits_are_true(target, mask)   ((target & (mask)) != 0)
#define bits_are_false(target, mask)  ((target & (mask)) == 0)
#define set_bitnum(target, num)       (target) |= bitnum_to_mask(num)
#define clear_bitnum(target, num)     (target) &= ~bitnum_to_mask(num)
#define bitnum_is_true(target, num)   ((target & bitnum_to_mask(num)) != 0)
#define bitnum_is_false(target, num)  ((target & bitnum_to_mask(num)) == 0)

// Conversions
#define MM_PER_INCH                   (25.40f);
#define INCH_PER_MM                   (0.0393701f);

// Computes hypotenuse, avoiding avr-gcc's bloated version and the extra error checking.
float hypot_f(float x, float y);

float convert_delta_vector_to_unit_vector(float *vector, uint8_t size);
float limit_value_by_axis_maximum(float *max_value, float *unit_vec, uint8_t size);

// Read a floating point value from a string. Line points to the input buffer, char_counter
// is the indexer pointing to the current character of the line, while float_ptr is
// a pointer to the result variable. Returns true when it succeeds
bool math_read_float(const char *line, size_t *char_counter, float *float_ptr);

#endif
