
#include "console.hpp"

constexpr u8 pipe_height = 15;
constexpr u8 pipe_width = 17;
constexpr u8 jump_speed = 13;
constexpr u8 jump_height = 23;
constexpr float falling_speed = 13.3f;
constexpr float pipe_speed = 6.0f;
constexpr u8 pipe_distance = 70;
constexpr u8 initial_distance = 160 + pipe_width;

constexpr u8 hole_size = 70;

constexpr float gravity = 9.81f;

i16 score;

Sprite flappy_bird_sprite(17,13, flappy_bird);
Sprite background_sprite(160, 128, background);

Sprite pipes[3]{Sprite(pipe_width, pipe_height, pipe), Sprite(pipe_width, pipe_height, pipe), Sprite(pipe_width, pipe_height, pipe)};
const i16 distances[]{initial_distance, initial_distance + pipe_distance, initial_distance + pipe_distance * 2};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<i8> dist(64, 128 - pipe_height);

i16 flappy_bird_pos_x;
i16 flappy_bird_pos_y;

u16 joystick_pin_state;
bool is_jumping;

std::string score_str;
bool is_scored;
u8 pipe_index;

void flappy_bird_game_init(Console* const console)
{
    flappy_bird_sprite.set_pos_x(console->get_canvas_size_x() / 4 - flappy_bird_sprite.get_size_x() / 2);
    flappy_bird_sprite.set_pos_y(console->get_canvas_size_y() / 2 - flappy_bird_sprite.get_size_y() / 2);

    flappy_bird_sprite.set_exact_pos_x(console->get_canvas_size_x() / 4 - flappy_bird_sprite.get_size_x() / 2);
    flappy_bird_sprite.set_exact_pos_y(console->get_canvas_size_y() / 2 - flappy_bird_sprite.get_size_y() / 2);

    flappy_bird_pos_x = flappy_bird_sprite.get_pos_x();
    flappy_bird_pos_y = flappy_bird_sprite.get_pos_y();

    joystick_pin_state = 0;
    is_jumping = false;
    score = 0;
    score_str = "0";
    is_scored = false;
    pipe_index = 0;


    for(u8 i = 0; i < 3; ++i){
        pipes[i].set_pos_x(distances[i]);
        pipes[i].set_exact_pos_x(distances[i]);
        pipes[i].set_pos_y(dist(gen));
        pipes[i].set_exact_pos_y(pipes[i].get_pos_y());
    }
}


void flappy_bird_draw_menu(Console* const console) {

    console->draw_sprite_centered(background_sprite, console->get_canvas_size_x() / 2, console->get_canvas_size_y() / 2);
    console->draw_text("Game Over", 40, 40, 0x0, font_5x8);
    console->draw_text("Score: " + std::to_string(score), 40, 60, 0x0, font_5x8);
    console->draw_text("joystick to restart", 10, 100, 0x0, font_5x8);
    //console->draw_button(10, 110, 150, 120, RGB565(10,245, 37), "Restart", font_5x8);

    while(true){
        console->get_frame_rate();
        if (joystick_pin_state == 1 && gpio_get(console->get_input_pins().joystick_pin) == 0){
            joystick_pin_state = 0;
            flappy_bird_game_init(console);
            break;
        } else if (joystick_pin_state == 0 && gpio_get(console->get_input_pins().joystick_pin) == 1) {
            joystick_pin_state = 1;
        }
        console->draw_canvas();
    }

}

void flappy_bird_game_update(Console* const console)
{
    console->draw_sprite(background_sprite, 0, 0);

    if (!is_jumping) {
        console->move_sprite(&flappy_bird_sprite, flappy_bird_sprite.get_pos_x(), 128, falling_speed * gravity);
    }

    if (flappy_bird_sprite.get_pos_y() >= 128 - flappy_bird_sprite.get_size_y()) {
        flappy_bird_sprite.set_pos_y(128 - flappy_bird_sprite.get_size_y());
        flappy_bird_sprite.set_exact_pos_y(128 - flappy_bird_sprite.get_size_y());
    }

    for(u8 i = 0; i < 3; ++i){
        const i16 pipe_pos_x = pipes[i].get_pos_x();
        const i16 pipe_pos_y = pipes[i].get_pos_y();
        const i16 pipe_pos_x_plus_width = pipe_pos_x + pipe_width;
        const i16 pipe_pos_y_minus_hole_size = pipe_pos_y - hole_size;

        if(pipe_pos_x <= 0 - pipe_width){
            pipes[i].set_pos_x(initial_distance + pipe_width);
            pipes[i].set_exact_pos_x(initial_distance + pipe_width);
            pipes[i].set_pos_y(dist(gen));
            pipes[i].set_exact_pos_y(pipes[i].get_pos_y());
        }

        console->draw_sprite(pipes[i], pipe_pos_x, pipe_pos_y);
        console->draw_sprite(pipes[i], pipe_pos_x, pipe_pos_y_minus_hole_size);

        console->draw_rectangle(pipe_pos_x, pipe_pos_y + pipe_height, pipe_pos_x_plus_width, console->get_canvas_size_x(), 0x0);
        console->draw_rectangle(pipe_pos_x, 0, pipe_pos_x_plus_width, pipe_pos_y_minus_hole_size, 0x0);

        console->move_sprite(&pipes[i], 0 - pipe_width, pipe_pos_y, pipe_speed * gravity);

        bool collision = console->detect_collision(&flappy_bird_sprite, pipe_pos_x, 0, pipe_pos_x_plus_width, pipe_pos_y_minus_hole_size + pipe_height) || console->detect_collision(&flappy_bird_sprite, pipe_pos_x, pipe_pos_y, pipe_pos_x_plus_width, console->get_canvas_size_x());
        if(collision){
            sleep_ms(250);
            flappy_bird_draw_menu(console);
        }
        else
        {
            bool is_point = console->detect_collision(&flappy_bird_sprite, pipe_pos_x + pipe_width / 2, pipe_pos_y_minus_hole_size, pipe_pos_x + pipe_width / 2, pipe_pos_y);

            if(is_point && !is_scored){
                is_scored = true;
                pipe_index = i;
                score++;
                score_str = std::to_string(score);
            }
            else if(!is_point && is_scored && pipe_index == i){
                is_scored = false;
            }
        }
    }

    if (joystick_pin_state == 1 && gpio_get(console->get_input_pins().joystick_pin) == 0){
        joystick_pin_state = 0;
        is_jumping = true;
        flappy_bird_pos_x = flappy_bird_sprite.get_pos_x();
        flappy_bird_pos_y = flappy_bird_sprite.get_pos_y() >= jump_height ? flappy_bird_sprite.get_pos_y() - jump_height : 0;
    } else if (joystick_pin_state == 0 && gpio_get(console->get_input_pins().joystick_pin) == 1) {
        joystick_pin_state = 1;
    }

    if (is_jumping) {
        if(console->move_sprite(&flappy_bird_sprite, flappy_bird_pos_x, flappy_bird_pos_y, jump_speed * gravity)){
            is_jumping = false;
        }
    }

    console->draw_sprite(flappy_bird_sprite, flappy_bird_sprite.get_pos_x(), flappy_bird_sprite.get_pos_y());

    console->draw_text(score_str, 5, 5, 0x0, font_5x8);
}

GameInfo flappy_bird_game_info{
        .on_exit = [](){},
        .on_start = flappy_bird_game_init,
        .on_update = flappy_bird_game_update
};

