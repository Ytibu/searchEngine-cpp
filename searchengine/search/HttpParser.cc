#include "HttpParser.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

HttpParser::HttpRequest HttpParser::parseRequest(const std::string& requestStr) {
    HttpRequest request;
    request.method = HttpMethod::UNKNOWN;
    if (requestStr.empty()) return request;

    std::istringstream stream(requestStr);
    std::string line;

    // Parse request line
    if (std::getline(stream, line) && !line.empty()) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        std::istringstream lineStream(line);
        std::string methodStr;
        lineStream >> methodStr >> request.path;

        if (methodStr == "GET") request.method = HttpMethod::GET;
        else if (methodStr == "POST") request.method = HttpMethod::POST;
    }

    // Parse headers
    while (std::getline(stream, line) && line != "\r" && line != "") {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Trim spaces from value
            size_t first = value.find_first_not_of(" \t");
            if (first != std::string::npos) {
                value = value.substr(first);
            }
            request.headers[key] = value;
        }
    }

    // Parse body
    std::ostringstream bodyStream;
    bodyStream << stream.rdbuf();
    request.body = bodyStream.str();

    return request;
}

std::string HttpParser::buildResponse(const HttpResponse& response) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << response.statusCode << " " << response.statusMessage << "\r\n";
    
    for (const auto& header : response.headers) {
        responseStream << header.first << ": " << header.second << "\r\n";
    }
    
    responseStream << "Content-Length: " << response.body.length() << "\r\n";
    responseStream << "\r\n";
    responseStream << response.body;

    return responseStream.str();
}

std::string HttpParser::buildSuccessResponse(const std::string& jsonBody) {
    HttpResponse response;
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.headers["Content-Type"] = "application/json; charset=utf-8";
    
    std::ostringstream jsonStream;
    jsonStream << "{\"code\":0,\"message\":\"success\",\"data\":" << jsonBody << "}";
    response.body = jsonStream.str();

    return buildResponse(response);
}

std::string HttpParser::buildErrorResponse(int errorCode, const std::string& errorMessage) {
    HttpResponse response;
    response.statusCode = 200;
    response.statusMessage = "OK";
    response.headers["Content-Type"] = "application/json; charset=utf-8";

    std::ostringstream jsonStream;
    jsonStream << "{\"code\":" << errorCode << ",\"message\":\"" << errorMessage << "\",\"data\":null}";
    response.body = jsonStream.str();

    return buildResponse(response);
}

// 根据请求的 JSON Body 构建推荐词组的 JSON 字符串结果
std::string HttpParser::getRecommendations(const std::string& requestBody) {
    std::string intent;
    int size = 8; // 默认返回8条推荐

    // 1. 简易解析 JSON 请求体: {"intent": "xxx", "size": 8}
    size_t intentPos = requestBody.find("\"intent\"");
    if (intentPos != std::string::npos) {
        size_t colonPos = requestBody.find(":", intentPos);
        size_t startQuote = requestBody.find("\"", colonPos);
        size_t endQuote = requestBody.find("\"", startQuote + 1);
        if (startQuote != std::string::npos && endQuote != std::string::npos) {
            intent = requestBody.substr(startQuote + 1, endQuote - startQuote - 1);
        }
    }

    size_t sizePos = requestBody.find("\"size\"");
    if (sizePos != std::string::npos) {
        size_t colonPos = requestBody.find(":", sizePos);
        size_t endDigitPos = requestBody.find_first_of(",}", colonPos);
        if (colonPos != std::string::npos && endDigitPos != std::string::npos) {
            std::string numStr = requestBody.substr(colonPos + 1, endDigitPos - colonPos - 1);
            size_t first = numStr.find_first_not_of(" \t");
            if (first != std::string::npos) {
                try {
                    size = std::stoi(numStr.substr(first));
                } catch (...) {
                    size = 8; // 转换失败则使用默认值
                }
            }
        }
    }

    // 2. 异常控制：如果没传 intent，交给上层去下发 error 信息
    if (intent.empty()) {
        return ""; // 表示参数错误，应在外层构建 buildErrorResponse(4001, "intent can not be empty")
    }

    // 3. 业务逻辑（模拟）：组合推荐结果（替换为你项目里实际的 Trie 树搜索、Redis缓存提取或者分词推荐结果）
    // 参考了 api-docs.md 里面的返回值要求
    std::vector<std::string> suffixes = {
        "", "-最新资讯", "-解决方案", "-使用场景", 
        "-入门指南", "-案例分析", "-趋势观察", "-常见问题"
    };

    // 4. 生成返回的 JSON 数据结构
    std::ostringstream jsonOut;
    jsonOut << "{";
    jsonOut << "\"intent\":\"" << intent << "\",";
    jsonOut << "\"recommendations\":[";
    
    int actualSize = std::min(size, (int)suffixes.size());
    for (int i = 0; i < actualSize; ++i) {
        jsonOut << "\"" << intent << suffixes[i] << "\"";
        if (i < actualSize - 1) {
            jsonOut << ",";
        }
    }
    
    jsonOut << "]}";

    return jsonOut.str();
}

std::string HttpParser::getResults(const std::string& requestBody) 
{
    std::string keyword = "";
    int page = 1;
    int pageSize = 10;

    size_t keywordPos = requestBody.find("\"keyword\"");
    if (keywordPos != std::string::npos) {
        size_t colonPos = requestBody.find(":", keywordPos);
        size_t startQuote = requestBody.find("\"", colonPos);
        size_t endQuote = requestBody.find("\"", startQuote + 1);
        if (startQuote != std::string::npos && endQuote != std::string::npos) {
            keyword = requestBody.substr(startQuote + 1, endQuote - startQuote - 1);
        }
    }

    size_t pagePos = requestBody.find("\"page\"");
    if (pagePos != std::string::npos) {
        size_t colonPos = requestBody.find(":", pagePos);
        size_t endDigitPos = requestBody.find_first_of(",}", colonPos);
        if (colonPos != std::string::npos && endDigitPos != std::string::npos) {
            std::string numStr = requestBody.substr(colonPos + 1, endDigitPos - colonPos - 1);
            size_t first = numStr.find_first_not_of(" \t");
            if (first != std::string::npos) {
                try {
                    page = std::stoi(numStr.substr(first));
                } catch (...) {
                    page = 1;
                }
            }
        }
    }

    size_t pageSizePos = requestBody.find("\"pageSize\"");
    if (pageSizePos != std::string::npos) {
        size_t colonPos = requestBody.find(":", pageSizePos);
        size_t endDigitPos = requestBody.find_first_of(",}", colonPos);
        if (colonPos != std::string::npos && endDigitPos != std::string::npos) {
            std::string numStr = requestBody.substr(colonPos + 1, endDigitPos - colonPos - 1);
            size_t first = numStr.find_first_not_of(" \t");
            if (first != std::string::npos) {
                try {
                    pageSize = std::stoi(numStr.substr(first));
                } catch (...) {
                    pageSize = 10;
                }
            }
        }
    }

    if (keyword.empty()) {
        return "";
    }

    std::vector<std::pair<std::string, std::string>> resultItems = {
        {keyword + "资讯聚合", "收录相关主题的新闻、专题和行业观察。"},
        {keyword + "行业洞察", "聚焦该主题的趋势分析与真实案例。"},
        {keyword + "应用指南", "提供入门、实践和落地方案参考。"}
    };

    int total = static_cast<int>(resultItems.size());
    int actualSize = std::min(pageSize, total);

    std::ostringstream jsonOut;
    jsonOut << "{";
    jsonOut << "\"keyword\":\"" << keyword << "\",";
    jsonOut << "\"page\":" << page << ",";
    jsonOut << "\"pageSize\":" << pageSize << ",";
    jsonOut << "\"total\":" << total << ",";
    jsonOut << "\"results\":[";

    for (int i = 0; i < actualSize; ++i) {
        jsonOut << "{";
        jsonOut << "\"title\":\"" << resultItems[i].first << "\",";
        jsonOut << "\"description\":\"" << resultItems[i].second << "\",";
        jsonOut << "\"url\":\"https://example.com/search?q=" << keyword << "\"";
        jsonOut << "}";
        if (i < actualSize - 1) {
            jsonOut << ",";
        }
    }

    jsonOut << "]}";
    return jsonOut.str();
}