//#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 uv;

void main() {
    uv = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}


//#shader fragment
#version 330 core

out vec4 fragColor;

in vec2 uv;

uniform sampler2D texture_diffuse1;
uniform float time;

// simple pseudo-random noise
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec4 scene = texture(texture_diffuse1, uv);

    // move snow downward with time
    float speedy = 1.0;
    float speedx = 0.1;
    float intensity = 1.0;
    float jitterAmount = 0.1;
    float x = mod(uv.x + time * speedx, 1.0);
    float y = mod(uv.y + time * speedy, 1.0);

    // multiple layers for parallax snow
    float snow = 0.0;
    for (int i = 0; i < 5; i++) {
        float scale = pow(2.0, float(i));
        vec2 p = vec2(x * scale, y * scale);

        // add time-varying jitter per layer
        vec2 jittered = p + vec2(
            sin(time * 2.0) * jitterAmount,
            0.0
        );

        snow += smoothstep(0.95, 1.0, rand(floor(jittered * 100.0)));
    }
    snow /= 5.0;

    // final mix
    vec3 color = mix(scene.rgb, vec3(1.0), snow * intensity);
    fragColor = vec4(color, 1.0);
}
