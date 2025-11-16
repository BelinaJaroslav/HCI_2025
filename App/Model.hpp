#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory> 

#include <GL/glew.h>
#include <glm/glm.hpp> 

#include "Assets.hpp"
#include "Mesh.hpp"
//#include "NonCopyable.hpp"
#include "OBJLoader.hpp"
#include "ShaderProgram.hpp"


class Model// : private NonCopyable
{
public:
    // origin point of whole model
    glm::vec3 pivot_position{}; // [0,0,0] of the object
    glm::vec3 eulerAngles{};    // pitch, yaw, roll
    glm::vec3 scale{1.0f};

    // mesh related data
    struct mesh_package
    {
        std::shared_ptr<Mesh> mesh;             // geometry & topology, vertex attributes
        std::shared_ptr<ShaderProgram> shader;  // which shader to use to draw this part of the model

        glm::vec3 origin;       // mesh origin relative to origin of the whole model
        glm::vec3 eulerAngles;  // mesh rotation relative to orientation of the whole model
        glm::vec3 scale;        // mesh scale relative to scale of the whole model
    };
    
    std::vector<mesh_package> meshes;
    
    Model() = default;
    Model(const std::filesystem::path & filename, std::shared_ptr<ShaderProgram> shader) {
        // Load mesh (all meshes) of the model, (in the future: load material of each mesh, load textures...)
        // notice: you can load multiple meshes and place them to proper positions, 
        //            multiple textures (with reusing) etc. to construct single complicated Model   
        //
        // This can be done by extending OBJ file parser (OBJ can load hierarchical models),
        // or by your own JSON model specification (or keep it simple and set a rule: 1model=1mesh ...) 
        //

        std::vector<vertex> vertices;
        std::vector<GLuint> indices;

        load_OBJ_GDrive(filename, vertices, indices);


        auto mesh_shared_ptr = make_shared<Mesh>(vertices, indices, GL_TRIANGLES);

        addMesh(mesh_shared_ptr, shader);
    }

    void addMesh(std::shared_ptr<Mesh> mesh,
                 std::shared_ptr<ShaderProgram> shader,
                 glm::vec3 origin = glm::vec3(0.0f),      // dafault value
                 glm::vec3 euler_angles = glm::vec3(0.0f), // dafault value
                 glm::vec3 scale = glm::vec3(1.0f)        // dafault value
                 ) {
        meshes.emplace_back(mesh, shader, origin, euler_angles, scale);
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
            mesh_pkg.mesh->draw();        // draw mesh
        }
    }
};

