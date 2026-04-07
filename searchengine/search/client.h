#include <string>

#include <workflow/MySQLMessage.h>
#include <workflow/MySQLResult.h>
#include <workflow/WFFacilities.h>
#include <wfrest/Json.h>
#include <wfrest/HttpServer.h>
#include <iostream>
#include <set>
#include <vector>

using namespace wfrest;
using std::string;
using std::cout;
using std::endl;
using std::vector;

/**
 * @brief 轻量 HTTP 服务类（测试版）
 *
 * 主要职责：
 * 1) 启动 wfrest 服务并注册路由；
 * 2) 对外提供推荐词接口与搜索结果接口；
 * 3) 托管静态资源目录，供前端页面访问。
 *
 * 说明：
 * - 当前推荐词为“规则生成”的测试实现；
 * - 后续可将 buildTestRecommendations 替换为真实推荐系统调用。
 */
class client
{
public:
    // cnt 用于 WaitGroup 计数，通常传 1，表示主线程阻塞直到手动结束服务。
    client(int cnt) 
    : _waitGroup(cnt)
    {}

    ~client(){}

    /**
     * @brief 启动 HTTP 服务
     * @param port 监听端口
     *
     * 行为：
     * - 成功启动后打印路由；
     * - 阻塞等待（_waitGroup.wait）；
     * - 结束时停止服务。
     */
    void start(unsigned short port)
    {
        if(_httpserver.track().start(port) == 0) {
        _httpserver.list_routes();
        _waitGroup.wait();
        _httpserver.stop();
        }
    }

    /**
     * @brief 注册所有模块（启动前调用）
     *
     * 当前包含：
     * - 静态资源模块（页面/脚本/样式）；
     * - API 模块（推荐词、搜索结果）。
     */
    void loadModules()
    {
        //加载模块
        loadStaticResourceModule();
        loadApiModule();
    }

    
private:
    /**
     * @brief 根据输入词构造测试推荐词
     * @param intent 用户输入词（意图）
     * @param size 返回条数上限，<=0 时使用默认值 8
     * @return 推荐词列表
     *
     * 生成策略：
     * - 以 intent 为种子，拼接常见检索后缀（如“是什么/教程/实战”等）；
     * - 使用 set 去重，避免重复项；
     * - 最终按 size 截断。
     */
    vector<string> buildTestRecommendations(const string &intent, int size)
    {
        if (size <= 0) {
            size = 8;
        }

        // Use a set to avoid duplicate recommendation entries.
        std::set<string> dedup;
        vector<string> generated;

        auto pushCandidate = [&](const string &candidate) {
            if (candidate.empty()) {
                return;
            }
            if (dedup.insert(candidate).second) {
                generated.push_back(candidate);
            }
        };

        pushCandidate(intent);
        pushCandidate(intent + " 是什么");
        pushCandidate(intent + " 怎么用");
        pushCandidate(intent + " 教程");
        pushCandidate(intent + " 入门");
        pushCandidate(intent + " 实战");
        pushCandidate(intent + " 面试题");
        pushCandidate(intent + " 最佳实践");
        pushCandidate(intent + " 示例");
        pushCandidate(intent + " 常见问题");

        if (generated.size() > static_cast<size_t>(size)) {
            generated.resize(size);
        }

        return generated;
    }

    void loadStaticResourceModule()
    {
        //加载静态资源模块
        // 绑定静态目录：将 URL 路径映射到 static 目录。
        // 例如：/index.html -> ../../static/index.html
        // 注意：当前环境下直接访问 / 可能 404，建议访问 /index.html。
        _httpserver.Static("/", "../../static"); 
    }

    void loadApiModule()
    {
        // 推荐词接口
        // POST /api/v1/keywords/recommendations
        // Request JSON:
        // {
        //   "intent": "搜索引擎",
        //   "size": 8
        // }
        // Response JSON:
        // {
        //   "code": 0,
        //   "message": "success",
        //   "data": {
        //     "intent": "搜索引擎",
        //     "recommendations": ["...", "..."]
        //   }
        // }
        _httpserver.POST("/api/v1/keywords/recommendations", [this](const HttpReq *req, HttpResp *resp) {
            // 参数校验：必须包含 intent 字段。
            if (req->json().is_null() || !req->json().has("intent")) {
                Json response;
                response["code"] = 4001;
                response["message"] = "intent can not be empty";
                response["data"] = nullptr;
                resp->Json(response);
                return;
            }

            std::string intent = req->json()["intent"].get<std::string>();
            // size 可选，不传时默认 8。
            int size = req->json().has("size") ? req->json()["size"].get<int>() : 8;
            vector<string> recommendationsVec = buildTestRecommendations(intent, size);

            Json response;
            response["code"] = 0;
            response["message"] = "success";

            response["data"]["intent"] = intent;
            response["data"]["recommendations"] = Json::Array();
            response["intent"] = intent;
            response["recommendations"] = Json::Array();
            for (const auto &word : recommendationsVec) {
                response["data"]["recommendations"].push_back(word);
                response["recommendations"].push_back(word);
            }

            resp->Json(response);
        });

        // 搜索结果接口（当前为 mock 数据）
        // POST /api/v1/search/results
        // Request JSON:
        // {
        //   "keyword": "搜索引擎",
        //   "page": 1,
        //   "pageSize": 10
        // }
        _httpserver.POST("/api/v1/search/results", [](const HttpReq *req, HttpResp *resp) {
            // 参数校验：必须包含 keyword 字段。
            if (req->json().is_null() || !req->json().has("keyword")) {
                Json response;
                response["code"] = 4002;
                response["message"] = "keyword can not be empty";
                response["data"] = nullptr;
                resp->Json(response);
                return;
            }

            std::string keyword = req->json()["keyword"].get<std::string>();
            // 分页参数可选，不传则走默认值。
            int page = req->json().has("page") ? req->json()["page"].get<int>() : 1;
            int pageSize = req->json().has("pageSize") ? req->json()["pageSize"].get<int>() : 10;

            Json response;
            response["code"] = 0;
            response["message"] = "success";

            response["data"]["keyword"] = keyword;
            response["data"]["page"] = page;
            response["data"]["pageSize"] = pageSize;
            response["data"]["total"] = 2; // Mock total
            response["keyword"] = keyword;
            response["page"] = page;
            response["pageSize"] = pageSize;
            response["total"] = 2;

            response["data"]["results"] = Json::Array();
            response["results"] = Json::Array();
            
            Json res1;
            res1["title"] = keyword + " 搜索结果1";
            res1["description"] = "这是关于 " + keyword + " 的描述内容。";
            res1["url"] = "https://example.com/search?q=" + keyword;
            response["data"]["results"].push_back(res1);
            response["results"].push_back(res1);
            
            Json res2;
            res2["title"] = keyword + " 搜索结果2";
            res2["description"] = "更多关于 " + keyword + " 的内容。";
            res2["url"] = "https://example.com/articles";
            response["data"]["results"].push_back(res2);
            response["results"].push_back(res2);
            
            resp->Json(response);
        });
    }

private:
    // 用于阻塞主线程，保证服务进程持续运行。
    WFFacilities::WaitGroup _waitGroup;
    // wfrest HTTP 服务实例。
    wfrest::HttpServer _httpserver;
};