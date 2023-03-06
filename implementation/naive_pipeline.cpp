#pragma once

#include "pipeline.hpp"


template<typename T>
class NaivePipeline : public Pipeline<T> {
 public:
    using Batch = typename Pipeline<T>::Batch;
    using Stage = typename Pipeline<T>::Stage;

    NaivePipeline(std::string config_loc)
    : Pipeline<T>(config_loc) {}

    std::vector<T> run() {
        // Handle entire input as a single batch and apply all stages one by one.
        Batch current = this->input;
        for (auto& s : this->stages) {
            current = s(current);
        }
        return current;
    }    

 private:
};