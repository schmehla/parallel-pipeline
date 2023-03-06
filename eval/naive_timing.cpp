#include "../eval/timing.cpp"
#include "implementation/naive_pipeline.cpp"

class NaiveTiming : public Timing {
 private:
    struct Printline {
        std::size_t iter;
        std::string stages;
        std::size_t input_size;
        double time;
    };

    void time_iter() {
        for (std::size_t input_size : INPUT_SIZES) {
            for (std::string stages : STAGES_LIST) {
                NaivePipeline<char> p("../data/pipelines/input_" + std::to_string(input_size) + "_stages_" + stages + ".pipeline");
                auto t1 = now();
                std::vector<char> output = p.run();
                auto t2 = now();
                print_line({
                    iter,
                    stages,
                    input_size,
                    time_diff(t1, t2)
                });
            }
        }
    }

    void print_line(Printline line) {
        out       << line.iter << "," 
                  << line.stages << "," 
                  << line.input_size << "," 
                  << line.time << std::endl; 
        std::cout << line.iter << "," 
                  << line.stages << "," 
                  << line.input_size << "," 
                  << line.time << std::endl; 
    }

    void print_headline() {
        std::string headline = "iter,stages,input_size,time";
        out << headline << std::endl;
        std::cout << headline << std::endl;
    }
    
  public:
    NaiveTiming()
    : Timing("naive_timing.txt") {}
};

int main(int argc, char* argv[]) {
    NaiveTiming timing;
    timing.time_all();
    return 0;
}