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
#include <QMimeDatabase>

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
#include <taglib/oggfile.h>
#include <taglib/opusfile.h>
#include <taglib/speexfile.h>
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>
#include <taglib/vorbisfile.h>
#include <taglib/wavfile.h>
#include <taglib/wavpackfile.h>
#include <taglib/aifffile.h>
#include <taglib/tpropertymap.h>
#include <taglib/infotag.h>
#include <taglib/popularimeterframe.h>

#include "covers.h"

class Library : public QObject {
	Q_OBJECT
public:
	explicit Library(QObject* parent = 0);
	bool searchDir(QStringList dirPath); //searches the directory for compatible music files, indexes all of them in a json - main index file, this is what is reviewed when songs are added/removed
	bool createLib();
	Q_INVOKABLE QVariantList loadLib();
	Q_INVOKABLE void updateLib(QStringList dirPath);
	Q_INVOKABLE void updateSong(int pos, bool isInQueue = false);
	Q_INVOKABLE void setRating(int rating, int pos, QJsonValue data);

private:
	void createInfoList(QJsonArray fileList, QJsonArray& infoArray, int idNumber);
	QMimeType getMime(QByteArray fn);

signals:
	void songUpdated(int pos, QJsonObject o);
};

#endif //LIBRARY_H
