#include<glad/glad.h>
#include <engine/util/Utils.hpp>
#include <engine/resources/Mesh.hpp>
#include <engine/resources/Shader.hpp>
#include <unordered_map>

namespace engine::resources {

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
           std::vector<Texture *> textures) {
    // NOLINTBEGIN
    static_assert(std::is_trivial_v<Vertex>);
    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

    glBindVertexArray(0);
    // NOLINTEND
    m_vao = VAO;
    m_num_indices = indices.size();
    m_textures = std::move(textures);
    m_vertices = vertices;
    m_indices = indices;
}

bool Mesh::triangle_collides_with_ray(glm::vec3 orig, glm::vec3 dir,  glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
    const float EPSILON = 1e-8f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);
    if (fabs(a) < EPSILON)
        return false;

    float f = 1.0f / a;
    glm::vec3 s = orig - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = f * glm::dot(edge2, q);
    return t > EPSILON;
}

bool Mesh::collides_with_ray(glm::vec3 orig, glm::vec3 dir) const {
    for (int i = 0; i < m_num_indices - 2; i++) {
        auto v0 = m_vertices[i].Position;
        auto v1 = m_vertices[i+1].Position;
        auto v2 = m_vertices[i+2].Position;
        if (triangle_collides_with_ray(orig, dir, v0, v1, v2))
            return true;
    }
    return false;
}

void Mesh::draw(const Shader *shader) {
    std::unordered_map<std::string_view, uint32_t> counts;
    std::string uniform_name;
    uniform_name.reserve(32);
    for (int i = 0; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        const auto &texture_type = Texture::uniform_name_convention(m_textures[i]->type());
        uniform_name.append(texture_type);
        const auto count = (counts[texture_type] += 1);
        uniform_name.append(std::to_string(count));
        shader->set_int(uniform_name, i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]->id());
        uniform_name.clear();
    }
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::destroy() {
    glDeleteVertexArrays(1, &m_vao);
}

}
