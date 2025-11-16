#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

#include <GL/glew.h> 
#include <glm/glm.hpp>

#include "NonCopyable.hpp"


class ShaderProgram : private NonCopyable {
public:
    // No default constructor. RAII - if constructed, it will be correctly initialized
    // and can be rendered. OpenGL resources are guaranteed to be deallocated using destructor. 
    // Double-free errors are prevented by making class non-copyable (therefore 
    // double destruction of the same OpenGL shader is prevented). 
    ShaderProgram() = delete; // does nothing
    // you can add more constructors for pipeline with GS, TS etc.
    ShaderProgram(std::filesystem::path const & VS_file, std::filesystem::path const & FS_file);
    ~ShaderProgram(void);

    void activate();
    void deactivate();
    void clear();
    
    GLuint get_ID(void) const { return ID; }
    //GLint get_attrib_location(const std::string & name);
    
    // set uniform according to name 
    // https://docs.gl/gl4/glUniform
    void set_uniform(const std::string & name, const GLfloat val);      
    void set_uniform(const std::string & name, const GLint val);
    void set_uniform(const std::string & name, const glm::vec3 & val);
    void set_uniform(const std::string & name, const glm::vec4 & val);
    void set_uniform(const std::string & name, const glm::mat3 & val);
    void set_uniform(const std::string & name, const glm::mat4 & val);
    void set_uniform(const std::string & name, const std::vector<GLint> & val);
    void set_uniform(const std::string & name, const std::vector<GLfloat> & val);
    void set_uniform(const std::string & name, const std::vector<glm::vec3> & val);

private:
    GLuint ID{0}; // default = 0, empty shader
    inline static GLuint currently_used_ID{0};

    std::unordered_map<std::string, GLuint> uniform_location_cache;

    GLuint get_uniform_location(const std::string & name);

    std::string read_text_file(const std::filesystem::path & filename); // load text file

    GLuint compile_shader(const std::string & source_code, const GLenum type); 
    std::string get_shader_info_log(const GLuint obj);    

    GLuint link_shader(const std::vector<GLuint> shader_ids); 
    std::string get_program_info_log(const GLuint obj);      
};
