#include "App/app.hpp"

#include <fmt/core.h>

App app;

int main()
{    
    try {
        if (app.Init()) {
            app.Run();
        }
    }
    catch (std::exception const& e) {
        fmt::println("ERROR :: {}", e.what());
    }
}
