#pragma once

#include <vector>
#include <filesystem>

#include <GL/glew.h>

#include "vertex.hpp"


bool load_OBJ_GDrive(
	const std::filesystem::path& file_name,
	std::vector <vertex>& vertices,
	std::vector <GLuint>& indices
);


void load_OBJ_PG2(
	const std::filesystem::path& file_name,
	std::vector<vertex>& vertices,
	std::vector<GLuint>& indices
);


void load_heightmap(
	const std::filesystem::path& file_name,
	std::vector<vertex>& vertices,
	std::vector<GLuint>& indices,
	std::map<std::pair<float, float>, float>& heightmap_heights
);

// Heightmap helper methods
glm::vec2 heightmap_get_subtex_S_and_T(const int x, const int y);
glm::vec2 heightmap_get_subtex_by_height(float height);
