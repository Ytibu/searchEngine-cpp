# Search Engine Client RESTful API 文档

## 1. 概述

该前端客户端依赖两个核心接口：

1. 根据用户输入的搜索意图生成推荐关键词
2. 根据用户最终确认的关键词返回网页链接结果

建议接口统一返回 `application/json`，并采用 RESTful 风格路径。

接口基础路径示例：

```text
/api/v1
```

---

## 2. 通用响应格式

### 成功响应

```json
{
  "code": 0,
  "message": "success",
  "data": {}
}
```

### 失败响应

```json
{
  "code": 4001,
  "message": "invalid request",
  "data": null
}
```

### 字段说明

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| code | number | 业务状态码，`0` 表示成功 |
| message | string | 响应说明 |
| data | object \| null | 业务数据 |

---

## 3. 推荐关键词生成接口

### 接口说明

- 方法：`POST`
- 路径：`/api/v1/keywords/recommendations`
- 描述：根据用户输入的搜索意图，返回推荐关键词列表

### 请求头

```http
Content-Type: application/json
```

### 请求参数

```json
{
  "intent": "人工智能前沿应用",
  "size": 8
}
```

### 请求参数说明

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| intent | string | 是 | 用户输入的原始搜索意图 |
| size | number | 否 | 返回推荐词数量，默认 `8` |

### 成功响应示例

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "intent": "人工智能前沿应用",
    "recommendations": [
      "人工智能前沿应用",
      "人工智能前沿应用 最新资讯",
      "人工智能前沿应用 解决方案",
      "人工智能前沿应用 使用场景",
      "人工智能前沿应用 入门指南",
      "人工智能前沿应用 案例分析",
      "人工智能前沿应用 趋势观察",
      "人工智能前沿应用 常见问题"
    ]
  }
}
```

### 失败响应示例

```json
{
  "code": 4001,
  "message": "intent can not be empty",
  "data": null
}
```

---

## 4. 网页链接查询接口

### 接口说明

- 方法：`POST`
- 路径：`/api/v1/search/results`
- 描述：根据用户确认的关键词返回网页链接结果

### 请求头

```http
Content-Type: application/json
```

### 请求参数

```json
{
  "keyword": "人工智能前沿应用 最新资讯",
  "page": 1,
  "pageSize": 10
}
```

### 请求参数说明

| 字段 | 类型 | 必填 | 说明 |
| --- | --- | --- | --- |
| keyword | string | 是 | 用户最终确认的关键词 |
| page | number | 否 | 页码，默认 `1` |
| pageSize | number | 否 | 每页条数，默认 `10` |

### 成功响应示例

```json
{
  "code": 0,
  "message": "success",
  "data": {
    "keyword": "人工智能前沿应用 最新资讯",
    "page": 1,
    "pageSize": 10,
    "total": 2,
    "results": [
      {
        "title": "人工智能前沿应用资讯聚合",
        "description": "收录人工智能前沿应用相关的新闻、专题和行业观察。",
        "url": "https://example.com/search?q=人工智能前沿应用%20最新资讯"
      },
      {
        "title": "人工智能前沿应用行业洞察",
        "description": "聚焦人工智能行业应用趋势与真实案例。",
        "url": "https://example.com/articles/ai-trends"
      }
    ]
  }
}
```

### 失败响应示例

```json
{
  "code": 4002,
  "message": "keyword can not be empty",
  "data": null
}
```

---

## 5. 前端调用顺序

1. 用户输入搜索意图
2. 前端调用 `POST /api/v1/keywords/recommendations`
3. 用户从推荐列表中选择一个关键词
4. 前端调用 `POST /api/v1/search/results`
5. 页面展示后端返回的网页链接列表

---

## 6. 状态码建议

| code | 含义 |
| --- | --- |
| 0 | 成功 |
| 4001 | 请求参数错误 |
| 4002 | 关键词为空 |
| 5000 | 服务内部错误 |

---

## 7. 前端对接说明

当前项目中的前端默认请求以下接口：

```text
POST /api/v1/keywords/recommendations
POST /api/v1/search/results
```

如果后端接口与当前页面不在同域下，可以在 `app.js` 中调整：

```javascript
const API_BASE_URL = "https://your-domain.com";
```

---

## 8. 项目文件建议

```text
SearchEngine-Client/
├─ index.html
├─ styles.css
├─ app.js
└─ api-docs.md
```
