
#include "Console.hpp"

void Console::get_frame_rate()
{
    delta_time = (double)time_us_64() / 1000000.0 - last_time; // in seconds
    last_time = (double)time_us_64() / 1000000.0;
    frame_rate = 1.0 / delta_time;
    printf("Frame rate: %f\n", frame_rate);
    //printf("Delta time: %f\n", delta_time);
}