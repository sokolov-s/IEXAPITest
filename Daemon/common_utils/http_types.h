#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace common_utils {

using HTTPHeaders = std::map<std::string, std::string>;

struct NetworkException : std::logic_error
{
    explicit NetworkException(const std::string &msg) : std::logic_error(msg) {}
};

struct HTTPException : NetworkException
{
    explicit HTTPException(const std::string &msg) : NetworkException(msg) {}
};

struct SSLCertificateException : NetworkException
{
    explicit SSLCertificateException(const std::string &msg) : NetworkException(msg) {}
};

enum class HTTPMethod {
    POST,
    GET,
    PUT,
    DEL
};

using HTTPUrl = std::string;
using HTTPFormData = std::vector<std::pair<std::string, std::string>>;
using HTTPRequestData = std::shared_ptr<std::istream>;
using HTTPResponseData = std::shared_ptr<std::ostream>;
using SSLCertBundle = std::shared_ptr<std::istream>;
using HTTPStatusCode = long;

///< Known HTTP codes.
struct HTTPStatusCodes
{
    /// 1xx: Informational - Request received, continuing process
    static const HTTPStatusCode Continue = 100;
    static const HTTPStatusCode SwitchingProtocols = 101;
    static const HTTPStatusCode Processing = 102;
    static const HTTPStatusCode EarlyHints = 103;
    /// 104-199	Unassigned

    /// 2xx: Success - The action was successfully received, understood, and accepted
    static const HTTPStatusCode OK = 200;
    static const HTTPStatusCode Created = 201;
    static const HTTPStatusCode Accepted = 202;
    static const HTTPStatusCode NonAuthoritativeInformation = 203;
    static const HTTPStatusCode NoContent = 204;
    static const HTTPStatusCode ResetContent = 205;
    static const HTTPStatusCode PartialContent = 206;
    static const HTTPStatusCode MultiStatus = 207;
    static const HTTPStatusCode AlreadyReported = 208;
    /// 209-225 Unassigned
    static const HTTPStatusCode IMUsed = 226;

    /// 3xx: Redirection - Further action must be taken in order to complete the request
    static const HTTPStatusCode MultipleChoices = 300;
    static const HTTPStatusCode MovedPermanently = 301;
    static const HTTPStatusCode Found = 302;
    static const HTTPStatusCode SeeOther = 303;
    static const HTTPStatusCode NotModified = 304;
    static const HTTPStatusCode UseProxy = 305;
    /// 306 Unused
    static const HTTPStatusCode TemporaryRedirect = 307;
    static const HTTPStatusCode PermanentRedirect = 308;
    /// 309-399	Unassigned

    /// 4xx: Client Error - The request contains bad syntax or cannot be fulfilled
    static const HTTPStatusCode BadRequest = 400;
    static const HTTPStatusCode Unauthorized = 401;
    static const HTTPStatusCode PaymentRequired = 402;
    static const HTTPStatusCode Forbidden = 403;
    static const HTTPStatusCode NotFound = 404;
    static const HTTPStatusCode MethodNotAllowed = 405;
    static const HTTPStatusCode NotAcceptable = 406;
    static const HTTPStatusCode ProxyAuthenticationRequired = 407;
    static const HTTPStatusCode RequestTimeout = 408;
    static const HTTPStatusCode Conflict = 409;
    static const HTTPStatusCode Gone = 410;
    static const HTTPStatusCode LengthRequired = 411;
    static const HTTPStatusCode PreconditionFailed = 412;
    static const HTTPStatusCode PayloadTooLarge = 413;
    static const HTTPStatusCode URITooLong = 414;
    static const HTTPStatusCode UnsupportedMediaType = 415;
    static const HTTPStatusCode RangeNotSatisfiable = 416;
    static const HTTPStatusCode ExpectationFailed = 417;
    /// 418-420 Unassigned
    static const HTTPStatusCode MisdirectedRequest = 421;
    static const HTTPStatusCode UnprocessableEntity = 422;
    static const HTTPStatusCode Locked = 423;
    static const HTTPStatusCode FailedDependency = 424;
    static const HTTPStatusCode TooEarly = 425;
    static const HTTPStatusCode UpgradeRequired = 426;
    static const HTTPStatusCode Unassigned = 427;
    static const HTTPStatusCode PreconditionRequired = 428;
    static const HTTPStatusCode TooManyRequests = 429;
    /// 430 Unassigned
    static const HTTPStatusCode RequestHeaderFieldsTooLarge = 431;
    /// 432-450 Unassigned
    static const HTTPStatusCode UnavailableForLegalReasons = 451;
    /// 452-499 Unassigned

    /// 5xx: Server Error - The server failed to fulfill an apparently valid request
    static const HTTPStatusCode InternalServerError = 500;
    static const HTTPStatusCode NotImplemented = 501;
    static const HTTPStatusCode BadGateway = 502;
    static const HTTPStatusCode ServiceUnavailable = 503;
    static const HTTPStatusCode GatewayTimeout = 504;
    static const HTTPStatusCode HTTPVersionNotSupported = 505;
    static const HTTPStatusCode VariantAlsoNegotiates = 506;
    static const HTTPStatusCode InsufficientStorage = 507;
    static const HTTPStatusCode LoopDetected = 508;
    /// 509 Unassigned
    static const HTTPStatusCode NotExtended = 510;
    static const HTTPStatusCode NetworkAuthenticationRequired = 511;
    /// 512-599 Unassigned

    static bool is_success(HTTPStatusCode code) {
        return code >= 200 && code <= 299;
    }
};

struct HTTPRequest
{
    HTTPMethod method;
    HTTPUrl url;
    HTTPRequestData data;
    HTTPHeaders headers;
    HTTPFormData form_data;
    SSLCertBundle ca_cert_bundle;
};

struct HTTPResponse
{
    HTTPResponseData data;
    HTTPHeaders headers;
    HTTPStatusCode status_code;
};

struct HTTPTransactionData
{
    HTTPRequest request;
    HTTPResponse response;
    uint32_t read_timeout_sec;
    uint32_t connection_timeout_sec;
};

struct Uri
{
    std::string protocol;
    std::string host;
    std::string port;
    std::string path;
    std::string query;

    bool empty() const {
        return protocol.empty() && host.empty() && port.empty() && path.empty() && query.empty();
    }
};

} // namespace common_utils

