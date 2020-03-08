#include "request.h"

//reference: https://stackoverflow.com/a/41974669
std::future<std::string> Request::get(std::string const& url) {
	return std::async(std::launch::async,
		[](std::string const& url) mutable {
			std::stringstream res;

			curlpp::Cleanup myCleanup;

			// Our request to be sent.
			curlpp::Easy myRequest;

			// Set the URL.
			myRequest.setOpt<curlpp::options::Url>(url);
			myRequest.setOpt<curlpp::options::WriteStream>(&res);

			// Send request and get a result.
			// By default the result goes to standard output.
			myRequest.perform();

			return std::string(res.str());
	}, url);
}

//std::future<std::string> Request::post(std::string const& url/*, std::string const& body*/) {
	//return std::string("oi");
//}