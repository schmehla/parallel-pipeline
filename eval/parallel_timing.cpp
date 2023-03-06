#include "../eval/timing.cpp"
#include "implementation/parallel_pipeline.cpp"

class ParallelTiming : public Timing {
 private:
    struct Printline {
        std::size_t iter;
        std::size_t threadpool_size;
        std::size_t batch_size;
        std::string stages;
        std::size_t input_size;
        double time;
    };

    ThreadPoolType threadpool_type;
    std::size_t max_threadpool_size = 6;
    std::vector<std::size_t> batch_sizes = {100, 1000, 10000};

    void time_iter() {
        for (std::size_t input_size : INPUT_SIZES) { // TODO change threadpool sizes
            for (std::size_t threadpool_size = 1; threadpool_size <= max_threadpool_size; ++threadpool_size) {
                for (std::size_t batch_size : batch_sizes) {
                    for (const std::string stages : STAGES_LIST) {
                        ParallelPipeline<char> p("../data/pipelines/input_" + std::to_string(input_size) + "_stages_" + stages + ".pipeline", threadpool_size);
                        auto t1 = now();
                        std::vector<char> output = p.run(batch_size, threadpool_type);
                        auto t2 = now();
                        print_line({iter,
                                    threadpool_size,
                                    batch_size,
                                    stages,
                                    input_size,
                                    time_diff(t1, t2)});
                    }
                } 
            }
        }
        std::cout << "==============================================================================" << std::endl;
    }

    void print_line(Printline line) {
        out       << line.iter << "," 
                  << line.threadpool_size << ","
                  << line.batch_size << ","
                  << line.stages << ","
                  << line.input_size << ","
                  << line.time << std::endl; 
        std::cout << line.iter << "," 
                  << line.threadpool_size << ","
                  << line.batch_size << ","
                  << line.stages << ","
                  << line.input_size << ","
                  << line.time << std::endl; 
    }

    void print_headline() {
        std::string headline = "iter,threadpool_size,batch_size,stages,input_size,time";
        out << headline << std::endl;
        std::cout << headline << std::endl;
    }
    
  public:
    ParallelTiming(ThreadPoolType threadpool_type, std::string out_file)
    : threadpool_type(threadpool_type)
    , Timing(out_file) {}
};
