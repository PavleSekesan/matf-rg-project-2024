//
// Created by pavle on 10/27/25.
//

#ifndef MATF_RG_PROJECT_BLOOM_HPP
#define MATF_RG_PROJECT_BLOOM_HPP
#include <engine/resources/Shader.hpp>

class Bloom {
    static constexpr bool BLOOM_ACTIVE = true;
    unsigned int hdrFBO;
    unsigned int colorBuffers[2];
    unsigned int pingpongFBO[2];
    unsigned int pingpongColorbuffers[2];

public:
    engine::resources::Shader *shaderBlur;
    engine::resources::Shader *shaderBloomFinal;

    void generateFramebuffers();

    void attachHdrFbo() const;

    void blurBrightFragments(unsigned int framebuffer);
};

#endif //MATF_RG_PROJECT_BLOOM_HPP