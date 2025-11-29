#pragma once 

#include <filesystem>
#include <opencv2/opencv.hpp>
#include <GL/glew.h> 
#include <glm/glm.hpp>

#include "NonCopyable.hpp"


class Texture : private NonCopyable
{
public:
    enum class Interpolation
    {
        nearest,
        linear,
        linear_mipmap_linear,
    };

    Texture();
    Texture(const cv::Mat& image, Interpolation interpolation = Interpolation::linear_mipmap_linear); // default = best texture filtering
    Texture(const glm::vec3& vec); // synthetic single-color RGB texture
    Texture(const glm::vec4& vec); // synthetic single-color RGBA texture
    Texture(const std::filesystem::path& path, Interpolation interpolation = Interpolation::linear_mipmap_linear);
    
    ~Texture();

    void bind(void);
    GLuint get_name() const;
    int get_height(void);
    int get_width(void);
    void set_interpolation(Interpolation interpolation);
    void replace_image(const cv::Mat& image);
    void replace_color(const glm::vec3& vec);

private:
    cv::Mat load_image(const std::filesystem::path& path);

    static void generate_checkboard(void); // create default texture
    
    static inline GLuint checkboard; // class-shared ckboard variable
    
    GLuint name; // set default-constructed texture to ckboard pattern
};
