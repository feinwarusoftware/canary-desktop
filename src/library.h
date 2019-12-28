#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QVariant>
#include <QDir>
#include <QDirIterator>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

#include <taglib/fileref.h>
#include <taglib/apefile.h>
#include <taglib/apetag.h>
#include <taglib/asffile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/commentsframe.h>
#include <taglib/flacfile.h>
#include <taglib/id3v1genres.h>
#include <taglib/id3v1tag.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/mp4file.h>
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>
#include <taglib/vorbisfile.h>
#include <taglib/wavfile.h>
#include <taglib/aifffile.h>
#include <taglib/tpropertymap.h>
//#include <taglib/wavproperties.h>

#include <taglib/infotag.h>

#include "covers.h"

//#include <taglib/tfile.h>

class Library : public QObject {
	Q_OBJECT
public:
	explicit Library(QObject* parent = 0);
	bool searchDir(QString dirPath);
	bool createLib();
	Q_INVOKABLE QString getLibrary();
	Q_INVOKABLE QString getAppLocation();

private:
	void loopForTags(TagLib::PropertyMap sMap, QJsonObject& songObj);

signals:

public slots:
};

#endif //LIBRARY_H
