
#include "Console.hpp"

Console::Console(u16 size_x, u16 size_y) : canvas_size_x(size_x), canvas_size_y(size_y)
{
    canvas = new u16[size_x * size_y];
}

void Console::reset()
{
    gpio_put(reset_pin, 0);
    sleep_ms(10);
    gpio_put(reset_pin, 1);
    sleep_ms(10);
}

void Console::init_lcd_pins(u8 cs_pin, u8 reset_pin, u8 dc_pin, u8 din_pin, u8 clk_pin)
{
    this->cs_pin = cs_pin;
    this->reset_pin = reset_pin;
    this->dc_pin = dc_pin;
    this->din_pin = din_pin;
    this->clk_pin = clk_pin;

    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    gpio_init(dc_pin);
    gpio_set_dir(dc_pin, GPIO_OUT);
    gpio_put(dc_pin, 0);

    gpio_init(reset_pin);
    gpio_set_dir(reset_pin, GPIO_OUT);
    gpio_put(reset_pin, 1);


    spi_init(spi0, SPI_FREQ);
    gpio_set_function(din_pin, GPIO_FUNC_SPI);
    gpio_set_function(clk_pin, GPIO_FUNC_SPI);
}

void Console::init_lcd_display()
{
    reset();

    write_command(CMD_SLEEP_OUT);
    sleep_ms(120);

    write_command(CMD_TEARING_ON);

//    write_command(CMD_POWER_CONTROL_1);
//    write_data(0x08);
//
//    write_command(CMD_POWER_CONTROL_2);
//    write_data(0x05);

    write_command(CMD_COLOR_MODE);
    write_data(LCD_COLOR_MODE_5R6G5B);

    write_command(CMD_MEM_ACCESS_CTL);
    write_data(CMD_MEM_ACCESS_CTL_DEFAULT);

    write_command(CMD_GAMMA_SET); 
    write_data(LCD_GAMMA_CURVE_1_8); 

    write_command(CMD_INVERSION_OFF);

    write_command(CMD_DISPLAY_ON);
}

void Console::init_lcd(u8 cs_pin, u8 reset_pin, u8 dc_pin, u8 din_pin, u8 clk_pin)
{
    init_lcd_pins(cs_pin, reset_pin, dc_pin, din_pin, clk_pin);
    init_lcd_display();
}

void Console::init_input(u8 joystick_pin, u8 joystick_vry_pin, u8 joystick_vrx_pin, u8 buzzer_pin){

    input ={
        .joystick_pin = joystick_pin,
        .joystick_vry_pin = joystick_vry_pin,
        .joystick_vrx_pin = joystick_vrx_pin,
        .buzzer_pin = buzzer_pin
    };

    gpio_init(joystick_pin);
    gpio_pull_up(joystick_pin);

    gpio_init(joystick_vry_pin);
    adc_init();


    gpio_init(buzzer_pin);
    gpio_set_function(buzzer_pin, GPIO_FUNC_PWM);
    pwm_config pwm_cfg = pwm_get_default_config();
    pwm_init(pwm_gpio_to_slice_num(buzzer_pin), &pwm_cfg, false);
}