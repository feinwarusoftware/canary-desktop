#ifndef __LASTFM_H__
#define __LASTFM_H__

#include <QObject>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <future>

#include <string>
#include <sstream>

#include <QJsonDocument>
#include <QJsonObject>

class LastFM : public QObject {
	Q_OBJECT
public:
	std::future<std::string> getToken();

private:

};

#endif
