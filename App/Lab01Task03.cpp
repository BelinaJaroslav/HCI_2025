#include "App.hpp"

#include <fmt/core.h>

void App::FindRedObjectInVideo() const
{
    fmt::println("== Lab 01 Task 03 ==");

	do {

		//TODO: Make "loop that grabs the image from a capture device until it is empty, calls find_object_***() and prints result, and test ESC key."
		// Measure time foreach frame, must be <10ms, resize the image so <10ms is possible

	} while (cv::pollKey() != 27); // Message loop until user presses ESC
}
