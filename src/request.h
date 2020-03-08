#ifndef REQUEST_H
#define REQUEST_H

#include <QObject>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <future>

#include <string>
#include <sstream>

class Request : public QObject {
	Q_OBJECT

	public:
		std::future<std::string> get(std::string const& url);
		std::future<std::string> post(std::string const& url/*, std::string const& body*/);

};

#endif