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

// =============================================================================
// ================================== Defines ==================================
// =============================================================================

#define DELAY               750
#define INVALID_INDEX       -1
#define FILL_NAME           "Filling..."
#define SHUFFLE_NAME        "Shuffling..."
#define CHECK_NAME          "Checking..."
#define BUBBLE_SORT_NAME    "Bubble Sort"
#define COCKTAIL_SORT_NAME  "Cocktail Shaker Sort"
#define MERGE_SORT_NAME     "Merge Sort"
#define INSERTION_SORT_NAME "Insertion Sort"
#define SELECTION_SORT_NAME "Selection Sort"
#define QUICK_SORT_NAME     "Quick Sort"
#define RADIX_SORT_NAME     "Radix Sort"

namespace atn {

// =============================================================================
// =============================== Declarations ================================
// =============================================================================

class Algorithms {
  public:
    std::shared_ptr<std::condition_variable> cond;
    std::mutex mtx;
    size_t array_size;
    int seed;
    std::string name;
    std::vector<size_t> array;
    size_t comparisons;
    std::unordered_set<size_t> comparison_indicies;
    size_t swaps;
    int swap_index_1, swap_index_2;
    size_t writes_to_aux_array;
    Algorithms(const std::shared_ptr<std::condition_variable>& cond, size_t array_size);
    void main(std::vector<void (Algorithms::*)()> sorts);
    void bubble_sort();
    void cocktail_shaker_sort();
    void merge_sort();
    void insertion_sort();
    void selection_sort();
    void quick_sort();
  private:
    void wait();
    void swap(size_t index_1, size_t index_2);
    bool compare_gt(size_t index_1, size_t index_2);
    void write_to_aux_array(std::vector<size_t>& aux, size_t value);
    void write_from_aux_array(std::vector<size_t> aux, size_t start);
    void clear_swap_indicies();
    void clear_comp_indicies();
    void reset();
    void fill();
    void shuffle();
    bool check_sorted();
    void merge_sort(size_t left, size_t right);
    void merge(size_t start, size_t mid, size_t end);
    void quick_sort(int left, int right);
    int partition(int left, int right);
};

// =============================================================================
// ================================ Definitions ================================
// =============================================================================

// ============================== Public Members ===============================

Algorithms::Algorithms(const std::shared_ptr<std::condition_variable>& cond, size_t array_size)
        : cond(cond), mtx(), array_size(array_size), seed(time(NULL)), name(FILL_NAME), array(), comparison_indicies() {
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
        this->clear_comp_indicies();
        this->clear_swap_indicies();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        this->reset();
        assert(this->check_sorted());
        this->reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
    }
}

// https://en.wikipedia.org/wiki/Bubble_sort#Optimizing_bubble_sort
void Algorithms::bubble_sort() {
    #ifdef DEBUG
    std::cerr << "Starting sort" << std::endl;
    #endif
    this->name = BUBBLE_SORT_NAME;
    size_t n = this->array_size;
    do {
        size_t new_n = 0;
        for (size_t index = 0; index < n - 1; ++index) {
            if (compare_gt(index, index + 1)) {
                this->swap(index, index + 1);
                new_n = index + 1;
            }
        }
        n = new_n;
        this->clear_comp_indicies();
    } while (n > 1);
    this->clear_comp_indicies();
    this->clear_swap_indicies();
    #ifdef DEBUG
    std::cerr << "Ending sort" << std::endl;
    #endif
}

// https://en.wikipedia.org/wiki/Cocktail_shaker_sort#Pseudocode
void Algorithms::cocktail_shaker_sort() {
    this->name = COCKTAIL_SORT_NAME;
    int lower = 0, upper = this->array_size - 1;
    int new_lower, new_upper;
    while (lower < upper) {
        int new_lower = upper;
        int new_upper = lower;
        for (int index = lower; index <= upper - 1; ++index) {
            bool result = compare_gt(index, index + 1);
            if (result) {
                this->swap(index, index + 1);
                new_upper = index + 1;
            }
        }
        this->clear_comp_indicies();
        upper = new_upper - 1;
        for (int index = upper - 1; index >= lower; --index) {
            bool result = compare_gt(index, index + 1);
            if (result) {
                this->swap(index, index + 1);
                new_lower = index;
            }
        }
        this->clear_comp_indicies();
        lower = new_lower + 1;
    }
}

// https://www.geeksforgeeks.org/in-place-merge-sort/
void Algorithms::merge_sort() {
    this->name = MERGE_SORT_NAME;
    return this->merge_sort(0, this->array_size - 1);
}

// https://www.geeksforgeeks.org/insertion-sort/
void Algorithms::insertion_sort() {
    this->name = INSERTION_SORT_NAME;
    size_t i, j;
    for (i = 1; i < this->array_size; ++i) {
        for (int j = i - 1; j >= 0 && this->compare_gt(j, j + 1); --j) {
            this->swap(j, j + 1);
        }
    }
}

// https://www.geeksforgeeks.org/selection-sort/
void Algorithms::selection_sort() {
    this->name = SELECTION_SORT_NAME;
    size_t min_index;
    for (size_t i = 0; i < this->array_size - 1; ++i) {
        min_index = i;
        for (size_t j = i + 1; j < this->array_size; ++j) {
            if (compare_gt(min_index, j)) {
                min_index = j;
            }
            this->clear_comp_indicies();
        }
        this->swap(min_index, i);
    }
}

// https://en.wikipedia.org/wiki/Quicksort#Hoare_partition_scheme
void Algorithms::quick_sort() {
    this->name = QUICK_SORT_NAME;
    return this->quick_sort(0, this->array_size - 1);
}

// ============================== Private Members =============================

void Algorithms::wait() {
    std::unique_lock<std::mutex> lock(this->mtx);
    this->cond->wait(lock);
}

void Algorithms::swap(size_t index_1, size_t index_2) {
    #ifdef DEBUG
    std::cerr << "Starting swap" << std::endl;
    #endif
    this->swap_index_1 = index_1;
    this->swap_index_2 = index_2;
    this->swaps++;
    size_t temp = this->array[index_1];
    this->array[index_1] = this->array[index_2];
    this->array[index_2] = temp;
    this->wait();
    this->clear_swap_indicies();
    this->clear_comp_indicies();
    #ifdef DEBUG
    std::cerr << "Ending swap" << std::endl;
    #endif
}

bool Algorithms::compare_gt(size_t index_1, size_t index_2) {
    #ifdef DEBUG
    std::cerr << "Starting compare_gt" << std::endl;
    #endif
    this->comparison_indicies.insert(index_1);
    this->comparison_indicies.insert(index_2);
    this->comparisons++;
    this->wait();
    #ifdef DEBUG
    std::cerr << "Ending compare_gt" << std::endl;
    #endif
    return this->array[index_1] > this->array[index_2];
}

void Algorithms::write_to_aux_array(std::vector<size_t>& aux, size_t index) {
    aux.push_back(this->array[index]);
    this->swap_index_1 = index;
    this->swaps++;
    this->writes_to_aux_array++;
    this->wait();
    this->clear_swap_indicies();
    this->clear_comp_indicies();
}

void Algorithms::write_from_aux_array(std::vector<size_t> aux, size_t start) {
    for (size_t i = 0; i < aux.size(); ++i) {
        this->array[start + i] = aux[i];
        this->swap_index_1 = start + i;
        this->swaps++;
        this->wait();
        this->clear_swap_indicies();
        this->clear_comp_indicies();
    }
}

void Algorithms::clear_swap_indicies() {
    #ifdef DEBUG
    std::cerr << "Starting clear_swap_indicies" << std::endl;
    #endif
    this->swap_index_1 = INVALID_INDEX;
    this->swap_index_2 = INVALID_INDEX;
    #ifdef DEBUG
    std::cerr << "Ending clear_swap_indicies" << std::endl;
    #endif
}

void Algorithms::clear_comp_indicies() {
    #ifdef DEBUG
    std::cerr << "Starting clear_comp_indicies" << std::endl;
    #endif
    this->comparison_indicies.clear();
    #ifdef DEBUG
    std::cerr << "Ending clear_comp_indicies" << std::endl;
    #endif
}

void Algorithms::reset() {
    #ifdef DEBUG
    std::cerr << "Starting reset" << std::endl;
    #endif
    srand(this->seed);
    this->comparisons = 0;
    this->swaps = 0;
    this->writes_to_aux_array = 0;
    this->clear_comp_indicies();
    this->clear_swap_indicies();
    #ifdef DEBUG
    std::cerr << "Ending reset" << std::endl;
    #endif
}

void Algorithms::fill() {
    #ifdef DEBUG
    std::cerr << "Starting fill" << std::endl;
    #endif
    this->array = std::vector<size_t>(this->array_size, 0);
    for (size_t i = 0 ; i < this->array_size; ++i) {
        this->array[i] = i + 1;
        this->wait();
    }
    #ifdef DEBUG
    std::cerr << "Ending fill" << std::endl;
    #endif
}

void Algorithms::shuffle() {
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

bool Algorithms::check_sorted() {
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

void Algorithms::merge_sort(size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + ((right - left) >> 1);
        this->merge_sort(left, mid);
        this->merge_sort(mid + 1, right);
        this->merge(left, mid, right);
    }
}

void Algorithms::merge(size_t start, size_t mid, size_t end) {
    size_t i = start, j = mid + 1;
    if (!this->compare_gt(mid, j)) return;
    std::vector<size_t> aux;
    while (i <= mid && j <= end) {
        this->clear_comp_indicies();
        if (this->compare_gt(i, j)) {
            this->write_to_aux_array(aux, j);
            j++;
        } else {
            this->write_to_aux_array(aux, i);
            i++;
        }
    }
    while (i <= mid) {
        this->write_to_aux_array(aux, i);
        i++;
    }
    while (j <= end) {
        this->write_to_aux_array(aux, j);
        j++;
    }
    this->write_from_aux_array(aux, start);
}

void Algorithms::quick_sort(int left, int right) {
    if (left < 0 || right < 0 || left >= right) return;
    this->compare_gt(left, right);
    int p = this->partition(left, right);
    this->quick_sort(left, p);
    this->quick_sort(p + 1, right);
}

int Algorithms::partition(int left, int right) {
    int pivot_index = (left + right) / 2;
    int pivot = this->array[pivot_index];
    int i = left - 1;
    int j = right + 1;
    while (true) {
        do {
            i++;
            this->compare_gt(pivot_index, i);
        } while (pivot > this->array[i]);
        do {
            j--;
            this->compare_gt(pivot_index, j);
        } while (this->array[j] > pivot);
        if (i >= j) return j;
        this->swap(i, j);
    }
}

} // End namespace atn

#endif // _SRC_ALGORITHMS_HPP_