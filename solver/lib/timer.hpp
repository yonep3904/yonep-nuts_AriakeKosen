#ifndef TIMER_HPP_
#define TIMER_HPP_ 

#include <iostream>
#include <chrono>

class Timer {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
        bool is_running;

    public:
        Timer() : start_time{}, end_time{}, is_running{false} {}

        // 計測を開始
        void start() {
            start_time = std::chrono::high_resolution_clock::now();
            is_running = true;
        }

        // 計測を停止
        void stop() {
            end_time = std::chrono::high_resolution_clock::now();
            is_running = false;
        }

        // 計測された時間をマイクロ秒単位で返す
        double elapsed_microseconds() const {
            return std::chrono::duration<double, std::micro>(end_time - start_time).count();
        }

        // 計測された時間をミリ秒単位で返す
        double elapsed_milliseconds() const {
            return std::chrono::duration<double, std::milli>(end_time - start_time).count();
        }

        // 計測された時間を秒単位で返す
        double elapsed_seconds() const {
            return std::chrono::duration<double>(end_time - start_time).count();
        }

        // オーバーロードされた<<演算子
        friend std::ostream& operator<<(std::ostream& os, const Timer& timer) {
            if (timer.is_running) {
                os << "Timer is still running!";
            } else {
                os << timer.elapsed_milliseconds() << "[ms]";
            }
            return os;
        }
};

#endif
