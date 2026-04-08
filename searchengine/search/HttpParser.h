#ifndef __HTTPPARSER_H__
#define __HTTPPARSER_H__

#include <string>
#include <map>

using std::string;
using std::map;

class HttpParser {
public:
    enum class HttpMethod {
        GET,
        POST,
        UNKNOWN
    };

    // 请求结构体
    struct HttpRequest {
        HttpMethod method;
        string path;
        map<string, string> headers;
        string body;
    };

    // 响应结构体
    struct HttpResponse {
        int statusCode;
        string statusMessage;
        map<std::string, std::string> headers;
        string body;
    };

    static HttpRequest parseRequest(const std::string& requestStr);
    static string buildResponse(const HttpResponse& response);
    
    static string buildSuccessResponse(const std::string& jsonBody);
    static string buildErrorResponse(int errorCode, const std::string& errorMessage);

    string getRecommendations(const std::string& requestBody);
    string getResults(const std::string& requestBody);
};

#endif // __HTTPPARSER_H__
