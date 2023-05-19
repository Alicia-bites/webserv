#include <map>
#include <string>
#include "codes.hpp"
#include "utils.hpp"
#include <algorithm>

static std::map<unsigned int, std::string> codeDesc;
using namespace webserv::http::codes;

void initCodes() {
	static bool initialized = false;
	if (initialized) return;
	initialized = true;
	codeDesc[Continue] = "Continue";
	codeDesc[SwitchingProtocols] = "Switching Protocols";
	codeDesc[Processing] = "Processing";
	codeDesc[EarlyHints] = "Early Hints";
	codeDesc[OK] = "OK";
	codeDesc[Created] = "Created";
	codeDesc[Accepted] = "Accepted";
	codeDesc[NonAuthoritativeInformation] = "Non-authoritative_information";
	codeDesc[NoContent] = "No Content";
	codeDesc[ResetContent] = "Reset Content";
	codeDesc[PartialContent] = "Partial Content";
	codeDesc[MultiStatus] = "Multi-status";
	codeDesc[AlreadyReported] = "Already Reported";
	codeDesc[ImUsed] = "Im Used";
	codeDesc[MultipleChoices] = "Multiple Choices";
	codeDesc[MovedPermanently] = "Moved Permanently";
	codeDesc[Found] = "Found";
	codeDesc[SeeOther] = "See Other";
	codeDesc[NotModified] = "Not Modified";
	codeDesc[UseProxy] = "Use Proxy";
	codeDesc[Unused] = "Unused";
	codeDesc[TemporaryRedirect] = "Temporary Redirect";
	codeDesc[PermanentRedirect] = "Permanent Redirect";
	codeDesc[BadRequest] = "Bad Request";
	codeDesc[Unauthorized] = "Unauthorized";
	codeDesc[PaymentRequired] = "Payment Required";
	codeDesc[Forbidden] = "Forbidden";
	codeDesc[NotFound] = "Not Found";
	codeDesc[MethodNotAllowed] = "Method Not Allowed";
	codeDesc[NotAcceptable] = "Not Acceptable";
	codeDesc[ProxyAuthenticationRequired] = "Proxy Authentication Required";
	codeDesc[RequestTimeout] = "Request Timeout";
	codeDesc[Conflict] = "Conflict";
	codeDesc[Gone] = "Gone";
	codeDesc[LengthRequired] = "Length Required";
	codeDesc[PreconditionFailed] = "Precondition Failed";
	codeDesc[PayloadTooLarge] = "Payload Too Large";
	codeDesc[UriTooLong] = "Uri Too Long";
	codeDesc[UnsupportedMediaType] = "Unsupported Media Type";
	codeDesc[RangeNotSatisfiable] = "Range Not Satisfiable";
	codeDesc[ExpectationFailed] = "Expectation Failed";
	codeDesc[ImATeapot] = "Im A Teapot";
	codeDesc[MisdirectedRequest] = "Misdirected Request";
	codeDesc[UnprocessableEntity] = "Unprocessable Entity";
	codeDesc[Locked] = "Locked";
	codeDesc[FailedDependency] = "Failed Dependency";
	codeDesc[TooEarly] = "Too Early";
	codeDesc[UpgradeRequired] = "Upgrade Required";
	codeDesc[PreconditionRequired] = "Precondition Required";
	codeDesc[TooManyRequests] = "Too Many Requests";
	codeDesc[RequestHeaderFieldsTooLarge] = "Request Header Fields Too Large";
	codeDesc[UnavailableForLegalReasons] = "Unavailable For Legal Reasons";
	codeDesc[InternalServerError] = "Internal Server Error";
	codeDesc[NotImplemented] = "Not Implemented";
	codeDesc[BadGateway] = "Bad Gateway";
	codeDesc[ServiceUnavailable] = "Service Unavailable";
	codeDesc[GatewayTimeout] = "Gateway Timeout";
	codeDesc[HttpVersionNotSupported] = "Http Version Not Supported";
	codeDesc[VariantAlsoNegotiates] = "Variant Also Negotiates";
	codeDesc[InsufficientStorage] = "Insufficient Storage";
	codeDesc[LoopDetected] = "Loop Detected";
	codeDesc[NotExtended] = "Not Extended";
	codeDesc[NetworkAuthenticationRequired] = "Network Authentication Required";
}

namespace webserv {
	namespace http {
		namespace codes {
			/**
			 * @brief Get description for a HTTP status code
			 * 
			 * @param code HTTP status code
			 * @return std::string Description of the code
			 */
			std::string getDesc(unsigned int code) {
				initCodes();
				return codeDesc[code];
			}

			/**
			 * @brief Generate HTML for a HTTP status code
			 * 
			 * @param code HTTP status code
			 * @return std::string HTML code
			 */
			std::string generateHTML(unsigned int code) {
				std::string codeDesc = getDesc(code);
				std::string codeString = webserv::utils::itoa(code);
				// nginx style http page
				return "<html><head><title>" + codeString + " " + codeDesc + "</title></head><body><center><h1>" + codeString + " " + codeDesc + "</h1></center><hr><center>serveurtoile</center></body></html>";
			}
		}
	}
}
