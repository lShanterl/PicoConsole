#include "Console.hpp"
#include "game_list.hpp"

constexpr u8 cs_pin = 20; // chip select
constexpr u8 reset_pin = 19; 
constexpr u8 dc_pin = 18; // command mode (A0 low) and data mode (A0 high)
constexpr u8 din_pin = 3; //sda
constexpr u8 clk_pin = 2; //clk

constexpr u8 joystick_pin = 15;
constexpr u8 joystick_vry_pin = 26;
constexpr u8 joystick_vrx_pin = 27;

constexpr u8 buzzer_pin = 16;

constexpr u8 ST7735_WIDTH = 160;
constexpr u8 ST7735_HEIGHT = 128;

Console console(ST7735_WIDTH, ST7735_HEIGHT);

void main_second_thread() {
    while(true) {
        console.play_music(dooms_gate, sizeof(dooms_gate) / sizeof(i16));
    }
}

int main(){

    stdio_init_all();
    Raspberry::internal_led_on();

    console.init_lcd(cs_pin, reset_pin, dc_pin, din_pin, clk_pin);
    console.init_input(joystick_pin, joystick_vry_pin, joystick_vrx_pin, buzzer_pin);
    console.set_window(0, 0, console.get_canvas_size_x(), console.get_canvas_size_y());
    console.clear_canvas();

    GameInfo& info = flappy_bird_game_info;
    info.on_start(&console);

    console.get_frame_rate();

    multicore_launch_core1(main_second_thread);

    while(true) {

        console.get_frame_rate();
        info.on_update(&console);
        console.draw_canvas();
    }
}