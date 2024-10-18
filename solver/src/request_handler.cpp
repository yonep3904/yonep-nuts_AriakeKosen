#include "request_handler.hpp"

#include <string>
#include <vector>
#include <utility>

#include "common.hpp"

#include "HTTPRequest.hpp"
#include "json.hpp"

using namespace std;

Response::Response()
{

}

Response::Response(http::Response&& response) :
    response(move(response))
{

}


RequestHandler::RequestHandler(const string& url, const string& token, const vector<pair<string, string>>& headers) : 
    url(url),
    token(token),
    headers(headers)
{

}
