#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory> 

#include <GL/glew.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include "Assets.hpp"
#include "Mesh.hpp"
#include "OBJLoader.hpp"
#include "ShaderProgram.hpp"
#include "Texture.hpp"


class Model
{
public:
    // origin point of whole model
    glm::vec3 pivot_position{}; // [0,0,0] of the object
    glm::vec3 euler_angles{};    // pitch, yaw, roll
    glm::vec3 scale{ 1.0f };

    glm::mat4 local_model_matrix{ 1.0 }; // cache, and for complex transformations (default = identity) 

    // mesh related data
    struct mesh_package
    {
        std::shared_ptr<Mesh> mesh;             // geometry & topology, vertex attributes
        std::shared_ptr<ShaderProgram> shader;  // which shader to use to draw this part of the model
        std::shared_ptr<Texture> texture;

        glm::vec3 origin;       // mesh origin relative to origin of the whole model
        glm::vec3 euler_angles; // mesh rotation relative to orientation of the whole model
        glm::vec3 scale;        // mesh scale relative to scale of the whole model
    };
    
    std::vector<mesh_package> meshes;
    
    Model() = default;

    // === NORMAL MODEL ===
    Model(const std::filesystem::path& filename,
        std::shared_ptr<ShaderProgram> shader,
        std::shared_ptr<Texture> texture_shared_ptr = std::make_shared<Texture>()
    )
    {
        std::vector<vertex> vertices;
        std::vector<GLuint> indices;

        load_OBJ_GDrive(filename, vertices, indices);
        //load_OBJ_PG2(filename, vertices, indices);

        auto mesh_shared_ptr = std::make_shared<Mesh>(vertices, indices, GL_TRIANGLES);

        add_mesh(mesh_shared_ptr, shader, texture_shared_ptr);
    }

    // === HEIGHTMAP ===
    Model(const std::filesystem::path& filename,
        std::shared_ptr<ShaderProgram> shader,
        std::map<std::pair<float, float>, float>& _heights
    )
    {
        std::vector<vertex> vertices;
        std::vector<GLuint> indices;

        load_heightmap(filename, vertices, indices, _heights);        

        auto mesh_shared_ptr = std::make_shared<Mesh>(vertices, indices, GL_TRIANGLES);

        auto texture_shared_ptr = std::make_shared<Texture>();

        add_mesh(mesh_shared_ptr, shader, texture_shared_ptr);
    }


    void add_mesh(std::shared_ptr<Mesh> mesh,
        std::shared_ptr<ShaderProgram> shader,
        std::shared_ptr<Texture> texture,
        glm::vec3 origin = glm::vec3(0.0f),
        glm::vec3 euler_angles = glm::vec3(0.0f),
        glm::vec3 scale = glm::vec3(1.0f)
    )
    {
        meshes.emplace_back(mesh, shader, texture, origin, euler_angles, scale);
    }


    // update based on running time
    void update(const float delta_t) {
        // change internal state of the model (positions of meshes, size, etc.) 
        // note: this allows dynamic behaviour - it can be modified to 
        //       use lambda funtion, call scripting language, etc. 
    }
    

    void draw() {
        // call draw() on mesh (all meshes)
        for (auto const& mesh_pkg : meshes) {
            mesh_pkg.shader->activate();  // select proper shader

            glm::mat4 mesh_model_matrix = create_model_mx(mesh_pkg.origin, mesh_pkg.euler_angles, mesh_pkg.scale);
            mesh_pkg.shader->set_uniform("u_model_mx", mesh_model_matrix * local_model_matrix);

            mesh_pkg.texture->bind();
            mesh_pkg.shader->set_uniform("tex0", 0);

            //std::cout << mesh_pkg.texture->get_name() << "\n";

            mesh_pkg.mesh->draw();
        }
    }


    float wrap_angle(float angle)
    { // wrap any float to [0, 360)
        angle = std::fmod(angle, 360.0f);
        if (angle < 0.0f) {
            angle += 360.0f;
        }
        return angle;
    }


    glm::mat4 create_model_mx(const glm::vec3& origin, const glm::vec3& euler_angles, const glm::vec3& scale)
    {
        // keep angles in proper range
        glm::vec3 euler_angles_wrapped{ wrap_angle(euler_angles.x), wrap_angle(euler_angles.y), wrap_angle(euler_angles.z) };

        glm::mat4 t = glm::translate(glm::mat4(1.0f), origin);
        glm::mat4 rotm = glm::yawPitchRoll(glm::radians(euler_angles_wrapped.y), glm::radians(euler_angles_wrapped.x), glm::radians(euler_angles_wrapped.z)); //yaw, pitch, roll
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

        return s * rotm * t;
    }


    void set_position(const glm::vec3& new_position)
    {
        pivot_position = new_position;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }
    void set_euler_angles(const glm::vec3& new_euler_angles)
    {
        euler_angles = new_euler_angles;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }
    void set_scale(const glm::vec3& new_scale)
    {
        scale = new_scale;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }    
    void set_model_mx(const glm::mat4& model_mx)
    { // for complex (externally provided) transformations 
        local_model_matrix = model_mx;
    }
    void relative_translate(const glm::vec3& offset)
    {
        pivot_position += offset;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }
    void relative_rotate(const glm::vec3& pitch_yaw_roll_offs)
    {
        euler_angles += pitch_yaw_roll_offs;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }
    void relative_scale(const glm::vec3& scale_offs)
    {
        scale *= scale_offs;
        local_model_matrix = create_model_mx(pivot_position, euler_angles, scale);
    }
};

