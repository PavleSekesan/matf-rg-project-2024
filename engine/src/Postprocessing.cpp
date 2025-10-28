#include <engine/graphics/OpenGL.hpp>
#include <engine/resources/ResourcesController.hpp>
#include <engine/resources/ShaderCompiler.hpp>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <engine/platform/PlatformController.hpp>
#include <engine/postprocessing/Postprocessing.hpp>
#include <engine/postprocessing/ScreenQuad.hpp>
#include <engine/util/Configuration.hpp>

namespace engine::postprocessing {

void Postprocessing::generate_framebuffer() {
    auto &config = util::Configuration::config();
    int window_width = config["window"]["width"];
    int window_height = config["window"]["height"];
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &m_tex_id);
    glBindTexture(GL_TEXTURE_2D, m_tex_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_id, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);      // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);// now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) spdlog::error("Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Postprocessing::activate() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Postprocessing::draw_to_screen() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    engine::graphics::OpenGL::disable_depth_testing();
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex_id);
    render_quad();
}

}