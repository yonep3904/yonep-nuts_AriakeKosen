#ifndef INHERITED_HANDLERS_HPP
#define INHERITED_HANDLERS_HPP

#include <string>

#include "common.hpp"
#include "request_handler.hpp"

#include "HTTPRequest.hpp"
#include "json.hpp"

class MatchServerHandler : public RequestHandler {
    private:
        // 外部ライブラリHTTPRequestのhttp::Requestオブジェクト
        http::Request get_problrm, post_answer; 

    public:
        // コンストラクタ 
        MatchServerHandler(const std::string& url, const std::string& token);
        // 大会サーバーに問題情報をGETするリクエストを送り結果をResponseオブジェクトで返す
        Response Get_problem();
        // 大会サーバーに解答をPOSTするリクエストを送り結果をResponseオブジェクトで返す
        Response Post_answer(const std::string& answer);
        Response Post_answer(const nlohmann::json& answer);
};


class AnswerManagerHandlear : public RequestHandler {
    private:
        // 外部ライブラリHTTPRequestのhttp::Requestオブジェクト
        http::Request post_move, post_complete, post_answer;

    public:
        // コンストラクタ
        AnswerManagerHandlear(const std::string& url, const std::string& token);
        // 解答管理サーバーに解答(手数情報のみ)をPOSTするリクエストを送り結果をResponseオブジェクトで返す
        Response Post_moves(int moves);
        // 解答管理サーバーに解答の送信が完了したことを知らせるためのPOSTリクエストを送り結果をResponseオブジェクトで返す
        Response Post_complete(bool success);
        // 解答管理サーバに直接解答を送信する
        Response Post_answer(const std::string& answer);
        Response Post_answer(const nlohmann::json& answer);
};

#endif