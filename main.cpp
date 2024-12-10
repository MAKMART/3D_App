#include "source/Application.h"

int main() {
    try {
        Application app("3D Model Viewer", 1920, 1080);
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
