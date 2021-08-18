#ifndef _SRC_ALGORITHMS_HPP_
#define _SRC_ALGORITHMS_HPP_

#include <assert.h>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
#ifdef DEBUG
#include <iostream>
#endif

#define DELAY               500
#define INVALID_INDEX       -1
#define FILL_NAME           "Filling..."
#define SHUFFLE_NAME        "Shuffling..."
#define CHECK_NAME          "Checking..."
#define BUBBLE_SORT_NAME    "Bubble Sort"
#define COCKTAIL_SORT_NAME  "Cocktail Shaker Sort"
#define RADIX_SORT_NAME     "Radix Sort"
#define MERGE_SORT_NAME     "Merge Sort"
#define SELECTION_SORT_NAME "Selection Sort"
#define INSERTION_SORT_NAME "Insertion Sort"

namespace atn {

class Algorithms {
  public:
    std::shared_ptr<std::condition_variable> cond;
    std::mutex mtx;
    size_t array_size;
    int seed = time(NULL);
    std::string name;
    std::vector<size_t> array;
    size_t comparisons;
    std::unordered_set<size_t> comparison_indicies;
    size_t swaps;
    int swap_index_1, swap_index_2;
    Algorithms(const std::shared_ptr<std::condition_variable>& cond, size_t array_size);
    void main(std::vector<void (Algorithms::*)()> sorts);
    void bubble_sort();
  private:
    void swap(size_t index_1, size_t index_2) {
        #ifdef DEBUG
        std::cerr << "Starting swap" << std::endl;
        #endif
        this->swap_index_1 = index_1;
        this->swap_index_2 = index_2;
        this->swaps++;
        size_t temp = this->array[index_1];
        this->array[index_1] = this->array[index_2];
        this->array[index_2] = temp;
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            this->cond->wait(lock);
        }
        this->clear_swap_indicies();
        this->clear_comp_indicies();
        #ifdef DEBUG
        std::cerr << "Ending swap" << std::endl;
        #endif
    }
    bool compare_gt(size_t index_1, size_t index_2) {
        #ifdef DEBUG
        std::cerr << "Starting compare_gt" << std::endl;
        #endif
        this->comparison_indicies.insert(index_1);
        this->comparison_indicies.insert(index_2);
        this->comparisons++;
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            this->cond->wait(lock);
        }
        #ifdef DEBUG
        std::cerr << "Ending compare_gt" << std::endl;
        #endif
        return this->array[index_1] > this->array[index_2];
    }
    void clear_swap_indicies() {
        #ifdef DEBUG
        std::cerr << "Starting clear_swap_indicies" << std::endl;
        #endif
        this->swap_index_1 = INVALID_INDEX;
        this->swap_index_2 = INVALID_INDEX;
        #ifdef DEBUG
        std::cerr << "Ending clear_swap_indicies" << std::endl;
        #endif
    }
    void clear_comp_indicies() {
        #ifdef DEBUG
        std::cerr << "Starting clear_comp_indicies" << std::endl;
        #endif
        this->comparison_indicies.clear();
        #ifdef DEBUG
        std::cerr << "Ending clear_comp_indicies" << std::endl;
        #endif
    }
    void reset() {
        #ifdef DEBUG
        std::cerr << "Starting reset" << std::endl;
        #endif
        srand(this->seed);
        this->comparisons = 0;
        this->swaps = 0;
        this->clear_comp_indicies();
        this->clear_swap_indicies();
        #ifdef DEBUG
        std::cerr << "Ending reset" << std::endl;
        #endif
    }
    void fill() {
        #ifdef DEBUG
        std::cerr << "Starting fill" << std::endl;
        #endif
        this->array = std::vector<size_t>(this->array_size, 0);
        for (size_t i = 0 ; i < this->array_size; ++i) {
            this->array[i] = i + 1;
            {
                std::unique_lock<std::mutex> lock(this->mtx);
                this->cond->wait(lock);
            }
        }
        #ifdef DEBUG
        std::cerr << "Ending fill" << std::endl;
        #endif
    }
    void shuffle() {
        #ifdef DEBUG
        std::cerr << "Starting shuffle" << std::endl;
        #endif
        this->name = SHUFFLE_NAME;
        for (size_t i = this->array_size - 1; i != size_t(-1); --i) {
            size_t rand_index = rand() % (i + 1);
            this->swap(i, rand_index);
        }
        #ifdef DEBUG
        std::cerr << "Ending shuffle" << std::endl;
        #endif
    }
    bool check_sorted() {
        #ifdef DEBUG
        std::cerr << "Starting check_sorted" << std::endl;
        #endif
        this->name = CHECK_NAME;
        for (size_t i = 0; i < this->array_size - 1; ++i) {
            bool result = this->compare_gt(i + 1, i);
            if (!result) return false;
        }
        #ifdef DEBUG
        std::cerr << "Ending check_sorted" << std::endl;
        #endif
    }
};

Algorithms::Algorithms(const std::shared_ptr<std::condition_variable>& cond,
        size_t array_size)
        : cond(cond), mtx(), array_size(array_size), seed(time(NULL)),
        name(FILL_NAME), array(), comparison_indicies() {
    #ifdef DEBUG
    std::cerr << "Starting Algorithm constructor" << std::endl;
    #endif
    this->reset();
    #ifdef DEBUG
    std::cerr << "Ending Algorithm constructor" << std::endl;
    #endif
}

void Algorithms::main(std::vector<void (Algorithms::*)()> sorts) {
    this->fill();
    std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    for (size_t i = 0; i < sorts.size(); ++i) {
        this->shuffle();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        this->reset();
        (this->*sorts[i])();
        this->reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        assert(this->check_sorted());
        this->reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
}

void Algorithms::bubble_sort() {
    #ifdef DEBUG
    std::cerr << "Starting sort" << std::endl;
    #endif
    this->name = BUBBLE_SORT_NAME;
    for (size_t iteration = 0; iteration < this->array_size; ++iteration) {
        for (size_t index = 0; index < this->array_size - iteration - 1; ++index) {
            bool result = compare_gt(index, index + 1);
            if (result) {
                this->swap(index, index + 1);
            }
        }
    }
    this->clear_comp_indicies();
    this->clear_swap_indicies();
    #ifdef DEBUG
    std::cerr << "Ending sort" << std::endl;
    #endif
}

}

#endif