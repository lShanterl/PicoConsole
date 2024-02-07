
#include "Console.hpp"

void Console::fill_canvas(u16 color){
    for(u16 i = 0; i < canvas_size_x * canvas_size_y; ++i){
        canvas[i] = COLOR_INTO_BYTE_FORMAT(color);
    }
}

void Console::set_window(u16 x, u16 y, u16 width, u16 height){

    // Set X coordinate
    write_command(CMD_COLUMN_ADDR_SET);
    write_data(0x00);
    write_data(x);
    write_data(0x00);
    write_data(x + width - 1);

    // Set Y coordinate
    write_command(CMD_ROW_ADDR_SET);
    write_data(0x00);
    write_data(y);
    write_data(0x00);
    write_data(y + height - 1);
}

void Console::draw_canvas(){
    write_command(CMD_MEMORY_WRITE);

    gpio_put(cs_pin, 0);
    gpio_put(dc_pin, 1);

    spi_write_blocking(spi0, (byte*)canvas, canvas_size_x * canvas_size_y * sizeof(u16));

    gpio_put(cs_pin, 1);
}

void Console::clear_canvas(){
    fill_canvas(RGB565(0, 0, 0));
    draw_canvas();
}

void Console::draw_pixel(u16 x, u16 y, u16 color){
    canvas[y * canvas_size_x + x] = COLOR_INTO_BYTE_FORMAT(color);
}

void Console::draw_sprite(Sprite& sprite, i16 x, i16 y){
    const u16 sprite_width = sprite.get_size_x();
    const u16 sprite_height = sprite.get_size_y();
    const u16* const sprite_data = (u16*)sprite.get_data();

    for(u16 sprite_y = 0; sprite_y < sprite_height; ++sprite_y){
        for(u16 sprite_x = 0; sprite_x < sprite_width; ++sprite_x){
            if((sprite_data[sprite_y * sprite_width + sprite_x] & ALPHA_FLAG) == 0){
                if(x + sprite_x < 0 || x + sprite_x >= canvas_size_x || y + sprite_y < 0 || y + sprite_y >= canvas_size_y)
                    continue;
                canvas[(y + sprite_y) * canvas_size_x + (x + sprite_x)] = sprite_data[sprite_y * sprite_width + sprite_x];
            }
        }
    }
}

void Console::draw_sprite_centered(Sprite& sprite, i16 x, i16 y){
    const u16 sprite_width = sprite.get_size_x();
    const u16 sprite_height = sprite.get_size_y();

    x -= sprite_width / 2;
    y -= sprite_height / 2;

    draw_sprite(sprite, x, y);
}

void Console::draw_rectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 color){

    const i16 y_border = std::min((i16)canvas_size_y, y1);
    const i16 x_border = std::min((i16)canvas_size_x, x1);

    for(i16 y = y0; y < y_border; ++y){
        for(i16 x = x0; x < x_border; ++x){
            if(x >= 0 && y >= 0 && x < canvas_size_x && y < canvas_size_y)
                draw_pixel(x, y, color);
        }
    }

}

void Console::draw_line(u16 x0, u16 y0, u16 x1, u16 y1, u16 color){

    // Bresenham's line algorithm

    u16 delta_x = std::abs(x1 - x0);
    u16 delta_y = std::abs(y1 - y0);

    i16 step_x = x0 <= x1 ? 1 : -1;
    i16 step_y = y0 <= y1 ? 1 : -1;

    //void (*draw_pixel)(Console*, u16, u16, u16) = nullptr;

    std::function<void(u16, u16, u16)> local_draw_pixel = [this](u16 x, u16 y, u16 color){
        this->draw_pixel(x, y, color);
    };

    if(delta_x < delta_y){
        std::swap(delta_x, delta_y);
        std::swap(x0, y0);
        std::swap(x1, y1);

        local_draw_pixel = [this](u16 y, u16 x, u16 color){
            this->draw_pixel(x, y, color);
        };
    }

    //float slope = (float)delta_y / (float)delta_x; // instead of using float, we just scale everything up by 2dx to get rid of the division (dy/dx)
    //i16 slope = 2 * delta_y;
    i16 error = /*(dy/dx)*/ 2 * delta_y - delta_x; /* and then we subtract by dx so in if statement we check if error > 0 not if it's larger than dx*/

    while(true){

        if(x0 == (x1) && y0 == (y1)){
            break;
        }

        local_draw_pixel(x0, y0, color);

        if(error > 0 /* 0.5 * 2dx = dx */){
            error += 2 * delta_y - 2 * delta_x; /* -1 multiplied by 2dx*/
            y0 += step_y;
        }
        else{
            error += 2 * delta_y;
        }
        x0 += step_x;
    }
}

void Console::bucket_fill(u16 x, u16 y, u16 color){
    u16 target_color = canvas[y * canvas_size_x + x];
    u16 new_color = COLOR_INTO_BYTE_FORMAT(color);

    if(target_color == new_color)
        return;

    std::queue<std::pair<u16, u16>> q;
    q.emplace(x, y);

    std::pair <u16, u16> current_pixel;

    while(!q.empty()){
        current_pixel = q.front();
        q.pop();

        if(canvas[current_pixel.second * canvas_size_x + current_pixel.first] == target_color){

            canvas[current_pixel.second * canvas_size_x + current_pixel.first] = new_color;

            if(current_pixel.first > 0)
                q.emplace(current_pixel.first - 1, current_pixel.second);

            if(current_pixel.first < canvas_size_x - 1)
                q.emplace(current_pixel.first + 1, current_pixel.second);

            if(current_pixel.second > 0)
                q.emplace(current_pixel.first, current_pixel.second - 1);

            if(current_pixel.second < canvas_size_y - 1)
                q.emplace(current_pixel.first, current_pixel.second + 1);
        }
    }
}

void Console::draw_circle(u16 x, u16 y, u16 radius, u16 color){
    //TODO: implement
}

void Console::draw_text(const std::string& text, u16 x, u16 y, u16 color,const Font& font) {

    //TODO: implement a system that allows to use fonts of size other than 1 byte per character
    u16 text_length = text.length();

    const u8* const font_data = font.data;

    for(u16 i = 0; i < text_length; ++i){
        u16 character = text[i] - 32;

        for(u16 j = 0; j < 8; ++j){
            u8 line = font_data[character * 8 + j];

            for(u16 k = 0; k < 8; ++k){
                if(line & (1 << k)){
                    draw_pixel(x + k, y + j, color);
                }
            }
        }

        x += 8;
    }
}

