# Freestyle Assignment

## Topic: Parallel Pipeline

A parallel pipeline transforms data batch-wise in multliple stages / filters.
A filter is a function which transforms input data batches to output data batches.
If the nature of the problem allows it, the filters can be executed in parallel.



### (1) Problem Instance:
- Think of a problem instance which can be solved by using a (parallel) pipline.
- This can be: Reading from a text file, applying a few character transformations (one transformation per stage) and writing back to a new file.
- Find a way to scale the work so that scheduling effects can be observed. Maybe apply a character transformation n times in a single filter.
- Write a programm to generate these instances with a given work factor (Alternatively the work factor can be set in the pipeline stage)
- The instances should have asynchronous workload, meaning that working on a batch takes unequal amounts of time per stage and in different stages.
- Generate some instances with meaningful size, those should later be used for testing and evaluation.


### (2) Intel TBB implementation:
- Use the Intel TBB Parallel Pipeline to solve the problem instance.


### (3) Tests: 
- Write some simple tests to test all upcoming implementations.


### (4) Naive Algorithm:
- Write a completely sequential algorithm that solves the problem instance.
- Use a single thread for the entire computation (no overhead for parallelization!).


### (5) Thread Pool:
- Implement a thread pool.
- The thread pool is handed tasks one by one and should run these tasks on the reserved threads.
- Implement a static scheduler (fixed amount of threads) and a dynamic scheduler (amount of threads is adapted to the workload).


### (6) Parallel Filters:
- Implement a pipeline which consists of multliple filters / stages.
- Use the previously implemented thread pool for each stage.
- Filters which can be executed in parallel should be executed in parallel.
- There should be buffers between the pipline filters. 
- The access to the buffers needs to be thread-safe.
- Each filter pulls data from the input buffer, applies the filter and writes it to the output buffer.
- For simplicy's sake, the buffers should always maintain the original order.
- Think of a communication / synchronization model so that a thread knows if it worked on the last batch and does not wait infinitely if nothing is pushed to the input buffer anymore. 
- Try different scheduling strategies (the desired outcome is that less-work-intensive stages use less threads when using dynamic scheduling). 


### (7) Evaluation:
- Use a varying number of threads in the parallel filters.
- Measure time for each stage of the pipeline.
- Compare the measurements the Intel TBB, naive and parallel filters variant.
- Also try to increase the pipeline length and find out how the runtime scales in comparison to the naive variant.
- (Expectation: As long as the pipeline throughput stays consistent, the runtime almost does not increase. In contrast, the naive variant should scale linearly with the length.)
- Find out how much of a bottleneck sequential filters pose (where data cannot be transformed in parallel).
