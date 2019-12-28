#ifndef __COVERS_H__
#define __COVERS_H__

#include <taglib/fileref.h>
#include <taglib/apefile.h>
#include <taglib/apetag.h>
#include <taglib/asffile.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/commentsframe.h>
#include <taglib/flacfile.h>
#include <taglib/id3v1genres.h>
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

#include <QObject>
#include <QVariant>
#include <QString>
#include <QImage>
#include <QBuffer>

class CCover : public QObject {
	Q_OBJECT
public:
	explicit CCover(QObject* parent = 0);
	bool getCover(const TagLib::FileRef& fr, QImage& target, int w, int h);

signals:

public slots:
};

#endif