#pragma once
#include "display.hpp"
#include <cstdint>

struct Sprite {
    std::uint8_t x = 0, y = 0;
    std::int8_t vx = 0, vy = 0;
    bool alive = false;
};

inline void move_axis(std::uint8_t& position, std::int8_t& velocity, int edge) {
    int next = static_cast<int>(position) + velocity;
    int speed = velocity;
    while (next < 0 || next > edge) {
        if (next < 0) next = -next;
        else next = 2 * edge - next;
        speed = -speed;
    }
    position = static_cast<std::uint8_t>(next);
    velocity = static_cast<std::int8_t>(speed);
}

inline void tick(Memory& mem, Sprite (&sprites)[8]) {
    clear(mem);
    for (auto& sprite : sprites) {
        if (!sprite.alive) continue;
        move_axis(sprite.x, sprite.vx, 63);
        move_axis(sprite.y, sprite.vy, 31);
        plot(mem, sprite.x, sprite.y);
    }
    show(mem);
}

inline bool sprite_command(const std::string& cmd, std::istringstream& words,
                           Memory& mem, Sprite (&sprites)[8]) {
    if (cmd != "sprite" && cmd != "tick") return false;
    std::string extra;
    if (cmd == "tick") {
        if (words >> extra) std::cout << "usage: tick\n";
        else tick(mem, sprites);
        return true;
    }
    int i, x, y, vx, vy;
    if (!(words >> i >> x >> y >> vx >> vy) || (words >> extra) ||
        i < 0 || i >= 8 || x < 0 || x >= 64 || y < 0 || y >= 32 ||
        vx < -127 || vx > 127 || vy < -127 || vy > 127) {
        std::cout << "usage: sprite <0..7> <0..63> <0..31> <-127..127> <-127..127>\n";
        return true;
    }
    sprites[i] = {static_cast<std::uint8_t>(x), static_cast<std::uint8_t>(y),
                  static_cast<std::int8_t>(vx), static_cast<std::int8_t>(vy), true};
    return true;
}
