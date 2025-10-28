//
// Created by pavle on 10/28/25.
//

#ifndef MATF_RG_PROJECT_GUICONTROLLER_HPP
#define MATF_RG_PROJECT_GUICONTROLLER_HPP
#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

class GUIController final : public engine::core::Controller {
    void initialize() { set_enable(false); }

    void poll_events() {
        const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        if (platform->key(engine::platform::KeyId::KEY_F2)
                    .state() == engine::platform::Key::State::JustPressed) { set_enable(!is_enabled()); }
    }

    void draw() {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        graphics->begin_gui();

        // Draw camera info
        ImGui::Begin("Camera info");
        const auto &c = *camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
        ImGui::End();
        graphics->end_gui();
    }
};

#endif//MATF_RG_PROJECT_GUICONTROLLER_HPP