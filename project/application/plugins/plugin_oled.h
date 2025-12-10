#ifndef __PLUGIN_OLED_H
#define __PLUGIN_OLED_H

void plugin_oled_clear(void);
void plugin_oled_show_char(uint8_t Line, uint8_t Column, char Char);
void plugin_oled_show_string(uint8_t Line, uint8_t Column, char *String);
void plugin_oled_show_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void plugin_oled_show_signed_num(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void plugin_oled_show_hex_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void plugin_oled_show_bin_num(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif
