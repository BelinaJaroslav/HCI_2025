#include "App/app.hpp"

App app;

int main()
{
    if (app.Init()) {
        app.Run();
    }
}
