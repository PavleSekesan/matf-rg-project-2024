//
// Created by pavle on 10/28/25.
//

#ifndef MATF_RG_PROJECT_MAINCONTROLLER_HPP
#define MATF_RG_PROJECT_MAINCONTROLLER_HPP

#include "GUIController.hpp"
#include "Light.hpp"
#include "TimerCallback.hpp"

#include <engine/postprocessing/Bloom.hpp>
#include <engine/postprocessing/Postprocessing.hpp>

class MainController final : public engine::core::Controller {

    glm::vec3 campfire_pos = glm::vec3(1.55, -1.15, -0.77);

    Light light{
            campfire_pos + glm::vec3(0.0, 0.1, 0.0),
            10.0f * glm::vec3(0.62396, 0.02416, 0.00402),
            false};

    Light dirLight = {
            glm::vec3(0.0, 1.0f, 0.0f),
            glm::vec3(0.1f),
            true};

    Bloom bloom;
    engine::postprocessing::Postprocessing postprocessing;
    TimerCallback timer;

    void initialize() override {
        engine::graphics::OpenGL::enable_depth_testing();

        auto res = engine::core::Controller::get<engine::resources::ResourcesController>();

        // bloom
        auto blur = res->shader("blur");
        blur->use();
        blur->set_int("image", 0);
        auto bloomFinal = res->shader("bloom_final");
        bloomFinal->use();
        bloomFinal->set_int("scene", 0);
        bloomFinal->set_int("bloomBlur", 1);
        bloom.shaderBlur = blur;
        bloom.shaderBloomFinal = bloomFinal;
        bloom.generateFramebuffers();

        // postprocessing
        auto postprocessingShader = res->shader("snowstorm");
        postprocessingShader->use();
        postprocessingShader->set_int("texture_diffuse1", 0);
        postprocessing.shader = postprocessingShader;
        postprocessing.generate_framebuffer();
    }

    void update() override {
        update_camera();
        update_light();
    }

    void begin_draw() override {
        bloom.attachHdrFbo();
        engine::graphics::OpenGL::clear_buffers();
        engine::graphics::OpenGL::enable_depth_testing();
    }

    void draw() override {
        // draw_skybox();
        // draw_light();

        draw_terrain();
        draw_campfire();
        draw_explorer();
        draw_polarbear();
    }

    void draw_model(const std::string &modelName, const std::string &shaderName, glm::mat4 modelMat) {
        auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
        auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader(shaderName);
        auto model = engine::core::Controller::get<engine::resources::ResourcesController>()->model(modelName);
        shader->use();
        shader->set_mat4("projection", graphics->projection_matrix());
        shader->set_mat4("view", graphics->camera()
                                         ->view_matrix());
        shader->set_mat4("model", modelMat);

        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        shader->set_vec3("viewPos", camera->Position);
        shader->set_vec3("light[0].position", light.get_position());
        shader->set_vec3("light[0].color", light.get_color());
        shader->set_bool("light[0].directional", light.get_directional());
        shader->set_vec3("light[1].position", dirLight.get_position());
        shader->set_vec3("light[1].color", dirLight.get_color());
        shader->set_bool("light[1].directional", dirLight.get_directional());
        model->draw(shader);
    }

    void draw_polarbear() {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, campfire_pos + glm::vec3(-0.5f, -0.02f, 2.1f));
        model = glm::rotate(model, glm::radians(170.0f), glm::vec3(0.0, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2));
        draw_model("polarbear", "basic", model);
    }

    void draw_explorer() {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, campfire_pos + glm::vec3(0.0f, 0.02f, -0.5f));
        model = glm::scale(model, glm::vec3(0.1));
        draw_model("explorer", "basic", model);
    }

    void draw_campfire() {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, campfire_pos);
        model = glm::scale(model, glm::vec3(0.02f));
        draw_model("campfire", "basic", model);
    }

    void draw_light() {
        auto model = glm::translate(glm::mat4(1.0f), light.get_position());
        model = glm::scale(model, glm::vec3(0.01));
        auto shader = get<engine::resources::ResourcesController>()->shader("light");
        shader->use();
        shader->set_vec3("lightColor", light.get_color());
        draw_model("light", "light", model);
    }

    void draw_terrain() {
        glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(20.0));
        draw_model("terrain", "basic", model);
    }

    void draw_skybox() {
        auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
        auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
        engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
    }

    void end_draw() override {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        bloom.blurBrightFragments(postprocessing.fbo);
        postprocessing.shader->use();
        postprocessing.shader->set_float("time", platform->frame_time().current);
        postprocessing.draw_to_screen();
        platform->swap_buffers();
    }

    void update_camera() {
        auto gui = engine::core::Controller::get<GUIController>();
        if (gui->is_enabled()) { return; }
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        float dt = platform->dt();
        if (platform->key(engine::platform::KEY_W)
                    .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt); }
        if (platform->key(engine::platform::KEY_S)
                    .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt); }
        if (platform->key(engine::platform::KEY_A)
                    .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt); }
        if (platform->key(engine::platform::KEY_D)
                    .state() == engine::platform::Key::State::Pressed) { camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt); }
        auto mouse = platform->mouse();
        camera->rotate_camera(mouse.dx, mouse.dy);
        camera->zoom(mouse.scroll);
    }

    void update_light() {
        auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
        float dt = platform->dt();
        if (platform->key(engine::platform::KEY_UP)
                    .state() == engine::platform::Key::State::Pressed) { light.move(glm::vec3(0.0, 0.0, -1.0), dt); }
        if (platform->key(engine::platform::KEY_DOWN)
                    .state() == engine::platform::Key::State::Pressed) { light.move(glm::vec3(0.0, 0.0, 1.0), dt); }
        if (platform->key(engine::platform::KEY_LEFT)
                    .state() == engine::platform::Key::State::Pressed) { light.move(glm::vec3(-1.0, 0.0, 0.0), dt); }
        if (platform->key(engine::platform::KEY_RIGHT)
                    .state() == engine::platform::Key::State::Pressed) { light.move(glm::vec3(1.0, 0.0, 0.0), dt); }
        if (platform->key(engine::platform::KEY_ENTER)
                    .state() == engine::platform::Key::State::Pressed) { light.change_intensity(1.0 * dt); }
        if (platform->key(engine::platform::KEY_RIGHT_SHIFT)
                    .state() == engine::platform::Key::State::Pressed) { light.change_intensity(-1.0 * dt); }


        if (platform->key(engine::platform::KEY_1)
                    .state() == engine::platform::Key::State::JustPressed) {
            auto cb2 = [this] {
                // dirLight.toggle();
                light.toggle();
            };
            auto cb1 = [this, cb2] {
                cb2();
                timer.setTimeout(1.0, cb2);
            };
            timer.setTimeout(1.0, cb1);
        }
        timer.tick(dt);
    }
};

#endif//MATF_RG_PROJECT_MAINCONTROLLER_HPP