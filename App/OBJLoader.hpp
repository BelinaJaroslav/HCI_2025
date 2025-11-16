#pragma once

#include <vector>
#include <filesystem>

#include <GL/glew.h>

#include "Assets.hpp"

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
