#pragma once

#include <thread>
#include <iostream>

#include "pipeline.hpp"
#include "thread_pool.hpp"
#include "buffer.hpp"

template<typename T>
class ParallelPipeline : public Pipeline<T> {
 public:
    using Batch = typename Pipeline<T>::Batch;
    using Stage = typename Pipeline<T>::Stage;

    ParallelPipeline(std::string config_loc, std::size_t threadpool_size)
    : Pipeline<T>(config_loc)
    , threadpool_size(threadpool_size) {
        if (threadpool_size <= 0) {
            std::runtime_error("cannot have threadpool_size <= 0");
        }
    }

    std::vector<T> run(std::size_t batch_size, ThreadPoolType threadpool_type) {
        // Build buffers.
        std::size_t num_stages = this->stages.size();
        std::vector<Buffer<Batch>> buffers;
        buffers.reserve(num_stages+1);
        for (std::size_t i = 0; i <= num_stages; ++i) {
            buffers.emplace_back(100);
        }
        Buffer<Batch>& input_buffer = buffers.front();
        Buffer<Batch>& output_buffer = buffers.back();

        auto is_last = [](const Batch& b) { return b.size() == 0; };

        // Start thread pools.
        std::vector<std::thread> thread_pool_managers;
        for (std::size_t i = 0; i < this->stages.size(); ++i) {
            thread_pool_managers.emplace_back([i, &buffers, &is_last, &threadpool_type, *this] {
                switch (threadpool_type) {
                    case STATIC: 
                        StaticThreadPool<Batch>(
                            this->stages[i],
                            buffers[i],
                            buffers[i+1],
                            is_last,
                            threadpool_size
                        );
                        break;
                    case DYNAMIC:
                        DynamicThreadPool<Batch>(
                            this->stages[i],
                            buffers[i],
                            buffers[i+1],
                            is_last,
                            threadpool_size
                        );
                        break;
                }
            });
        }

        // Start thread to extract batches from output buffer.
        // The inserting thread cannot do this job as the pipeline
        // might be at full capacity => Deadlock, as no one can extract.
        std::vector<T> output;
        output.reserve(this->input.size());
        std::thread output_extractor([&] {
            Batch out_batch = output_buffer.pop();
            while (!is_last(out_batch)) {
                output.insert(output.end(), out_batch.begin(), out_batch.end());
                out_batch = output_buffer.pop();
            }
        });

        // Split input into batches and insert into pipeline.
        auto it = this->input.begin();
        for (; it < this->input.end()-batch_size; it += batch_size) {
            Batch batch(it, it+batch_size);
            input_buffer.push(std::move(batch));
        }
        Batch batch(it, this->input.end());
        input_buffer.push(std::move(batch));
        // Input dummy to detect last batch.
        // This element should not change when being mapped.
        Batch dummy;
        input_buffer.push(std::move(dummy));
        // Input dummy to detect last batch.


        // Join thread pool manager threads and output extractor.
        for (auto& tpm : thread_pool_managers) tpm.join();
        output_extractor.join();

        return output;
    }    

 private:
    std::size_t threadpool_size;
};