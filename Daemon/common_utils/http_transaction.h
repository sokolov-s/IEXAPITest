#pragma once

#include "http_types.h"

#include <functional>
#include <curl/curl.h>
#include <memory>
#include <sstream>

namespace common_utils {

class HTTPTransaction;

using HTTPTransactionPtr = std::shared_ptr<HTTPTransaction>;
using HTTPTransactionPtr = std::shared_ptr<HTTPTransaction>;
using HTTPTransactionCreationFn = std::function<HTTPTransactionPtr(HTTPTransactionData &&)>;

class HTTPTransaction
{
public:
    HTTPTransaction(HTTPTransactionData &&http_transaction);

    HTTPTransaction(HTTPTransaction const &) = delete;
    HTTPTransaction& operator=(HTTPTransaction const &) = delete;

    virtual ~HTTPTransaction() = default;

    void Perform();
    static std::shared_ptr<std::stringstream> PerformGet(const std::string &url);

    HTTPTransactionData const& GetTransaction() const { return httpTransaction; }

    using CurlConnectionPtr = std::unique_ptr<CURL, decltype(&curl_easy_cleanup)>;
    using CurlSlistPtr = std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)>;
    using CurlFormPostPtr = std::unique_ptr<curl_httppost, decltype(&curl_formfree)>;

    CurlSlistPtr InitHeaders() const;
    CurlConnectionPtr InitConnection() const;
    CurlFormPostPtr InitFormData() const;

    char const * GetMethodStr() const;
    size_t CalculateUploadDataSize() const;

    static size_t ReadCallback(void* data, size_t element_size, size_t elements_count, void* cookie);
    static size_t WriteCallback(void* data, size_t element_size, size_t elements_count, void* cookie);
    static size_t HeaderCallback(void* data, size_t element_size, size_t elements_count, void* cookie);
    static size_t SSL_CTX_Callback(CURL* curl, void* ssl_ctx, void* cookie);

    size_t Read(void* data, size_t element_size, size_t elements_count);
    size_t Write(void* data, size_t element_size, size_t elements_count);
    size_t ProcessResponseHeaders(void* data, size_t element_size, size_t elements_count);
    size_t Process_SSL_CTX(CURL* curl, void* ssl_ctx);

    void ParseResponseHeaders();
    static std::string Trim(std::string &&);

private:
    HTTPTransactionData httpTransaction;
    size_t const uploadDataSize = 0;
    CurlSlistPtr headers;
    std::stringstream responseHeadersData;
    CurlFormPostPtr formData;
    CurlConnectionPtr connection;
    bool completed = false;
};

} // namespace common_utils

