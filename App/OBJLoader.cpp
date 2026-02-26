#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include <GL/glew.h> 
#include <glm/glm.hpp>

#include <fmt/core.h>

#include "OBJLoader.hpp"
#include "HeightmapConfig.hpp"


void load_OBJ(const std::filesystem::path& file_name, std::vector<vertex>& mesh_vertices, std::vector<GLuint>& mesh_vertex_indices)
{
    fmt::println("Loading OBJ file {} ...", file_name.string());

    mesh_vertices.clear();
    mesh_vertex_indices.clear();

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texture_coordinates;
    std::vector<glm::vec3> vertex_normals;
    std::vector<GLuint> indices_vertex, indices_texture_coordinate, indices_vertex_normal;

    std::string first_two_chars, first_three_chars;
    glm::vec2 uv;
    glm::vec3 vertex_or_normal;

    bool line_success;

    std::string line;
    std::ifstream file_reader(file_name);
    while (getline(file_reader, line)) {
        if (!line.empty()) {
            line_success = true;
            first_two_chars = line.substr(0, 2);
            first_three_chars = line.substr(0, 3);
            // v -1.183220029 4.784470081 47.4618988
            if (first_two_chars == "v ") {
                vertex_or_normal = {};
                (void)sscanf(line.c_str(), "v %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertices.push_back(vertex_or_normal);
            }
            // vt 0.5000 0.7500
            else if (first_three_chars == "vt ") {
                uv = {};
                (void)sscanf(line.c_str(), "vt %f %f", &uv.x, &uv.y);
                uv.y = -uv.y; // DDS textures are inverted
                texture_coordinates.push_back(uv);
            }
            // vn 0.7235898972 -0.6894102097 -0.03363365307
            else if (first_three_chars == "vn ") {
                vertex_or_normal = {};
                (void)sscanf(line.c_str(), "vn %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertex_normals.push_back(vertex_or_normal);
            }
            else if (first_two_chars == "f ") {
                auto n = std::count(line.begin(), line.end(), '/');
                // f 1 2 3
                if (n == 0) {
                    unsigned int indices_temp[3]{};
                    (void)sscanf(line.c_str(), "f %d %d %d", &indices_temp[0], &indices_temp[1], &indices_temp[2]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                }
                // f 3/1 4/2 5/3
                else if (n == 3) {
                    unsigned int indices_temp[6]{};
                    (void)sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                }
                else if (n == 6) {
                    // f 7//1 8//2 9//3
                    if (line.find("//") != std::string::npos) {
                        unsigned int indices_temp[6]{};
                        (void)sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                    }
                    // f 6/4/1 3/5/3 7/6/5
                    else {
                        unsigned int indices_temp[9]{};
                        (void)sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[6], &indices_temp[1], &indices_temp[4], &indices_temp[7], &indices_temp[2], &indices_temp[5], &indices_temp[8]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[6], indices_temp[7], indices_temp[8] });
                    }
                }
                // f 1/1/1 2/2/2 22/23/3 21/22/4
                else if (n == 8) {
                    unsigned int v[4]{};
                    unsigned int vt[4]{};
                    unsigned int vn[4]{};
                    (void)sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);
                    indices_vertex.insert(indices_vertex.end(), { v[0], v[1], v[2], v[0], v[2], v[3] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { vt[0], vt[1], vt[2], vt[0], vt[2], vt[3] });
                    indices_vertex_normal.insert(indices_vertex_normal.end(), { vn[0], vn[1], vn[2], vn[0], vn[2], vn[3] });
                }
                else {
                    line_success = false;
                }
            }
            else {
                line_success = false;
            }

            if (!line_success && first_two_chars != "# ") {
                fmt::println("OBJLoader@{}> Ignoring line '{}'", file_name.string(), line);
            }
        }
    }
    file_reader.close();    

    // Indirect -> direct
    std::vector<glm::vec3> vertices_direct;
    std::vector<glm::vec2> texture_coordinates_direct;
    std::vector<glm::vec3> vertex_normals_direct;

    for (unsigned int u = 0; u < indices_vertex.size(); u++) {
        vertices_direct.push_back(vertices[indices_vertex[u] - 1]);
    }
    for (unsigned int u = 0; u < indices_texture_coordinate.size(); u++) {
        texture_coordinates_direct.push_back(texture_coordinates[indices_texture_coordinate[u] - 1]);
    }
    for (unsigned int u = 0; u < indices_vertex_normal.size(); u++) {
        vertex_normals_direct.push_back(vertex_normals[indices_vertex_normal[u] - 1]);
    }

    auto n_direct_uvs = texture_coordinates_direct.size();
    auto n_direct_normals = vertex_normals_direct.size();

    // Vectors to Vertex vector
    for (unsigned int u = 0; u < vertices_direct.size(); u++) {
        vertex vertex{};
        vertex.position = vertices_direct[u];
        if (u < n_direct_uvs) vertex.texture_coordinates = texture_coordinates_direct[u];
        if (u < n_direct_normals) vertex.normal = vertex_normals_direct[u];
        mesh_vertices.push_back(vertex);
        mesh_vertex_indices.push_back(u);
    }
    fmt::println("Done loading OBJ file {}", file_name.string());
}


void load_heightmap(const std::filesystem::path& file_name, std::vector<vertex>& mesh_vertices, std::vector<GLuint>& mesh_vertex_indices, std::map<std::pair<float, float>, float>& heightmap_heights)
{
    mesh_vertices.clear();
    mesh_vertex_indices.clear();

    cv::Mat hmap = cv::imread(file_name.string(), cv::IMREAD_GRAYSCALE);
    if (hmap.empty()) std::cerr << "[!] Height map empty? File: " << file_name << "\n";

    const unsigned int mesh_step_size = 10;

    std::cout << "Heightmap size: " << hmap.size << ", channels: " << hmap.channels();

    if (hmap.channels() != 1) std::cerr << "[!] Height map requested 1 channel, got: " << hmap.channels() << "\n";

    // Create heightmap mesh from TRIANGLES in XZ plane, Y is UP (right hand rule)
    //
    //   3-----2
    //   |    /|
    //   |  /  |
    //   |/    |
    //   0-----1
    //
    //   012,023

    glm::vec3 normalA{};
    glm::vec3 normalB{};
    glm::vec3 normal{};
    unsigned int indices_counter = 0;

    std::map<std::pair<unsigned int, unsigned int>, glm::vec3> normal_sums;
    std::pair<unsigned int, unsigned int> pair, pair0, pair1, pair2, pair3;

    for (unsigned int x_coord = 0; x_coord < (hmap.cols - mesh_step_size); x_coord += mesh_step_size) {
        for (unsigned int z_coord = 0; z_coord < (hmap.rows - mesh_step_size); z_coord += mesh_step_size) {
            // Get The (X, Y, Z) Value For The Bottom Left Vertex = 0
            glm::vec3 p0(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Bottom Right Vertex = 1
            glm::vec3 p1(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)), z_coord);
            // Get The (X, Y, Z) Value For The Top Right Vertex = 2
            glm::vec3 p2(x_coord + mesh_step_size, hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)), z_coord + mesh_step_size);
            // Get The (X, Y, Z) Value For The Top Left Vertex = 3
            glm::vec3 p3(x_coord, hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)), z_coord + mesh_step_size);

            // Get max normalized height for tile, set texture accordingly
            // Grayscale image returns 0..256, normalize to 0.0f..1.0f by dividing by 256 (255 ?)
            float max_h = std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord)) / 255.0f,
                std::max(hmap.at<uchar>(cv::Point(x_coord, z_coord + mesh_step_size)) / 255.0f,
                    std::max(hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord + mesh_step_size)) / 255.0f,
                        hmap.at<uchar>(cv::Point(x_coord + mesh_step_size, z_coord)) / 255.0f
                    )));

            // Get texture coords in vertices, bottom left of geometry == bottom left of texture
            glm::vec2 tc0 = heightmap_get_subtex_by_height(max_h);
            glm::vec2 tc1 = tc0 + glm::vec2((1.0f / 16), 0.0f);		    // add offset for bottom right corner
            glm::vec2 tc2 = tc0 + glm::vec2((1.0f / 16), (1.0f / 16));  // add offset for top right corner
            glm::vec2 tc3 = tc0 + glm::vec2(0.0f, (1.0f / 16));         // add offset for bottom left corner

            // - calculate normal vector            
            normalA = glm::normalize(glm::cross(p1 - p0, p2 - p0));
            normalB = glm::normalize(glm::cross(p2 - p0, p3 - p0));
            normal = (normalA + normalB) / 2.0f;

            // - place vertices and ST to mesh
            mesh_vertices.emplace_back(vertex{ p0, -normal, tc0 });
            mesh_vertices.emplace_back(vertex{ p1, -normal, tc1 });
            mesh_vertices.emplace_back(vertex{ p2, -normal, tc2 });
            mesh_vertices.emplace_back(vertex{ p3, -normal, tc3 });

            // - place indices
            indices_counter += 4;
            mesh_vertex_indices.emplace_back(indices_counter - 4);
            mesh_vertex_indices.emplace_back(indices_counter - 2);
            mesh_vertex_indices.emplace_back(indices_counter - 3);
            mesh_vertex_indices.emplace_back(indices_counter - 4);
            mesh_vertex_indices.emplace_back(indices_counter - 1);
            mesh_vertex_indices.emplace_back(indices_counter - 2);

            // - normal averaging
            pair0 = { x_coord, z_coord };
            pair1 = { x_coord + mesh_step_size, z_coord };
            pair2 = { x_coord + mesh_step_size, z_coord + mesh_step_size };
            pair3 = { x_coord, z_coord + mesh_step_size };
            normal_sums[pair0] -= normal;
            normal_sums[pair1] -= normal;
            normal_sums[pair2] -= normal;
            normal_sums[pair3] -= normal;
        }
    }

    // - normal averaging, 2nd iter
    for (auto& vertex : mesh_vertices) {
        pair = { static_cast<unsigned int>(vertex.position.x), static_cast<unsigned int>(vertex.position.z) };
        vertex.normal = glm::normalize(normal_sums[pair]); // no need to divide by four, we can just normalize

        heightmap_heights[{vertex.position.x* HEIGHTMAP_SCALE, vertex.position.z* HEIGHTMAP_SCALE}] = vertex.position.y; // for heightmap collision
    }

   std::cout << ", vertices: " << mesh_vertices.size() << " ... (done loading heightmap)\n";
}

glm::vec2 heightmap_get_subtex_S_and_T(const int x, const int y)
{
    return glm::vec2((x * 1.0f / 16), (y * 1.0f / 16)); // Expects tilemap with 16 rows&cols; interpolation is dealt with via bleeding pixels
}

glm::vec2 heightmap_get_subtex_by_height(float height)
{
    if (height > 0.9) return heightmap_get_subtex_S_and_T(4, 4);
    else if (height > 0.8) return heightmap_get_subtex_S_and_T(1, 4);
    else if (height > 0.5) return heightmap_get_subtex_S_and_T(7, 1);
    else if (height > 0.3) return heightmap_get_subtex_S_and_T(4, 1);
    else return heightmap_get_subtex_S_and_T(1, 1);
}
