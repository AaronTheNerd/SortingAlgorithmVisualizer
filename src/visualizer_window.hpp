#ifndef _SRC_VISUALIZER_WINDOW_HPP_
#define _SRC_VISUALIZER_WINDOW_HPP_

#include <gtkmm.h>
#ifdef DEBUG
#include <iostream>
#endif
#include "../src/visualizer_drawing_area.hpp"

namespace atn {

class VisualizerWindow : public Gtk::Window {
  private:
    VisualizerDrawingArea drawing_area;
  public:
    VisualizerWindow() : drawing_area() {
        #ifdef DEBUG
        std::cerr << "Starting VisualizerWindow constructor" << std::endl;
        #endif
        this->set_title("Sorting Algorithm Visualizer");
        this->add(this->drawing_area);
        this->drawing_area.show();
        this->maximize();
        #ifdef DEBUG
        std::cerr << "Ending VisualizerWindow constructor" << std::endl;
        #endif
    }
    virtual ~VisualizerWindow() {}
};

} // End namespace atn

#endif // _SRC_VISUALIZER_WINDOW_HPP_