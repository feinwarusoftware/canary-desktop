#ifndef __LASTFM_H__
#define __LASTFM_H__

#include <QObject>

#include "request.h"

#include <QJsonDocument>
#include <QJsonObject>

class LastFM : public QObject {
	Q_OBJECT
public:
	Q_INVOKABLE QString getToken();

private:

};

#endif
