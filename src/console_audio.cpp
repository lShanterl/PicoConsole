
#include "Console.hpp"

void Console::play_note(uint note, u16 duration) {
    const u8 slice = pwm_gpio_to_slice_num(input.buzzer_pin);
    const u8 channel = pwm_gpio_to_channel(input.buzzer_pin);

    pwm_set_clkdiv(slice, 2000.0f / note);
    pwm_set_wrap(slice, PWM_WRAP);
    pwm_set_chan_level(slice, channel, PWM_DUTY);
    pwm_set_enabled(slice, true);


    sleep_ms(duration);

    pwm_set_enabled(slice, false);

}

void Console::play_music(const i16* const music, u16 arr_elements)
{
    i8 divider{};
    u16 note_duration{};

    // this calculates the duration of a whole note in ms (60s/tempo)*4 beats
    i64 whole_note = (60000 * 4) / DEFAULT_TEMPO;

    pwm_set_enabled(pwm_gpio_to_slice_num(input.buzzer_pin), true);

    for(u16 i{}; i < arr_elements; i += 2)
    {
        divider = music[i + 1];
        if(divider > 0){
            note_duration = whole_note / divider;
        } else if(divider < 0)
        {
            note_duration = whole_note / (divider * -1);
            note_duration *= 1.5;
        }

        play_note(music[i], note_duration * 0.9); // leaving 10% of the duration for a pause

    }

    pwm_set_enabled(pwm_gpio_to_slice_num(input.buzzer_pin), false);
}