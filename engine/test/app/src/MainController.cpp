#include <app/GUIController.hpp>
#include <app/MainController.hpp>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <memory>
#include <spdlog/spdlog.h>

namespace engine::test::app {
void MainPlatformEventObserver::on_key(engine::platform::Key key) {
    spdlog::info("Keyboard event: key={}, state={}", key.name(), key.state_str());
}

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    spdlog::info("MousePosition: {} {}", position.x, position.y);
}

void MainController::initialize() {

    engine::graphics::OpenGL::enable_depth_testing();
    engine::graphics::OpenGL::enable_stencil_testing();
    auto observer = std::make_unique<MainPlatformEventObserver>();
    engine::core::Controller::get<engine::platform::PlatformController>()->register_platform_event_observer(
            std::move(observer));
}

bool MainController::loop() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE)
                .state() == engine::platform::Key::State::JustPressed) {
        return false;
    }
    return true;
}

void MainController::poll_events() {
    const auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto gui = engine::core::Controller::get<GUIController>();
    if (platform->key(engine::platform::KEY_F1)
                .state() == engine::platform::Key::State::JustPressed) {
        m_cursor_enabled = !m_cursor_enabled;
        platform->set_enable_cursor(m_cursor_enabled);
    } else if (platform->key(engine::platform::MOUSE_BUTTON_LEFT).state() == engine::platform::Key::State::JustPressed && !gui->is_enabled()) {
        auto shape_pos = graphics->camera()->Position + 7.5f * get_camera_ray();
        m_shapes.push_back({m_selected_shape, shape_pos});
    }
}

void MainController::update() {
    update_camera();
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();
}

void MainController::draw() {
    // draw_backpack();
    draw_skybox();
    draw_cubes();
}

void MainController::end_draw() {
    engine::core::Controller::get<engine::platform::PlatformController>()->swap_buffers();
}

void MainController::draw_backpack() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("basic");
    auto backpack = engine::core::Controller::get<engine::resources::ResourcesController>()->model("backpack");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    shader->set_mat4("model", scale(glm::mat4(1.0f), glm::vec3(m_backpack_scale)));
    backpack->draw(shader);
}

void MainController::draw_cubes() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto res = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto shader = res->shader("basic");
    auto outlineShader = res->shader("outline");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    outlineShader->use();
    outlineShader->set_mat4("projection", graphics->projection_matrix());
    outlineShader->set_mat4("view", graphics->camera()
                                     ->view_matrix());
    for (auto shapeData : m_shapes) {
        resources::Model * shape;
        if (shapeData.selected_shape == Sphere) {
            shape = res->model("sphere");
        } else {
            shape = res->model("cube");
        }
        auto model = glm::translate(glm::mat4(1.0f), shapeData.position);
        auto outlineModel = glm::scale(model, glm::vec3(1.02f));
        shader->use();
        shader->set_mat4("model", model);
        outlineShader->use();
        outlineShader->set_mat4("model", outlineModel);
        auto ray = glm::vec3(glm::inverse(model) * glm::vec4(get_camera_ray(), 0.0));
        auto orig = glm::vec3(glm::inverse(model) * glm::vec4(graphics->camera()->Position, 1.0));

        if (shape->collides_with_ray(orig, ray)) {
            // glStencilFunc(GL_ALWAYS, 1, 0xFF);
            // glStencilMask(0xFF);
            graphics::OpenGL::setup_stencil(false, 1, 0xFF);
            shape->draw(shader);
            // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            // glStencilMask(0x00);
            graphics::OpenGL::setup_stencil(true, 1, 0x00);
            shape->draw(outlineShader);
            // glStencilFunc(GL_ALWAYS, 0, 0xFF);
            // glStencilMask(0xFF);
            graphics::OpenGL::setup_stencil(false, 0, 0xFF);
        } else {
            shape->draw(shader);
        }
    }
}

void MainController::draw_skybox() {
    auto shader = engine::core::Controller::get<engine::resources::ResourcesController>()->shader("skybox");
    auto skybox_cube = engine::core::Controller::get<engine::resources::ResourcesController>()->skybox("skybox");
    engine::core::Controller::get<engine::graphics::GraphicsController>()->draw_skybox(shader, skybox_cube);
}

glm::vec3 MainController::get_camera_ray() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto mouse = platform->mouse();
    auto width = platform->window()->width();
    auto height = platform->window()->height();
    float x = (2.0f * mouse.x) / width - 1.0f;
    float y = 1.0f - (2.0f * mouse.y) / height;
    glm::vec4 ray_clip(x, y, -1.0f, 1.0f);
    glm::vec4 ray_eye = glm::inverse(graphics->projection_matrix()) * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    glm::vec3 ray_world = glm::vec3(glm::inverse(graphics->camera()->view_matrix()) * ray_eye);
    ray_world = glm::normalize(ray_world);
    return ray_world;
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KEY_S)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KEY_A)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KEY_D)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}
}



