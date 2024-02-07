#ifndef SPRITES_HPP
#define SPRITES_HPP

#include "types.hpp"
#include "console_sprites.hpp"

class Sprite
{
public:
    inline Sprite(u16 size_x, u16 size_y, const u16* data) : is_heap_allocated(false), size_x(size_x), size_y(size_y), pos_x(0), pos_y(0), exact_pos_x(0), exact_pos_y(0){this->data = (u16*)data;};
    inline Sprite(std::vector<std::vector<u16>> data)  : size_x(data[0].size()), size_y(data.size()){

        //TODO: consider using std::vector<u16> instead of raw u16*, and then use std::vector::data() to get the raw ptr, it's safer and more convenient

        this->data = new u16[size_x * size_y];

        for(u16 y = 0; y < size_y; ++y){
            for(u16 x = 0; x < size_x; ++x){
                this->data[y * size_x + x] = data[y][x];
            }
        }
        is_heap_allocated = true;
    };
    inline ~Sprite() {if(is_heap_allocated) delete[] data;};

    [[nodiscard]] inline u16 get_size_x() const { return size_x; };
    [[nodiscard]] inline u16 get_size_y() const { return size_y; };
    [[nodiscard]] inline i16 get_pos_x() const { return pos_x; };
    [[nodiscard]] inline i16 get_pos_y() const { return pos_y; };
    inline const u16* get_data() { return data; };

    inline void set_pos_x(i16 pos_x) { this->pos_x = pos_x; };
    inline void set_pos_y(i16 pos_y) { this->pos_y = pos_y; };

    inline void set_exact_pos_x(float exact_pos_x) { this->exact_pos_x = exact_pos_x; };
    inline void set_exact_pos_y(float exact_pos_y) { this->exact_pos_y = exact_pos_y; };

    [[nodiscard]] inline float get_exact_pos_x() const { return exact_pos_x; };
    [[nodiscard]] inline float get_exact_pos_y() const { return exact_pos_y; };


private:
    u16 size_x;
    u16 size_y;
    u16* data;
    bool is_heap_allocated;

    i16 pos_x;
    i16 pos_y;
    float exact_pos_x;
    float exact_pos_y;
};

#endif /* SPRITES_HPP */
