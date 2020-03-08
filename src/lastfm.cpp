#include "lastfm.h"

Request request;

QString LastFM::getToken() {
	std::string tokenRes = request.get("https://ws.audioscrobbler.com/2.0/?method=auth.gettoken&api_key=f3c2f73c60e2ac1375206451f7db8ec1&format=json").get();
	QByteArray ba(tokenRes.c_str(), tokenRes.length());
	QJsonObject o = QJsonDocument::fromJson(ba).object();

	return o.value("token").toString();
}