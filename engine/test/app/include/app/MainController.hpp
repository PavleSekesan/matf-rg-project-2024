
#ifndef MAINCONTROLLER_HPP
#define MAINCONTROLLER_HPP
#include <engine/platform/PlatformController.hpp>
#include <engine/platform/PlatformEventObserver.hpp>
#include <vector>
#include <glm/glm.hpp>

namespace engine::test::app {
class MainPlatformEventObserver final : public engine::platform::PlatformEventObserver {
public:
    void on_key(engine::platform::Key key) override;

    void on_mouse_move(engine::platform::MousePosition position) override;
};

enum SelectedShape {
    Cube, Sphere
};

struct ShapeData {
    SelectedShape selected_shape{Cube};
    glm::vec3 position{0.0f, 0.0f, 0.0f};
};

class MainController final : public engine::core::Controller {
public:
    SelectedShape m_selected_shape;

    std::string_view name() const override {
        return "test::app::MainController";
    }

private:
    void initialize() override;

    bool loop() override;

    void poll_events() override;

    void update() override;

    void begin_draw() override;

    void draw() override;

    void end_draw() override;

    void draw_skybox();

    void draw_backpack();

    void draw_cubes();

    void update_camera();

    glm::vec3 get_camera_ray();

    std::vector<ShapeData> m_shapes;
    float m_backpack_scale{1.0f};
    bool m_draw_gui{false};
    bool m_cursor_enabled{true};
};
}
#endif //MAINCONTROLLER_HPP
