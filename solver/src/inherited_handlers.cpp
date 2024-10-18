#include "inherited_handlers.hpp"

#include <string>

#include "common.hpp"
#include "request_handler.hpp"

#include "HTTPRequest.hpp"
#include "json.hpp"

using namespace std;

MatchServerHandler::MatchServerHandler(const string& url, const string& token) :
    RequestHandler(url, token, {{"Content-Type", "application/json"}, {"Procon-Token", token}}),
    get_problrm(http::Request(url + "/problem")),
    post_answer(http::Request(url + "/answer"))
{

}

Response MatchServerHandler::Get_problem() {
    return Response(get_problrm.send("GET", "", headers));
}

Response MatchServerHandler::Post_answer(const string& answer) {
    return Response(post_answer.send("POST", answer, headers));
}

Response MatchServerHandler::Post_answer(const nlohmann::json& answer) {
    return Response(post_answer.send("POST", answer.dump(), headers));
}


AnswerManagerHandlear::AnswerManagerHandlear(const string& url, const string& token) :
    RequestHandler(url, token, {{"Content-Type", "application/json"}, {"Authorization", token}}),
    post_move(http::Request(url + "/moves")),
    post_complete(http::Request(url + "/complete")),
    post_answer(http::Request(url + "/answer"))
{

}

Response AnswerManagerHandlear::Post_moves(int moves) {
    nlohmann::json body;
    body["moves"] = moves;
    return Response(post_move.send("POST", body.dump(), headers));
}

Response AnswerManagerHandlear::Post_complete(bool success) {
    nlohmann::json body;
    body["success"] = success;
    return Response(post_complete.send("POST", body.dump(), headers));
}

Response AnswerManagerHandlear::Post_answer(const std::string& answer) {
    return Response(post_answer.send("POST", answer, headers));
}

Response AnswerManagerHandlear::Post_answer(const nlohmann::json& answer) {
    return Response(post_answer.send("POST", answer.dump(), headers));
}