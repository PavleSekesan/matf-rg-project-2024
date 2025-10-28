//#shader fragment
#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    bool directional;
};

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

#define NUM_LIGHTS 2
uniform Light light[NUM_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_emissive1;

vec4 computeLighting(vec3 normal, Light light, vec3 viewPos, vec3 fragPos) {
    vec3 baseColor = texture(texture_diffuse1, TexCoords).rgb;
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.0 * distance + 1.0 * (distance * distance));
    // ambient
    vec3 ambient = 0.1 * light.color * baseColor;

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = light.directional ? normalize(light.position) : normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.9 * light.color * (diff * baseColor);

    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    vec3 specular = 0.0 * light.color * (spec * baseColor);

    vec3 result = ambient + diffuse + specular;
    if (!light.directional) {
        result *= attenuation;
    }
    return vec4(result, 1.0);
}

void main() {
    vec4 lighting = vec4(0.0);
    for (int i = 0; i < NUM_LIGHTS; i++) {
        lighting += computeLighting(Normal, light[i], viewPos, FragPos);
    }

    vec4 emissive = vec4(texture(texture_emissive1, TexCoords).rgb, 1.0);
    FragColor = lighting + 100.0 * emissive;

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 3.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
