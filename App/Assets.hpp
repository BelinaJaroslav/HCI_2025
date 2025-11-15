#pragma once

//#include <GL/glew.h> 
//#include <GL/wglew.h>
#include <glm/glm.hpp> 
//#include <glm/gtc/type_ptr.hpp>

struct vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coords;

    bool operator == (const vertex& v1) const
    {
        return (position == v1.position
            && normal == v1.normal
            && texture_coords == v1.texture_coords);
    }
};
