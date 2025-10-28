#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

#include <spdlog/spdlog.h>

#include "GUIController.hpp"
#include "MainController.hpp"

class MyApp : public engine::core::App {
public:
    void app_setup() override {
        auto main_controller = register_controller<MainController>();
        auto gui_controller = register_controller<GUIController>();
        gui_controller->after(main_controller);
        main_controller->after(engine::core::Controller::get<engine::core::EngineControllersEnd>());
    };
};

int main(int argc, char **argv) { return std::make_unique<MyApp>()->run(argc, argv); }