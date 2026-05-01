#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include <string>
#include <vector>
#include <utility>

#include "common.hpp"
#include "HTTPRequest.hpp"
#include "json.hpp"

class Response {
    private:
        // 外部ライブラリHTTPRequestのhttp::Responseオブジェクト
        http::Response response;

    public:
        // コンストラクタ
        Response();
        Response(http::Response&& response);

        // ステータスコードを取得
        int status();
        // ヘッダーを取得
        std::vector<std::pair<std::string, std::string>> headers();
        // ボディ(内容)を取得
        std::string body();
        // JSON形式のボディ(内容)をパース(解析)して取得
        nlohmann::json to_json();

};

inline int Response::status() {
    return static_cast<int>(response.status.code);
}

inline std::vector<std::pair<std::string, std::string>> Response::headers() {
    return response.headerFields;
}

inline std::string Response::body() {
    return std::string(response.body.begin(), response.body.end());
}

inline nlohmann::json Response::to_json() {
    return nlohmann::json::parse(body());
}


class RequestHandler {
    protected:
        // URL(http://[IP]:[ポート番号])
        std::string url;
        // トークン
        std::string token;
        // ヘッダー
        std::vector<std::pair<std::string, std::string>> headers;

    public:
        // コンストラクタ
        RequestHandler(const std::string& url, const std::string& token, const std::vector<std::pair<std::string, std::string>>& headers);
        // URL(http://[IP]:[ポート番号])を取得(参照)
        const std::string& get_url();
        // トークンを取得(参照)
        const std::string& get_token();
        // ヘッダーを取得(参照)
        const std::vector<std::pair<std::string, std::string>>& get_headers();
};

inline const std::string& RequestHandler::get_url() {
    return url;
}

inline const std::string& RequestHandler::get_token() {
    return token;
}

inline const std::vector<std::pair<std::string, std::string>>& RequestHandler::get_headers() {
    return headers;
}

#endif
