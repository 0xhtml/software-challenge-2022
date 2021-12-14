#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <queue>
#include <vector>

template <typename InputType, typename OutputType> class ThreadPool {
    template <typename T> struct ThreadSafeVar {
        std::mutex mutex;
        std::condition_variable condition;
        T var;
    };

    std::vector<std::thread> threads;

    ThreadSafeVar<std::queue<InputType>> inputs;
    bool terminate;

    ThreadSafeVar<std::queue<OutputType>> outputs;

    std::function<OutputType(InputType)> func;

    void worker(){
        while (true) {
            InputType input;

            {
                std::unique_lock<std::mutex> lock(inputs.mutex);

                inputs.condition.wait(lock, [this](){
                    return !inputs.var.empty() || terminate;
                });

                if (terminate) break;

                input = inputs.var.front();
                inputs.var.pop();
            }

            OutputType output = func(input);

            {
                std::unique_lock<std::mutex> lock(outputs.mutex);

                outputs.var.push(output);
            }
            outputs.condition.notify_one();
        }
    }

public:
    ThreadPool(std::function<OutputType(InputType)> func){
        this->func = func;

        for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
            threads.push_back(std::thread(&ThreadPool::worker, this));
        }
    }

    void push(InputType input){
        {
            std::unique_lock<std::mutex> lock(inputs.mutex);
            inputs.var.push(input);
        }
        inputs.condition.notify_one();
    }

    OutputType pop(){
        std::unique_lock<std::mutex> lock(outputs.mutex);

        outputs.condition.wait(lock, [this](){
            return !outputs.var.empty();
        });

        OutputType output = outputs.var.front();
        outputs.var.pop();

        return output;
    }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(inputs.mutex);
            terminate = true;
        }
        inputs.condition.notify_all();

        for (std::thread &thread : threads) {
            thread.join();
        }
    }
};