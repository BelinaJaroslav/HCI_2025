#pragma once

//#include <GL/glew.h> 
#include <glm/glm.hpp> 
//#include <glm/gtc/type_ptr.hpp>

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;

    bool operator == (const vertex& v1) const
    {
        return (position == v1.position
            && normal == v1.normal
            && texture_coordinates == v1.texture_coordinates);
    }
};
