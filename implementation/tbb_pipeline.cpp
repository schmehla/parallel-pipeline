#pragma once

#include <thread>
#include <iostream>

#include "pipeline.hpp"
#include "oneapi/tbb/parallel_pipeline.h"
#include "oneapi/tbb/tick_count.h"
#include "oneapi/tbb/tbb_allocator.h"
#include "oneapi/tbb/global_control.h"

/**
 * 
 * // build tbb pipeline instead
*/

template<typename T>
class TBBPipeline : public Pipeline<T> {
 public:
    using Batch = typename Pipeline<T>::Batch;
    using Stage = typename Pipeline<T>::Stage;

    TBBPipeline(std::string config_loc, std::size_t n_tokens)
    : Pipeline<T>(config_loc)
    , n_tokens(n_tokens)
    , input_iter(this->input.begin()) {
        if (n_tokens <= 0) {
            std::runtime_error("cannot have n_tokens <= 0");
        }
        output.reserve(this->input.size());
    }

    std::vector<T> run(std::size_t batch_size) {
        this->batch_size = batch_size;
        auto input_filter = oneapi::tbb::make_filter(oneapi::tbb::filter_mode::serial_in_order, [this](oneapi::tbb::flow_control& fc) -> Batch {
            return this->get_input_batch(fc);
        });
        std::vector<oneapi::tbb::filter<Batch, Batch>> filters;
        filters.reserve(this->stages.size());
        for (auto& stage : this->stages) {
            filters.emplace_back(oneapi::tbb::filter_mode::parallel, stage);
        }
        oneapi::tbb::filter<std::vector<char>, std::vector<char>> f = filters[0];
        for (std::size_t i = 1; i < filters.size(); ++i) {
            f = f & filters[i];
        }
        oneapi::tbb::filter<std::vector<char>, void> output_filter(oneapi::tbb::filter_mode::serial_in_order, [this](Batch&& out_batch){
            write_output_batch(out_batch);
        });
        oneapi::tbb::parallel_pipeline(n_tokens, input_filter & f & output_filter);
        return output;
    }    

 private:
    std::size_t n_tokens;
    std::vector<T> output;
    std::vector<T>::iterator input_iter;
    std::size_t batch_size;

    Batch get_input_batch(oneapi::tbb::flow_control& fc) {
        if (input_iter < this->input.end()-batch_size) {
            Batch batch(input_iter, input_iter+batch_size);
            input_iter += batch_size;
            return batch;
        } else if (input_iter >= this->input.end()) {
            fc.stop();
            return Batch();
        } else {
            Batch batch(input_iter, this->input.end());
            input_iter += batch_size;
            return batch;
        }
    }

    void write_output_batch(Batch& out_batch) {
        output.insert(output.end(), out_batch.begin(), out_batch.end());
    }
};