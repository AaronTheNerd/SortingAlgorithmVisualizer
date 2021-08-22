#ifndef _SRC_VISUALIZER_WINDOW_HPP_
#define _SRC_VISUALIZER_WINDOW_HPP_

#include <gtkmm.h>
#ifdef DEBUG
#include <iostream>
#endif
#include "../src/visualizer_drawing_area.hpp"

#define RATIO 5

namespace atn {

class VisualizerWindow : public Gtk::Window {
  private:
    Gtk::Grid layout;
    VisualizerDrawingArea drawing_area;
    Gtk::Box controls;
  public:
    VisualizerWindow() : layout(), controls(), drawing_area() {
        #ifdef DEBUG
        std::cerr << "Starting VisualizerWindow constructor" << std::endl;
        #endif
        this->set_title("Sorting Algorithm Visualizer");
        this->layout.set_row_homogeneous(true);
        this->layout.attach(this->drawing_area, 0, 0, RATIO, 1);
        this->drawing_area.set_hexpand(true);
        this->layout.attach(this->controls, RATIO, 0, 1, 1);
        this->controls.set_hexpand(true);
        this->add(this->layout);
        this->maximize();
        this->layout.show();
        this->drawing_area.show();
        this->controls.show();
        #ifdef DEBUG
        std::cerr << "Ending VisualizerWindow constructor" << std::endl;
        #endif
    }
    virtual ~VisualizerWindow() {}
};

} // End namespace atn

#endif // _SRC_VISUALIZER_WINDOW_HPP_