#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <GL/glew.h> 
#include <glm/glm.hpp>

#include <fmt/core.h>

#include "OBJLoader.hpp"

constexpr auto MAX_LINE_SIZE = 1024;

bool load_OBJ_GDrive(const std::filesystem::path& file_name, std::vector<vertex>& vertices, std::vector<GLuint>& indices)
{
    fmt::println("Loading OBJ file {} ...", file_name.string());

	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	vertices.clear();
	indices.clear();

	FILE * file = nullptr;
	fopen_s(&file, file_name.string().c_str(), "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char line_header[MAX_LINE_SIZE];
		int res = fscanf_s(file, "%s", line_header, MAX_LINE_SIZE);
		if (res == EOF) {
			break;
		}

		if (strcmp(line_header, "v") == 0) {
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(line_header, "vt") == 0) {
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(line_header, "vn") == 0) {
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(line_header, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertex_index[3], uv_index[3], normal_index[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertex_index[0], &uv_index[0], &normal_index[0], &vertex_index[1], &uv_index[1], &normal_index[1], &vertex_index[2], &uv_index[2], &normal_index[2]);
			if (matches != 9) {
				printf("File can't be read by simple parser :( Try exporting with other options\n");
				return false;
			}

			for (int i = 0; i < 3; i++) {
				GLuint current_index;
				vertex current_vertex;
				current_vertex.position = temp_vertices[vertex_index[i]-1]; // OBJ array start from 1
				current_vertex.normal = temp_normals[normal_index[i]-1];
				current_vertex.texture_coordinates = temp_uvs[uv_index[i]-1];

                // avoid duplicit vertices
				auto t = std::find_if(vertices.begin(),
					vertices.end(),
					[&current_vertex]
					(const vertex& v2) -> bool {
						return (current_vertex == v2);
					});
                    
				if (t == vertices.end()) {
					vertices.push_back(current_vertex);
					current_index = GLuint(vertices.size() - 1);
				}
				else {
					current_index = GLuint(t - vertices.begin());
				}
				indices.push_back(current_index);
			}

		}
	}
	
    fmt::println("Done loading OBJ file {}", file_name.string());

	fclose(file);
	return true;
}


void load_OBJ_PG2(const std::filesystem::path& file_name, std::vector<vertex>& mesh_vertices, std::vector<GLuint>& mesh_vertex_indices)
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
                (void)sscanf_s(line.c_str(), "v %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertices.push_back(vertex_or_normal);
            }
            // vt 0.5000 0.7500
            else if (first_three_chars == "vt ") {
                uv = {};
                (void)sscanf_s(line.c_str(), "vt %f %f", &uv.x, &uv.y);
                uv.y = -uv.y; // DDS textures are inverted
                texture_coordinates.push_back(uv);
            }
            // vn 0.7235898972 -0.6894102097 -0.03363365307
            else if (first_three_chars == "vn ") {
                vertex_or_normal = {};
                (void)sscanf_s(line.c_str(), "vn %f %f %f", &vertex_or_normal.x, &vertex_or_normal.y, &vertex_or_normal.z);
                vertex_normals.push_back(vertex_or_normal);
            }
            else if (first_two_chars == "f ") {
                auto n = std::count(line.begin(), line.end(), '/');
                // f 1 2 3
                if (n == 0) {
                    unsigned int indices_temp[3]{};
                    (void)sscanf_s(line.c_str(), "f %d %d %d", &indices_temp[0], &indices_temp[1], &indices_temp[2]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                }
                // f 3/1 4/2 5/3
                else if (n == 3) {
                    unsigned int indices_temp[6]{};
                    (void)sscanf_s(line.c_str(), "f %d/%d %d/%d %d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                    indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                    indices_texture_coordinate.insert(indices_texture_coordinate.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                }
                else if (n == 6) {
                    // f 7//1 8//2 9//3
                    if (line.find("//") != std::string::npos) {
                        unsigned int indices_temp[6]{};
                        (void)sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d", &indices_temp[0], &indices_temp[3], &indices_temp[1], &indices_temp[4], &indices_temp[2], &indices_temp[5]);
                        indices_vertex.insert(indices_vertex.end(), { indices_temp[0], indices_temp[1], indices_temp[2] });
                        indices_vertex_normal.insert(indices_vertex_normal.end(), { indices_temp[3], indices_temp[4], indices_temp[5] });
                    }
                    // f 6/4/1 3/5/3 7/6/5
                    else {
                        unsigned int indices_temp[9]{};
                        (void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &indices_temp[0], &indices_temp[3], &indices_temp[6], &indices_temp[1], &indices_temp[4], &indices_temp[7], &indices_temp[2], &indices_temp[5], &indices_temp[8]);
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
                    (void)sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2], &v[3], &vt[3], &vn[3]);
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
                fmt::println("load_OBJ_PG2: Ignoring line '{}' in file '{}'", line, file_name.string());
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

    ///* Uncomment these if you don't like to live dangerously
    auto n_direct_uvs = texture_coordinates_direct.size();
    auto n_direct_normals = vertex_normals_direct.size();
    /**/

    // vectors to Vertex vector
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
