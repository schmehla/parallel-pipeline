#include "../eval/timing.cpp"
#include "implementation/tbb_pipeline.cpp"

class TBBTiming : public Timing {
 private:
    struct Printline {
        std::size_t iter;
        std::size_t n_token;
        std::size_t batch_size;
        std::string stages;
        std::size_t input_size;
        double time;
    };

    std::size_t max_n_tokens = 12;
    std::vector<std::size_t> batch_sizes = {100, 1000, 10000};

    void time_iter() {
        for (std::size_t input_size : INPUT_SIZES) { // TODO change threadpool sizes
            for (std::size_t n_tokens = 1; n_tokens <= max_n_tokens; n_tokens+=2) {
                for (std::size_t batch_size : batch_sizes) {
                    for (std::string stages : STAGES_LIST) {
                        TBBPipeline<char> p("../data/pipelines/input_" + std::to_string(input_size) + "_stages_" + stages + ".pipeline", n_tokens);
                        auto t1 = now();
                        std::vector<char> output = p.run(batch_size);
                        auto t2 = now();
                        print_line({iter,
                                    n_tokens,
                                    batch_size,
                                    stages,
                                    input_size,
                                    time_diff(t1, t2)});
                    }
                } 
            }
        }
    }

    void print_line(Printline line) {
        out       << line.iter << "," 
                  << line.n_token << ","
                  << line.batch_size << ","
                  << line.stages << ","
                  << line.input_size << ","
                  << line.time << std::endl; 
        std::cout << line.iter << "," 
                  << line.n_token << ","
                  << line.batch_size << ","
                  << line.stages << ","
                  << line.input_size << ","
                  << line.time << std::endl; 
    }

    void print_headline() {
        std::string headline = "iter,n_tokens,batch_size,stages,input_size,time";
        out << headline << std::endl;
        std::cout << headline << std::endl;
    }
    
  public:
    TBBTiming()
    : Timing("tbb_timing.txt") {}
};

int main(int argc, char* argv[]) {
    TBBTiming timing;
    timing.time_all();
    return 0;
}