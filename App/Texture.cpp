#include "Texture.hpp"


void Texture::generate_checkboard(void)
{
    if (glIsTexture(checkboard) != GL_TRUE) { // default checker-board texture yet not valid texture
        glCreateTextures(GL_TEXTURE_2D, 1, &checkboard);

        cv::Vec3b black{ 0, 0, 0 };
        cv::Vec3b white{ 255, 255, 255 };
        cv::Mat ckb = cv::Mat(2, 2, CV_8UC3, black); // 2x2 RGB pixels, default pixel color = black
        ckb.at<cv::Vec3b>(0, 0) = white;
        ckb.at<cv::Vec3b>(1, 1) = white;

        glTextureStorage2D(checkboard, 1, GL_RGB8, ckb.cols, ckb.rows);
        glTextureSubImage2D(checkboard, 0, 0, 0, ckb.cols, ckb.rows, GL_BGR, GL_UNSIGNED_BYTE, ckb.data);
        glTextureParameteri(checkboard, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(checkboard, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(checkboard, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(checkboard, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}


cv::Mat Texture::load_image(const std::filesystem::path& path)
{
    cv::Mat image = cv::imread(path.string(), cv::IMREAD_UNCHANGED); // Read with (potential) alpha, do not rotate by EXIF.

    // check! cv::imread does NOT throw exception, if the image is not found.
    if (image.empty()) {
        throw std::runtime_error{ std::string("no texture in file: ").append(path.string()) };
    }

    std::cout << "Loaded texture image: " << path.string() << "\n";

    return image;
}


Texture::Texture(void)
{
    Texture::generate_checkboard();
    name = Texture::checkboard;
}

Texture::Texture(const std::filesystem::path& path, Interpolation interpolation) : Texture{ load_image(path), interpolation } {}

Texture::Texture(const glm::vec3& vec) : Texture{ cv::Mat{1, 1, CV_8UC3, cv::Scalar{vec.b, vec.g, vec.r}}, Interpolation::nearest } {}

Texture::Texture(const glm::vec4& vec) : Texture{ cv::Mat{1, 1, CV_8UC4, cv::Scalar{vec.b, vec.g, vec.r, vec.a}}, Interpolation::nearest } {}

Texture::Texture(cv::Mat const& image, Interpolation interpolation)
{
    if (image.empty()) {
        throw std::runtime_error{ "the input image is empty" };
    }

    cv::flip(image, image, 0); // OpenGL vs. Window coordinates...

    glCreateTextures(GL_TEXTURE_2D, 1, &name);

    switch (image.type()) {
    case CV_8UC1: // single channel image - greyscale
        // upload only one channel
        glTextureStorage2D(name, 1, GL_R8, image.cols, image.rows);
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_RED, GL_UNSIGNED_BYTE, image.data);
        // use data also for other channels
        glTextureParameteri(name, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTextureParameteri(name, GL_TEXTURE_SWIZZLE_B, GL_RED);
        break;
    case CV_8UC3: // RGB
        glTextureStorage2D(name, 1, GL_RGB8, image.cols, image.rows);
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case CV_8UC4: // RGBA
        glTextureStorage2D(name, 1, GL_RGBA8, image.cols, image.rows);
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
        break;
    default:
        throw std::runtime_error{ "unsupported number of channels or channel depth in texture" };
    }

    set_interpolation(interpolation);

    // Configures the way the texture repeats
    glTextureParameteri(name, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(name, GL_TEXTURE_WRAP_T, GL_REPEAT);

    std::cout << "Texture name=" << name << " created.\n";
}


Texture::~Texture()
{
    glDeleteTextures(1, &name);
}


GLuint Texture::get_name() const
{
    return name;
}


void Texture::bind(void)
{
    glBindTextureUnit(0, name); // bind to some texturing unit, e.g. 0
}


void Texture::set_interpolation(Interpolation interpolation)
{
    // Select texture filering method 
    switch (interpolation) {
    case Interpolation::nearest:
        // nearest neighbor - ugly & fast 
        glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;
    case Interpolation::linear:
        // bilinear - nicer & slower
        glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    case Interpolation::linear_mipmap_linear:
        // Trilinear: MIPMAP filtering + automatic MIPMAP generation - nicest, needs more memory. Notice: MIPMAP is only for image minifying.
        glTextureParameteri(name, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear magnifying
        glTextureParameteri(name, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // trilinear minifying
        glGenerateTextureMipmap(name); // Generate mipmaps now.
        break;
    }
}


int Texture::get_height(void)
{
    int tex_height = 0;
    int basemiplevel = 0; // base image
    glGetTextureLevelParameteriv(name, basemiplevel, GL_TEXTURE_HEIGHT, &tex_height);

    return tex_height;
}


int Texture::get_width(void)
{
    int tex_width = 0;
    int basemiplevel = 0; // base image
    glGetTextureLevelParameteriv(name, basemiplevel, GL_TEXTURE_WIDTH, &tex_width);

    return tex_width;
}


void Texture::replace_image(const cv::Mat& image)
{
    // immutable texture format used: only content can be changed (size and data format MUST match)

    // check size
    if ((image.rows != get_height()) || (image.cols != get_width()))
        throw std::runtime_error("improper image replacement size");

    // check channels and format
    int tex_format = 0;
    int basemiplevel = 0; // base image
    glGetTextureLevelParameteriv(name, basemiplevel, GL_TEXTURE_INTERNAL_FORMAT, &tex_format);

    switch (image.type()) {
    case CV_8UC1: // single channel image - greyscale
        if (tex_format != GL_R8)
            throw std::runtime_error("improper image replacement channel data, GL_R8 was the original");
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_RED, GL_UNSIGNED_BYTE, image.data);
        break;
    case CV_8UC3: // RGB
        if (tex_format != GL_RGB8)
            throw std::runtime_error("improper image replacement channel data, GL_RGB8 was the original");
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_BGR, GL_UNSIGNED_BYTE, image.data);
        break;
    case CV_8UC4: // RGBA
        if (tex_format != GL_RGBA8)
            throw std::runtime_error("improper image replacement channel data, GL_RGBA8 was the original");
        glTextureSubImage2D(name, 0, 0, 0, image.cols, image.rows, GL_BGRA, GL_UNSIGNED_BYTE, image.data);
        break;
    default:
        throw std::runtime_error{ "unsupported number of channels or channel depth in texture" };
    }
}
