#include <iostream>

#include "eval/parallel_timing.cpp"

int main(int argc, char* argv[]) {
    ParallelTiming static_timing(ThreadPoolType::STATIC, "static_parallel_timing.txt");
    static_timing.time_all();
    return 0;
}