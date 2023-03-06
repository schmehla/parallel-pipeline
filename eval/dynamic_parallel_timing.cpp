#include <iostream>

#include "eval/parallel_timing.cpp"

int main(int argc, char* argv[]) {
    ParallelTiming dynamic_timing(ThreadPoolType::DYNAMIC, "dynamic_parallel_timing.txt");
    dynamic_timing.time_all();
    return 0;
}