//
// Created by pavle on 10/28/25.
//

#ifndef MATF_RG_PROJECT_LIGHT_HPP
#define MATF_RG_PROJECT_LIGHT_HPP
#include <glm/glm.hpp>

class Light {
    static constexpr float SPEED = 2.5f;

    glm::vec3 position;
    glm::vec3 color;
    float intensity = 1.0f;
    float toggled = 1.0f;
    bool directional;

public:
    Light(glm::vec3 position, glm::vec3 color, bool directional = false)
        : position(position)
        , color(color)
        , directional(directional) {}

    glm::vec3 get_position() const { return position; }
    glm::vec3 get_color() const { return color * intensity * toggled; }
    bool get_directional() const { return directional; }

    void move(glm::vec3 direction, float dt) { position += direction * SPEED * dt; }

    void change_intensity(float increment) { intensity = std::max(intensity + increment, 0.0f); }

    void toggle() { toggled = 1.0f - toggled; }
};

#endif//MATF_RG_PROJECT_LIGHT_HPP