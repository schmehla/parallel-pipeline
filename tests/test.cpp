#include <stdlib.h>
#include <iostream>
#include <string>
#include <cassert>

#include "../implementation/parallel_pipeline.cpp"
#include "../implementation/tbb_pipeline.cpp"
#include "../implementation/naive_pipeline.cpp"

#define CALL \
{ \
    std::cout << "test failed!" << std::endl; \
    std::cout << "========================================" << std::endl; \
    std::cout << "with file: \"" << config_file << "\""<< std::endl; \
    std::cout << "concurrency parameter: " << i << std::endl; \
    std::cout << "batch_size: " << batch_size << std::endl; \
} 

std::string char_vec_to_string(std::vector<char> vec) {
    std::string str(vec.begin(), vec.end());
    return str; 
}

int main(int argn, char** argc) {
    // this is only correct when mapping two times
    const std::string correct_output = ":mf";

    for (std::string config_file : {"../tests/test1.pipeline", "../tests/test2.pipeline"}) {
        {
            std::cout << "testing   naive variant...";
            NaivePipeline<char> p(config_file);
            std::string pipeline_output = char_vec_to_string(p.run());
            if (pipeline_output.compare(correct_output)) {
                std::cout << "test failed!"  << std::endl;
                std::cout << "========================================" << std::endl;
                std::cout << "with file:" << config_file << std::endl;
                return 0;
            }
            std::cout << "works!" << std::endl;
        }
        for (std::size_t i = 1; i < 5; ++i) {
            for (std::size_t batch_size : {1, 2, 3}) {
                {
                    std::cout << "testing  static variant...";
                    ParallelPipeline<char> p(config_file, i);
                    std::string pipeline_output = char_vec_to_string(p.run(batch_size, ThreadPoolType::STATIC));
                    if (pipeline_output.compare(correct_output)) {
                        CALL;
                        return 0;
                    }
                    std::cout << "works!" << std::endl;
                }
                {
                    std::cout << "testing dynamic variant...";
                    ParallelPipeline<char> p(config_file, i);
                    std::string pipeline_output = char_vec_to_string(p.run(batch_size, ThreadPoolType::DYNAMIC));
                    if (pipeline_output.compare(correct_output)) {
                        CALL;
                        return 0;
                    }
                    std::cout << "works!" << std::endl;
                }
                {
                    std::cout << "testing     tbb variant...";
                    TBBPipeline<char> p(config_file, i);
                    std::string pipeline_output = char_vec_to_string(p.run(batch_size));
                    if (pipeline_output.compare(correct_output)) {
                        CALL;
                        return 0;
                    }
                    std::cout << "works!" << std::endl;
                }
            }
        }
    }
    std::cout << "========================================" << std::endl;
    std::cout << "All tests passed :)" << std::endl;
}