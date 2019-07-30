#include "http_transaction.h"
#include <openssl/ssl.h>
#include <cassert>

using namespace common_utils;

#define ASSERT_THROW_HTTP(condition, message) \
    if (!(condition)) { \
    throw HTTPException(#condition " " + std::string(message)); \
    }

#define ASSERT_THROW_HTTP_SUCCEED(condition) \
    if (!(condition)) { \
    throw HTTPException(#condition " FAILED"); \
    }

#define VERIFY_THROW_SSL_CERT_VALID(status) \
    if (CURLE_SSL_CERTPROBLEM == (status) || CURLE_SSL_CIPHER == (status) \
    || CURLE_SSL_CACERT == (status) || CURLE_SSL_CACERT_BADFILE == (status)) { \
    const std::string status_err = curl_easy_strerror(status); \
    throw SSLCertificateException(status_err); \
    }

#define VERIFY_THROW_NETWORK_ERROR(status) \
    if (CURLE_RECV_ERROR == (status) || CURLE_SEND_ERROR == (status) || CURLE_COULDNT_CONNECT == (status) \
    || CURLE_COULDNT_RESOLVE_PROXY == (status) || CURLE_COULDNT_RESOLVE_HOST == (status) \
    || CURLE_OPERATION_TIMEDOUT == (status)) { \
    const std::string status_err = curl_easy_strerror(status); \
    throw NetworkException(status_err); \
    }

HTTPTransaction::HTTPTransaction(HTTPTransactionData &&http_transaction)
    : httpTransaction(std::move(http_transaction))
    , uploadDataSize(CalculateUploadDataSize())
    , headers(InitHeaders())
    , formData(InitFormData())
    , connection(InitConnection())
    , completed(false)
{
    class GlobalCurlInit
    {
    public:
        GlobalCurlInit(){curl_global_init(CURL_GLOBAL_DEFAULT);}
        ~GlobalCurlInit() {curl_global_cleanup();}
    };

    static GlobalCurlInit globalInit;
}

HTTPTransaction::CurlConnectionPtr HTTPTransaction::InitConnection() const
{
    CurlConnectionPtr curl(curl_easy_init(), curl_easy_cleanup);

    try {
        ASSERT_THROW_HTTP_SUCCEED(!!curl);

        auto curl_handle = curl.get();

        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_URL, httpTransaction.request.url.c_str()));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, GetMethodStr()));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers.get()));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L));

        const auto read_timeout = httpTransaction.read_timeout_sec == 0  || httpTransaction.read_timeout_sec > 300 ? 60 : httpTransaction.read_timeout_sec;
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, read_timeout));

        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1));

        const auto connection_timeout = httpTransaction.connection_timeout_sec == 0 || httpTransaction.connection_timeout_sec > 50 ? 5 : httpTransaction.connection_timeout_sec;
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, connection_timeout));

        if (httpTransaction.request.ca_cert_bundle) {
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 1L));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 2L));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_CTX_FUNCTION, &SSL_CTX_Callback));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_CTX_DATA, this));
        } else {
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L));
        }

        if (uploadDataSize != 0) {
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, &ReadCallback));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_READDATA, this));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE_LARGE, uploadDataSize));
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1));
        }

        if (!httpTransaction.request.form_data.empty()) {
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formData.get()));
        }

        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &WriteCallback));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, &HeaderCallback));
        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, this));
    } catch (std::exception const &) {
        throw HTTPException("HTTP transaction initialization failed");
    }

    return curl;
}

void HTTPTransaction::Perform()
{
    try {
        ASSERT_THROW_HTTP(!completed, "Inapplicable operation: transaction completed!");

        auto curl_handle = connection.get();
        auto transfer_status = curl_easy_perform(curl_handle);

        if (CURLE_OK != transfer_status) {
            VERIFY_THROW_NETWORK_ERROR(transfer_status);
            VERIFY_THROW_SSL_CERT_VALID(transfer_status);
            ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == transfer_status);
        }

        ASSERT_THROW_HTTP_SUCCEED(CURLE_OK == curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &httpTransaction.response.status_code));

        ParseResponseHeaders();

        completed = true;
    } catch (SSLCertificateException const &) {
        throw SSLCertificateException("HTTP operation failed, certificate is not valid");
    } catch (NetworkException const &) {
        throw;
    } catch (std::exception const &) {
        throw HTTPException("HTTP operation failed");
    }
}

std::shared_ptr<std::stringstream> HTTPTransaction::PerformGet(const std::string &url)
{
    HTTPRequest request;
    request.method = HTTPMethod::GET;
    request.url = url;

    auto storage = std::make_shared<std::stringstream>();
    HTTPResponse response {storage, HTTPHeaders(), HTTPStatusCode(HTTPStatusCodes::NoContent)};
    HTTPTransactionData data {request, response, 0, 0};

    HTTPTransaction transaction (std::move(data));
    transaction.Perform();

    auto result = transaction.GetTransaction();
    auto const http_status_code = result.response.status_code;

    if(http_status_code < 200 || http_status_code >= 300) {
        std::string msg = "server response is " + std::to_string(http_status_code) + " " + storage->str();
        throw HTTPException(msg);
    }
    return storage;
}

HTTPTransaction::CurlSlistPtr HTTPTransaction::InitHeaders() const
{
    curl_slist *headers_slist { nullptr };

    for (auto const &header : httpTransaction.request.headers) {
        headers_slist = curl_slist_append(headers_slist, (header.first + ": " + header.second).c_str());
    }

    CurlSlistPtr headers(headers_slist, curl_slist_free_all);

    return headers;
}

HTTPTransaction::CurlFormPostPtr HTTPTransaction::InitFormData() const
{
    curl_httppost *form_post = nullptr;
    curl_httppost *last_ptr = nullptr;

    for (const auto& data : httpTransaction.request.form_data) {
        curl_formadd(
                    &form_post,
                    &last_ptr,
                    CURLFORM_COPYNAME, data.first.c_str(),
                    CURLFORM_COPYCONTENTS, data.second.c_str(),
                    CURLFORM_END
                    );
    }

    return { form_post, curl_formfree };
}

char const * HTTPTransaction::GetMethodStr() const
{
    switch (httpTransaction.request.method) {
    case HTTPMethod::POST:
        return "POST";
    case HTTPMethod::GET:
        return "GET";
    case HTTPMethod::PUT:
        return "PUT";
    case HTTPMethod::DEL:
        return "DELETE";
    default:
        throw HTTPException("Invalid HTTP method");
    }
}

size_t HTTPTransaction::CalculateUploadDataSize() const
{
    auto upload_data = httpTransaction.request.data;

    if (!!upload_data) {
        upload_data->seekg(0, std::ios_base::end);
        auto const upload_data_size = static_cast<size_t>(upload_data->tellg());
        upload_data->seekg(0, std::ios_base::beg);

        return upload_data_size;
    }

    return 0;
}

size_t HTTPTransaction::ReadCallback(void* data, size_t element_size, size_t elements_count, void* cookie)
{
    assert(cookie);
    auto self = reinterpret_cast<HTTPTransaction *>(cookie);
    return self->Read(data, element_size, elements_count);
}

size_t HTTPTransaction::WriteCallback(void* data, size_t element_size, size_t elements_count, void* cookie)
{
    assert(cookie);
    auto self = reinterpret_cast<HTTPTransaction *>(cookie);
    return self->Write(data, element_size, elements_count);
}

size_t HTTPTransaction::HeaderCallback(void* data, size_t element_size, size_t elements_count, void* cookie)
{
    assert(cookie);
    auto self = reinterpret_cast<HTTPTransaction *>(cookie);
    return self->ProcessResponseHeaders(data, element_size, elements_count);
}

size_t HTTPTransaction::SSL_CTX_Callback(CURL* curl, void* ssl_ctx, void* cookie)
{
    assert(cookie);
    auto self = reinterpret_cast<HTTPTransaction *>(cookie);
    return self->Process_SSL_CTX(curl, ssl_ctx);
}

size_t HTTPTransaction::Read(void* data, size_t element_size, size_t elements_count)
{
    auto upload_data = httpTransaction.request.data;
    assert(!!upload_data);
    auto const max_data_size = elements_count * element_size;

    auto const bytes_left = uploadDataSize - static_cast<size_t>(upload_data->tellg());
    auto const read_size = bytes_left > max_data_size ? max_data_size : bytes_left;

    if (read_size != 0) {
        upload_data->read(reinterpret_cast<char*>(data), read_size);
    }

    return read_size;
}

size_t HTTPTransaction::Write(void* data, size_t element_size, size_t elements_count)
{
    auto download_data = httpTransaction.response.data;
    auto const data_size = elements_count * element_size;

    if (!!download_data) {
        download_data->write(reinterpret_cast<char*>(data), data_size);
    }

    return data_size;
}

size_t HTTPTransaction::ProcessResponseHeaders(void* data, size_t element_size, size_t elements_count)
{
    auto const headers_data_size = elements_count * element_size;

    responseHeadersData.write(reinterpret_cast<char*>(data), headers_data_size);

    return headers_data_size;
}

void HTTPTransaction::ParseResponseHeaders()
{
    std::string header;
    while (std::getline(responseHeadersData, header)) {
        auto const delimiter_index = header.find(':', 0);
        if (delimiter_index != std::string::npos) {
            httpTransaction.response.headers.emplace(
                        Trim(header.substr(0, delimiter_index)),
                        Trim(header.substr(delimiter_index + 1))
                        );
        }
    }
}

size_t HTTPTransaction::Process_SSL_CTX(CURL* curl, void* ssl_ctx)
{
    struct RAIIFlags
    {
        RAIIFlags(std::istream &is) : _istream(is), _flags(_istream.exceptions()) {}
        ~RAIIFlags() { _istream.exceptions(_flags); }
    private:
        std::istream &_istream;
        std::ios_base::iostate const _flags;
    };

    try {
        RAIIFlags raii_flags(*httpTransaction.request.ca_cert_bundle);
        httpTransaction.request.ca_cert_bundle->exceptions(std::istream::failbit | std::istream::badbit);

        httpTransaction.request.ca_cert_bundle->seekg(0, std::ios_base::end);
        auto const ca_data_size = static_cast<size_t>(httpTransaction.request.ca_cert_bundle->tellg());
        httpTransaction.request.ca_cert_bundle->seekg(0, std::ios_base::beg);
        std::vector<char> ca_buf(ca_data_size);
        if (ca_data_size > 0) {
            httpTransaction.request.ca_cert_bundle->read(ca_buf.data(), ca_data_size);
        }

        std::unique_ptr<BIO, decltype(&BIO_free)> bio(BIO_new_mem_buf(ca_buf.data(), static_cast<int>(ca_data_size)), BIO_free);
        std::unique_ptr<STACK_OF(X509_INFO), std::function<void(STACK_OF(X509_INFO)*)>> info(PEM_X509_INFO_read_bio(bio.get(), nullptr, nullptr, nullptr),
                                                                                             [](STACK_OF(X509_INFO) *inf) { sk_X509_INFO_pop_free(inf, X509_INFO_free); });

        if (!info) {
            return CURLE_SSL_CACERT_BADFILE;
        }

        auto const cert_count = sk_X509_INFO_num(info.get());

        if (0 == cert_count) {
            return CURLE_SSL_CACERT_BADFILE;
        }

        auto cert_store = SSL_CTX_get_cert_store((SSL_CTX *)ssl_ctx);

        for (int i_cert = 0; i_cert < cert_count; ++i_cert) {
            X509_INFO *itmp = sk_X509_INFO_value(info.get(), i_cert);
            if (itmp->x509) {
                X509_STORE_add_cert(cert_store, itmp->x509);
            }
            if (itmp->crl) {
                X509_STORE_add_crl(cert_store, itmp->crl);
            }
        }
    } catch (std::bad_alloc const &ce) {
        (void)ce;
        return CURLE_OUT_OF_MEMORY;
    } catch (std::exception const &ce) {
        (void)ce;
        return CURLE_SSL_CERTPROBLEM;
    }

    return CURLE_OK;
}

std::string HTTPTransaction::Trim(std::string &&str)
{
    str.erase(0, str.find_first_not_of(" \r"));

    auto const end_pos = str.find_last_not_of(" \r");
    if (std::string::npos != end_pos) {
        str.erase(end_pos + 1);
    }

    return std::move(str);
}

