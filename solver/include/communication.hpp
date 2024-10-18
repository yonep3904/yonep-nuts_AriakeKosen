#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "httplib.h"
#include "json.hpp"
#include "font.hpp"

namespace comm {
    using Result = httplib::Result;
    using Json = nlohmann::json;

    // 任意の時間[ms]の待機を行う
    void wait_millisec(int ms);
    // 大会サーバーから問題を取得
    Json get_problem(std::string url, std::string token, int interval_ms, int lim);
    // 大会サーバーに解答を提出
    bool post_answer_to_server(std::string url, std::string token, nlohmann::json body, int interval_ms, int lim);
    // 解答管理サーバーに解答を提出
    bool post_answer_to_manager(std::string url, std::string token, nlohmann::json body, int interval_ms, int lim);
}


namespace comm {
    inline void wait_millisec(int ms) {
        font::dim();
        std::cout << "Waiting for " << ms << " [ms]..." << std::endl;
        font::reset();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}
#endif