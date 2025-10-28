//
// Created by pavle on 10/26/25.
//

#ifndef MATF_RG_PROJECT_FRAMEBUFFER_HPP
#define MATF_RG_PROJECT_FRAMEBUFFER_HPP

#include <memory>
#include <engine/resources/Texture.hpp>

namespace engine::postprocessing {
class Postprocessing {
    unsigned int m_tex_id;
    unsigned int m_rbo;

public:
    unsigned int fbo;
    resources::Shader *shader;

    void generate_framebuffer();

    void activate() const;

    void draw_to_screen();
};
}

#endif //MATF_RG_PROJECT_FRAMEBUFFER_HPP