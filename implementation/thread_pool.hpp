#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

#include "buffer.hpp"

using namespace std::chrono_literals; 

enum ThreadPoolType {
    STATIC,
    DYNAMIC
};

template<class T>
class ThreadPool {
 public:
	using Job = std::function<T(const T&)>;

    ThreadPool(const Job& job, Buffer<T>& input_buffer, Buffer<T>& output_buffer, const std::function<bool(const T&)> is_last, std::size_t num_workers)
    : job(job)
    , input_buffer(input_buffer)
    , output_buffer(output_buffer)
    , is_last(is_last)
    , num_workers(num_workers)
    , done(false)
    , id_counter(0) {}

 protected:
    const Job& job;
    Buffer<T>& input_buffer;
    Buffer<T>& output_buffer;
    std::function<bool(const T&)> is_last;
    std::vector<std::thread> workers;
    std::size_t num_workers;

    std::atomic<bool> done;
    std::atomic<std::size_t> id_counter;
    std::atomic<std::size_t> output_ready_for;
    std::mutex pop_and_set_done;
    std::mutex output_and_incr_ready_for;


    void work(const std::atomic<bool>& external_shutdown) {
        while (!external_shutdown.load()) {
            T element;
            std::size_t id;
            {
                std::lock_guard<std::mutex> l(this->pop_and_set_done);
                if (this->done.load()) break;
                element = input_buffer.pop();
                id = this->id_counter.fetch_add(1);
                if (is_last(element)) {
                    this->done.store(true);
                }
            }
            T transformed = job(element);
            while (id != this->output_ready_for.load()) {};
            {
                std::lock_guard<std::mutex> l(this->output_and_incr_ready_for);
                output_buffer.push(std::move(transformed));
                this->output_ready_for.fetch_add(1);
            }
        }
        //if (external_shutdown.load()) std::cout << "thread killed" << std::endl;
    }
};

template<class T>
class StaticThreadPool : public ThreadPool<T> {
 public:
	using Job = typename ThreadPool<T>::Job;

    StaticThreadPool(const Job& job, Buffer<T>& input_buffer, Buffer<T>& output_buffer, const std::function<bool(const T&)> is_last, std::size_t num_workers) 
    : ThreadPool<T>(job, input_buffer, output_buffer, is_last, num_workers) {
        this->workers.reserve(num_workers);
        for (std::size_t i = 0; i < num_workers; ++i) {
            this->workers.emplace_back([this] {this->work(false);});
        }
        for (auto& w : this->workers) w.join();
    }
 private:
};

template<class T>
class DynamicThreadPool : public ThreadPool<T> {
 public:
	using Job = typename ThreadPool<T>::Job;

    DynamicThreadPool(Job job, Buffer<T>& input_buffer, Buffer<T>& output_buffer, std::function<bool(const T&)> is_last, std::size_t num_initial_workers) 
    : ThreadPool<T>(job, input_buffer, output_buffer, is_last, num_initial_workers) {
        const std::size_t upper_thread_bound = 2*num_initial_workers;
        const double upper_fill_level_bound = 0.9;
        const double lower_fill_level_bound = 0.9;
        this->workers.reserve(upper_thread_bound);
        active_workers = 0;
        for (std::size_t i = 0; i < num_initial_workers; ++i) {
            shutdown.emplace_back(false);
            this->workers.emplace_back([&] {this->work(shutdown.back());});
            ++active_workers;
        }
        while (!this->done.load()) {
            std::this_thread::sleep_for(100ms);
            if (active_workers <= upper_thread_bound
            && input_buffer.fill_level() > prev_input_fill_level
            && output_buffer.fill_level() < prev_output_fill_level) {
                //std::cout << "spawning new thread" << std::endl;
                shutdown.emplace_back(false);
                ++active_workers;
                this->workers.emplace_back([&] {this->work(shutdown.back());});
            } else if (active_workers > 1
            && input_buffer.fill_level() < prev_input_fill_level
            && output_buffer.fill_level() > prev_output_fill_level) {
                for (auto it = shutdown.begin(); it != shutdown.end(); ++it) {
                    if ((*it).load() == false) {
                        (*it).store(true);
                        break;
                    }
                }
                --active_workers;
                //std::cout << "killing thread.." << std::endl;
            }
            prev_input_fill_level = input_buffer.fill_level();
            prev_output_fill_level = output_buffer.fill_level();
        }
        for (auto& w : this->workers) w.join();
    }

 private:
    std::list<std::atomic<bool>> shutdown;
    std::size_t active_workers;
    double prev_input_fill_level;
    double prev_output_fill_level;
};