#include "App/App.hpp"

#include <fmt/core.h>

App app;

int main()
{    
    try {
        if (app.init()) {
            app.run();
        }
    }
    catch (std::exception const& e) {
        fmt::println("ERROR :: {}", e.what());
    }
}
