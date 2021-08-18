#include "../src/algorithms.hpp"
#include "../src/visualizer_window.hpp"
#ifdef DEBUG
#include <iostream>
#endif

int main(int argc, char** argv) {
    #ifdef DEBUG
    std::cerr << "Starting main" << std::endl;
    #endif
    auto app = Gtk::Application::create(argc, argv, "atn.sortingvisualizer");
    atn::VisualizerWindow window;
    return app->run(window);
}