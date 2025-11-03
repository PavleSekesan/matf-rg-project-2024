
#include <engine/resources/Model.hpp>
#include <engine/resources/Shader.hpp>

namespace engine::resources {

void Model::draw(const Shader *shader) {
    shader->use();
    for (auto &mesh: m_meshes) {
        mesh.draw(shader);
    }
}

void Model::destroy() {
    for (auto &mesh: m_meshes) {
        mesh.destroy();
    }
}

bool Model::collides_with_ray(glm::vec3 orig, glm::vec3 dir) const {
    for (auto &mesh: m_meshes) {
        if (mesh.collides_with_ray(orig, dir))
            return true;
    }
    return false;
}

}
