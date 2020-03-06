#include "lastfm.h"

std::future<std::string> LastFM::getToken() {
	//from https://stackoverflow.com/a/41974669, will reorganize if I see it's a overly messy solution
	//NEXT: create request class
    return std::async(std::launch::async,
        []() mutable {
			std::stringstream res;

			curlpp::Cleanup myCleanup;

			// Our request to be sent.
			curlpp::Easy myRequest;

			// Set the URL.
			myRequest.setOpt<curlpp::options::Url>("https://ws.audioscrobbler.com/2.0/?method=auth.gettoken&api_key=f3c2f73c60e2ac1375206451f7db8ec1&format=json");
			myRequest.setOpt<curlpp::options::WriteStream>(&res);

			// Send request and get a result.
			// By default the result goes to standard output.
			myRequest.perform();

			std::string r = res.str();
			QByteArray ba(r.c_str(), r.length());
			QJsonObject o = QJsonDocument::fromJson(ba).object();
			std::string v = o.value("token").toString().toStdString();

			return v;
    });
}