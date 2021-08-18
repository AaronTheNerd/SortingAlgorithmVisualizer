#ifndef _SRC_VISUALIZER_DRAWING_AREA_HPP_
#define _SRC_VISUALIZER_DRAWING_AREA_HPP_

#include "../src/algorithms.hpp"

#include <condition_variable>
#include <thread>
#include <gtkmm.h>
#ifdef DEBUG
#include <iostream>
#endif

namespace atn {

#define SEPARATION      0
#define TEXT_OFFSET     10
#define FONT_SIZE       24
#define ARRAY_SIZE      100
#define REFRESH_RATE    10

// http://transit.iut2.upmf-grenoble.fr/doc/gtkmm-3.0/tutorial/html/sec-drawing-text.html
class VisualizerDrawingArea : public Gtk::DrawingArea {
  public:
    std::shared_ptr<std::condition_variable> cond;
    std::mutex mtx;
    atn::Algorithms algos;
    std::thread t;
    VisualizerDrawingArea();
    virtual ~VisualizerDrawingArea();
    void init(const int width, const int height);
  protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  private:
    float _bar_scale;
    float _bar_width;
    bool update();
    void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height, size_t index);
    void draw_stats(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
    void draw_array(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
    void draw_special_indicies(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height);
};

VisualizerDrawingArea::VisualizerDrawingArea(): cond(std::make_shared<std::condition_variable>()), algos(cond, ARRAY_SIZE) {
    #ifdef DEBUG
    std::cerr << "Starting VisualizerDrawingArea constructor" << std::endl;
    #endif
    Glib::signal_timeout().connect(sigc::mem_fun(*this, &VisualizerDrawingArea::update), REFRESH_RATE);
    auto function = &Algorithms::bubble_sort;
    std::vector<void (Algorithms::*)()> sorts = {&Algorithms::bubble_sort};
    #ifdef DEBUG
    std::cout << "FUNCTION TYPE: " << typeid(function).name() << std::endl;
    #endif
    t = std::thread(&Algorithms::main, &this->algos, sorts);
    #ifdef DEBUG
    std::cerr << "Ending VisualizerDrawingArea constructor" << std::endl;
    #endif
}

VisualizerDrawingArea::~VisualizerDrawingArea() {}

void VisualizerDrawingArea::init(const int width, const int height) {
    this->_bar_width = 1.0f * (width - ((this->algos.array_size - 1) * SEPARATION)) / this->algos.array_size;
    this->_bar_scale = (1.0f * height - (3 * FONT_SIZE) - TEXT_OFFSET) / this->algos.array_size;
    #ifdef DEBUG
    std::cerr << "Init Results: w: " << this->_bar_width << ", s: " << this->_bar_scale << std::endl;
    #endif
}

bool VisualizerDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    #ifdef DEBUG
    std::cerr << "Starting on_draw" << std::endl;
    #endif
    Gtk::Allocation allocation = this->get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    // Clear screen
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->rectangle(0, 0, width, height);
    cr->fill();
    // Init
    this->init(width, height);
    // Draw stats
    this->draw_stats(cr, width, height);
    // Draw array
    this->draw_array(cr, width, height);
    // Draw swap/compare indicies
    this->draw_special_indicies(cr, width, height);
    #ifdef DEBUG
    std::cerr << "Ending on_draw" << std::endl;
    #endif
    return true;
}

bool VisualizerDrawingArea::update() {
    #ifdef DEBUG
    std::cerr << "Starting update" << std::endl;
    #endif
    auto window = this->get_window();
    if (window) {
        this->cond->notify_all();
        Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
        window->invalidate_rect(r, false);
    }
    #ifdef DEBUG
    std::cerr << "Ending update" << std::endl;
    #endif
    return true;
}

void VisualizerDrawingArea::draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height, size_t index) {
    cr->rectangle(index * (SEPARATION + this->_bar_width), 
            height - this->algos.array[index] * this->_bar_scale,
            this->_bar_width,
            this->algos.array[index] * this->_bar_scale);
    cr->fill();
}

void VisualizerDrawingArea::draw_stats(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height) {
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->set_font_size(FONT_SIZE);
    cr->select_font_face("monospace", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    cr->save();
    cr->move_to(TEXT_OFFSET, TEXT_OFFSET + FONT_SIZE);
    cr->show_text(this->algos.name);
    cr->restore();
    cr->save();
    cr->move_to(FONT_SIZE * 20 + TEXT_OFFSET, TEXT_OFFSET + FONT_SIZE);
    cr->show_text(std::string("Array Size: ") + std::to_string(this->algos.array_size));
    cr->restore();
    cr->save();
    cr->move_to(TEXT_OFFSET, TEXT_OFFSET + 2 * FONT_SIZE);
    cr->show_text(std::string("Comparisons: ") + std::to_string(this->algos.comparisons));
    cr->restore();
    cr->save();
    cr->move_to(TEXT_OFFSET, TEXT_OFFSET + 3 * FONT_SIZE);
    cr->show_text(std::string("Swaps: ") + std::to_string(this->algos.swaps));
    cr->restore();
}

void VisualizerDrawingArea::draw_array(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height) {
    cr->set_source_rgb(1.0, 1.0, 1.0);
    for (size_t i = 0; i < this->algos.array_size; ++i) {
        this->draw_rectangle(cr, width, height, i);
    }
}

void VisualizerDrawingArea::draw_special_indicies(const Cairo::RefPtr<Cairo::Context>& cr, const int width, const int height) {
    cr->set_source_rgb(0.26, 0.96, 0.26);
    for (auto it = this->algos.comparison_indicies.begin(); it != this->algos.comparison_indicies.end(); ++it) {
        this->draw_rectangle(cr, width, height, *it);
    }
    cr->set_source_rgb(1.0, 0.36, 0.30);
    if (this->algos.swap_index_1 != INVALID_INDEX) {
        this->draw_rectangle(cr, width, height, this->algos.swap_index_1);
    }
    if (this->algos.swap_index_2 != INVALID_INDEX) {
        this->draw_rectangle(cr, width, height, this->algos.swap_index_2);
    }
}

} // End namespace atn

#endif // _SRC_VISUALIZER_DRAWING_AREA_HPP_