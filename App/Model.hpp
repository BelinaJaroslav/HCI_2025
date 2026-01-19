#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "OBJLoader.hpp"
#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"


// 1 simple model has 1 mesh
class Model
{
private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Texture> texture;
    glm::mat4 model_mx{};

public:
    // === NORMAL MODEL ===
    Model(const std::filesystem::path& filename,
        std::shared_ptr<ShaderProgram> shader,
        std::shared_ptr<Texture> texture = std::make_shared<Texture>()
    ):
        shader(shader),
        texture(texture)
    {
        std::vector<vertex> vertices;
        std::vector<GLuint> indices;
        load_OBJ(filename, vertices, indices);
        mesh = std::make_shared<Mesh>(vertices, indices, GL_TRIANGLES);
    }

    // === HEIGHTMAP ===
    Model(const std::filesystem::path& filename,
        std::shared_ptr<ShaderProgram> shader,
        std::map<std::pair<float, float>, float>& heightmap_heights,
        std::shared_ptr<Texture> texture
    ) :
        shader(shader),
        texture(texture)
    {
        std::vector<vertex> vertices;
        std::vector<GLuint> indices;
        load_heightmap(filename, vertices, indices, heightmap_heights);
        mesh = std::make_shared<Mesh>(vertices, indices, GL_TRIANGLES);
    }

    // Transformations
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec4 rotation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // axes xyz + angle (deg)
    glm::vec4 rotation_base = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // if model is weirdly rotated, it can be fixed by setting this; other rotations are relative to this

    // Draw
    void draw()
    {
        shader->activate();

        // Einheitsmatrix
        model_mx = glm::identity<glm::mat4>();
        // Move object
        model_mx = glm::translate(model_mx, position);
        // Scale object
        model_mx = glm::scale(model_mx, glm::vec3(scale));
        // Rotate
        model_mx = glm::rotate(model_mx, glm::radians(rotation_base.w), glm::vec3(rotation_base.x, rotation_base.y, rotation_base.z));
        model_mx = glm::rotate(model_mx, glm::radians(rotation.w), glm::vec3(rotation.x, rotation.y, rotation.z));        

        shader->set_uniform("u_model_mx", model_mx);

        texture->bind();
        shader->set_uniform("tex0", 0);

        mesh->draw();
    }
};
