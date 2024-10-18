#include <iostream>
#include <string>
#include <stdlib.h>

#include "common.hpp"
#include "communication.hpp"

#include "HTTPRequest.hpp"
#include "font.hpp"

using namespace std;

namespace comm {

    string indent = "    ";

    Json get_problem(string url, string token, int interval_ms, int lim) {
        http::Request request(url + "/problem");
        http::HeaderFields headers = {
            { "Content-Type", "application/json"},
            { "Procon-Token", token}
        };

        for (int i = 0; i < lim; i++) {
            cout << indent << "GET problem[" << i+1 << "] ... ";
            auto response = request.send("GET", "", headers);
            int status = static_cast<int>(response.status.code);
            
            cout << "Status : ";
            if (status == 200) {
                cout << font::green << status << font::color_reset << endl;
                return nlohmann::json::parse(std::string(response.body.begin(), response.body.end()));
            } else {
                cout << font::red << status << font::color_reset << endl;
            }

            wait_millisec(interval_ms);
        }
        cout << indent << "Count UP !!" << endl;
        exit(1);
    }

    bool post_answer_to_server(string url, string token, nlohmann::json body, int interval_ms, int lim) {
        http::Request request(url + "/answer");
        
        http::HeaderFields headers = {
            { "Content-Type", "application/json"},
            { "Procon-Token", token}
        };

        for (int i = 0; i < lim; i++) {
            cout << indent << "POST answer[" << i+1 << "] ... ";
            auto response = request.send("POST", body.dump(), headers);
            int status = static_cast<int>(response.status.code);

            cout << "Status : ";
            if (status == 200) {
                cout << font::green << status << font::color_reset << endl;
                return true;
            } else {
                cout << font::red << status << font::color_reset << endl;
            }

            wait_millisec(interval_ms);
        }
        return false;
    }

    bool post_answer_to_manager(string url, string token, nlohmann::json body, int interval_ms, int lim) {
        http::Request request(url + "/answer");
        
        http::HeaderFields headers = {
            { "Content-Type", "application/json"},
            { "Authorization", token}
        };

        for (int i = 0; i < lim; i++) {
            cout << indent << "POST answer[" << i+1 << "]... ";
            auto response = request.send("POST", body.dump(), headers);
            int status = static_cast<int>(response.status.code);
            
            cout << "Status : ";
            if (status == 200) {
                cout << font::green << status << font::color_reset << endl;
                return true;
            } else {
                cout << font::red << status << font::color_reset << endl;
            }

            wait_millisec(interval_ms);
        }
        return false;
    }
}
