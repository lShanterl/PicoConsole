
#include "Console.hpp"

void Raspberry::internal_led_on() noexcept
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
}

void Raspberry::internal_led_off() noexcept
{
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
}

void Console::write_command(byte command)
{
    gpio_put(dc_pin, 0);
    gpio_put(cs_pin, 0);
    spi_write(command);
    gpio_put(cs_pin, 1);
}

void Console::write_data(byte data)
{
    gpio_put(dc_pin, 1);
    gpio_put(cs_pin, 0);
    spi_write(data);
    gpio_put(cs_pin, 1);
}

void Console::spi_write(byte data)
{
    spi_write_blocking(spi0, &data, 1);
}

bool Console::detect_collision(Sprite* sprite_a, Sprite* sprite_b)
{
    //check bounding boxes first and check whether the pixel is transparent or not

    u16 sprite_b_x = sprite_b->get_pos_x();
    u16 sprite_b_y = sprite_b->get_pos_y();
    u16 sprite_b_size_x = sprite_b->get_size_x();
    u16 sprite_b_size_y = sprite_b->get_size_y();


    if(detect_collision(sprite_a, sprite_b_x, sprite_b_y, sprite_b_x + sprite_b_size_x, sprite_b_y + sprite_b_size_y))
    {
        return true;
    }

    return false;
}

void Console::detect_collision(Sprite *sprite_a, Sprite *sprite_b, const std::function<void()>& callback) {
    if(detect_collision(sprite_a, sprite_b)){
        callback();
    }
}

bool Console::detect_collision(Sprite *sprite, i16 x0, i16 y0, i16 x1, i16 y1) {

    u16 sprite_a_x = sprite->get_pos_x();
    u16 sprite_a_y = sprite->get_pos_y();
    u16 sprite_a_size_x = sprite->get_size_x();
    u16 sprite_a_size_y = sprite->get_size_y();

    if(sprite_a_x < x1 && sprite_a_x + sprite_a_size_x > x0 && sprite_a_y < y1 && sprite_a_y + sprite_a_size_y > y0){
        return true;
    }
    return false;
}

bool Console::move_sprite(Sprite* sprite, i16 target_x, i16 target_y, float speed) {
    const i16 sprite_x = sprite->get_pos_x();
    const i16 sprite_y = sprite->get_pos_y();

    if (sprite_x == target_x && sprite_y == target_y) {
        return true;
    }

    i16 distance_x = target_x - sprite_x;
    i16 distance_y = target_y - sprite_y;

    i8 direction_x = (distance_x > 0) - (distance_x < 0);
    i8 direction_y = (distance_y > 0) - (distance_y < 0);

    if(direction_x){
        sprite->set_exact_pos_x(sprite->get_exact_pos_x() + direction_x * speed * delta_time);
        sprite->set_pos_x(sprite->get_exact_pos_x());
    }
    if(direction_y){
        sprite->set_exact_pos_y(sprite->get_exact_pos_y() + direction_y * speed * delta_time);
        sprite->set_pos_y(sprite->get_exact_pos_y());
    }

    //printf("sdt::clamp: %d\n", std::clamp(sprite->get_pos_x(), std::min(target_x, sprite_x), std::max(target_x, sprite_x)));
    sprite->set_pos_x(std::clamp(sprite->get_pos_x(), std::min(target_x, sprite_x), std::max(target_x, sprite_x)));
    //sprite->set_exact_pos_x(sprite->get_pos_x());

    //printf("sdt::clamp: %d\n", std::clamp(sprite->get_pos_y(), std::min(target_y, sprite_y), std::max(target_y, sprite_y)));
    sprite->set_pos_y(std::clamp(sprite->get_pos_y(), std::min(target_y, sprite_y), std::max(target_y, sprite_y)));
    //sprite->set_exact_pos_y(sprite->get_pos_y());

    return sprite->get_pos_x() == target_x && sprite->get_pos_y() == target_y;
}

