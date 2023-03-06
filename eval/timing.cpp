#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <string>

class Timing {
 protected:
    struct Printline {};

    const std::vector<std::size_t> INPUT_SIZES = {10000, 1000000}; 
    const std::vector<std::string> STAGES_LIST = {"100", "100 100", "100 100 100 100 100 100 100 100 100 100", "10 1000 10"};

    static const std::size_t NUM_ITER = 3;
    std::size_t iter;
    std::ofstream out;

    double time_diff(std::chrono::_V2::system_clock::time_point t0, std::chrono::_V2::system_clock::time_point t1) {
        return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()/1000.;
    }

    virtual void time_iter() {};

    virtual void print_line(Printline line) {};

    virtual void print_headline() {};

    std::chrono::_V2::system_clock::time_point now() {
        return std::chrono::high_resolution_clock::now();
    }
    
  public:
    Timing(std::string file)
    : out(file) {}

    void time_all() {
        print_headline();
        for (std::size_t it = 0; it < NUM_ITER; ++it) {
            iter = it;
            time_iter();
        }
    }
};