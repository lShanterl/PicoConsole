#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <iostream>
#include <vector>
#include <functional>
#include <queue>
#include <cmath>
#include <algorithm>
#include <random>

#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "cinttypes"
#include "sprites.hpp"
#include "console_audio.hpp"
#include "pico/multicore.h"


#define SPI_FREQ 65536000

#define CMD_DISPLAY_ON 0x29
#define CMD_DISPLAY_OFF 0x28

#define CMD_SLEEP_IN 0x11
#define CMD_SLEEP_OUT 0x11

#define CMD_INVERSION_ON 0x21
#define CMD_INVERSION_OFF 0x20

#define CMD_GAMMA_SET 0x26
#define CMD_GAMMA_CURVE_2_2 0x01 
#define LCD_GAMMA_CURVE_1_8 0x02


#define CMD_COLUMN_ADDR_SET 0x2a
#define CMD_ROW_ADDR_SET 0x2b

#define CMD_MEMORY_WRITE 0x2c

#define CMD_TEARING_ON 0x35
#define CMD_TEARING_OFF 0x34

#define CMD_COLOR_MODE 0x3a

#define CMD_POWER_CONTROL_1 0xc0
#define CMD_POWER_CONTROL_2 0xc1

#define CMD_VCOM_CONTROL_1 0xc5
#define VCOM_DATA 0x38, 0x40

#define CMD_FRAMERATE_CONTROL 0xb1
#define FRAMERATE_DATA 0x0E, 0x10

#define LCD_COLOR_MODE_4R4G4B 0x03
#define LCD_COLOR_MODE_5R6G5B 0x05
#define LCD_COLOR_MODE_6R6G6B 0x06
#define LCD_COLOR_MODE_DEFAULT LCD_COLOR_MODE_5R6G5B

#define CMD_MEM_ACCESS_CTL 0x36
#define CTL_ORDER_TOP_TO_BOTTOM (0 << 7) // vertical layout
#define CTL_ORDER_BOTTOM_TO_TOP (1 << 7) // vertical layout

#define CTL_ORDER_LEFT_TO_RIGHT (0 << 6) // vertical layout
#define CTL_ORDER_RIGHT_TO_LEFT (1 << 6) // vertical layout

#define CTL_ROW_COLUMN_EXCHANGE (1 << 5) // horizontal layout
#define CTL_ROW_COLUMN_NORMAL (0 << 5) // vertical layout

#define CTL_VERTICAL_REFRESH_TOP_TO_BOTTOM (0 << 4)
#define CTL_VERTICAL_REFRESH_BOTTOM_TO_TOP (1 << 4)

#define CTL_RGB (0 << 3)
#define CTL_BGR (1 << 3)

#define CTL_HORIZONTAL_REFRESH_LEFT_TO_RIGHT (0 << 2) 
#define CTL_HORIZONTAL_REFRESH_RIGHT_TO_LEFT (1 << 0)
#define CTL_HORIZONTAL_REFRESH_TOP_TO_BOTTOM (1 << 1)
#define CTL_HORIZONTAL_REFRESH_BOTTOM_TO_TOP (3 << 0)

#define CMD_MEM_ACCESS_CTL_DEFAULT (CTL_ORDER_BOTTOM_TO_TOP | CTL_ORDER_LEFT_TO_RIGHT | CTL_ROW_COLUMN_EXCHANGE | CTL_VERTICAL_REFRESH_BOTTOM_TO_TOP | CTL_RGB | CTL_HORIZONTAL_REFRESH_TOP_TO_BOTTOM)

// @details
// CMD_MEM_ACCESS_CTL
// Bit 7 (MY): Row Address Order (0: Top to Bottom, 1: Bottom to Top)
// Bit 6 (MX): Column Address Order (0: Left to Right, 1: Right to Left)
// Bit 5 (MV): Row/Column Exchange (0: Normal, 1: Exchange)
// Bit 4 (ML): Vertical Refresh Order (0: Refresh Top to Bottom, 1: Bottom to Top)
// Bit 3 (RGB): RGB/BGR Order (0: RGB, 1: BGR)
// Bit 2-0 (MH[2:0]): Horizontal Refresh Order (000: Refresh Left to Right, 001: Right to Left, 010: Refresh Top to Bottom, 011: Bottom to Top, 100: Undefined, 101: Undefined, 110: Undefined, 111: Undefined)

#ifndef CYW43_WL_GPIO_LED_PIN
    #define CYW43_WL_GPIO_LED_PIN 0
#endif

#define RGB565(r, g, b) (((r>>3) << 11) | ((g>>2) << 5) | (b >> 3))
#define COLOR_INTO_BYTE_FORMAT(color) ((color >> 8) | (color << 8)) // basically all that does is split 16-bit color into 2 8-bit colors

#define ALPHA_FLAG 0b0010000000000000

namespace Raspberry
{
    extern void internal_led_on() noexcept;
    extern void internal_led_off() noexcept;
}

struct ConsoleInput{
    u8 joystick_pin;
    u8 joystick_vry_pin;
    u8 joystick_vrx_pin;
    u8 buzzer_pin;
};
class Console;
struct GameInfo{
    std::function<void()> on_exit;
    std::function<void(Console* const)> on_start;
    std::function<void(Console* const)> on_update;
};

class Console
{
public:

    // initialization & properties
    Console(u16 size_x, u16 size_y);
    void reset();
    void init_input(u8 joystick_pin,u8 joystick_vry_pin, u8 joystick_vrx_pin, u8 buzzer_pin);
    void init_lcd(u8 cs_pin, u8 reset_pin, u8 dc_pin, u8 din_pin, u8 clk_pin);

    // core
    void write_command(byte command);
    void write_data(byte data);
    static void spi_write(byte data);
    void detect_collision(Sprite* sprite_a, Sprite* sprite_b, const std::function<void()>& callback);
    bool detect_collision(Sprite* sprite_a, Sprite* sprite_b);
    bool detect_collision(Sprite* sprite, i16 x0, i16 y0, i16 x1, i16 y1);
    bool move_sprite(Sprite* sprite, i16 target_x, i16 target_y, float speed);
    inline const ConsoleInput& get_input_pins() { return input; }

    // drawing
    void set_window(u16 x, u16 y, u16 width, u16 height);
    void fill_canvas(u16 color);
    void draw_canvas();
    void clear_canvas();
    void draw_pixel(u16 x, u16 y, u16 color);

    void draw_sprite(Sprite& sprite, i16 x, i16 y);
    void draw_sprite_centered(Sprite& sprite, i16 x, i16 y);

    void draw_rectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 color);
    void draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u16 color);
    void bucket_fill(u16 x, u16 y, u16 color);
    void draw_circle(u16 x, u16 y, u16 radius, u16 color);

    void draw_text(const std::string& text, u16 x, u16 y, u16 color,const Font& font);

    // audio
    void play_note(uint note, u16 duration);
    void play_tone(uint frequency, uint duration);
    void play_music(const i16* const music, u16 arr_elements);

    // misc
    void get_frame_rate();
    inline u16* get_canvas() { return canvas; }
    [[nodiscard]] inline u16 get_canvas_size_x() const { return canvas_size_x; }
    [[nodiscard]] inline u16 get_canvas_size_y() const { return canvas_size_y; }
    [[nodiscard]] inline double get_delta_time() { return delta_time; }

private:

    // init
    void init_lcd_display();
    void init_lcd_pins(u8 cs_pin, u8 reset_pin, u8 dc_pin, u8 din_pin, u8 clk_pin);
    
    // properties
    u16 canvas_size_x;
    u16 canvas_size_y;
    u16* canvas;
    ConsoleInput input;


    // initialization
    u8 cs_pin{};
    u8 reset_pin{};
    u8 dc_pin{};
    u8 din_pin{};
    u8 clk_pin{};

    // misc
    double delta_time = 0.0;
    double last_time = 0.0;
    double frame_rate = 0.0;
};

#endif /* CONSOLE_HPP */
