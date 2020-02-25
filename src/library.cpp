#include "library.h"

#include <QDebug>

QList<QString> supportedFormats = { "mp3", "ogg", "wav", "aiff", "flac", "wv", "opus", "m4a", "caf", "ape", "aac", "ac3", "tta" };

Library::Library(QObject* parent) : QObject(parent) {

}

bool Library::searchDir(QStringList dirPath) {
	QJsonArray jsonArr;

	for (QString& location : dirPath) {
		QDirIterator it(location, QDirIterator::Subdirectories);

		while (it.hasNext()) {
			//qDebug() << it.next();

			QFileInfo fileInfo(it.next());
			if (fileInfo.isFile() && supportedFormats.indexOf(fileInfo.suffix()) != -1) { //if it's a file / if it's a supported file
				qDebug() << fileInfo.absoluteFilePath();
				jsonArr.append(fileInfo.absoluteFilePath());
			}
		}
	}

	if (!QDir("userdata").exists()) {
		QDir().mkdir("userdata");
	}

	qDebug() << "writing file...";
	QFile jsonFile("./userdata/fileList.json");
	bool createJSON = jsonFile.open(QIODevice::ReadWrite);
	QJsonDocument jsonDoc(jsonArr);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();

	return createJSON && createLib();
}

bool Library::createLib() {
	QFile fileListJson("./userdata/fileList.json");
	bool readJSON = fileListJson.open(QIODevice::ReadOnly/* | QIODevice::Text*/);
	QJsonDocument list = QJsonDocument::fromJson(fileListJson.readAll());
	QJsonArray listArray = list.array();

	QJsonArray libArray;

	int idNumber = 0;

	if (!QDir("react/canary-desktop/public/cache").exists()) {
		QDir().mkdir("react/canary-desktop/public/cache");
	}

	if (!QDir("react/canary-desktop/public/cache/covers").exists()) {
		QDir().mkdir("react/canary-desktop/public/cache/covers");
	}

	foreach(const QJsonValue & value, listArray) {
		QString fileDirString = value.toString();
		QByteArray fileName = QFile::encodeName(fileDirString); //using QFile to encode dir so it's readable to TagLib

		QJsonObject song;

		TagLib::FileRef fr(fileName.toStdString().c_str());

		bool newAlbum = true;

		if (fr.audioProperties() != NULL) {
			song.insert("lengthInSeconds", fr.audioProperties()->lengthInSeconds());
			song.insert("bitrate", fr.audioProperties()->bitrate());
			song.insert("sampleRate", fr.audioProperties()->sampleRate());
			song.insert("channels", fr.audioProperties()->channels());
		}
		else {
			continue; //audio properties (length, etc. are not readable, do not add song to the library
		}

		qDebug() << fileDirString;

		song.insert("dir", fileDirString); //write the file directory into the library, even if it has no tagss

		//checks for format-specific tags - will be replaced with the normal ones if repeated
		if (TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(fr.file()))
		{
			if (file->ID3v2Tag())
			{
				loopForTags(file->ID3v2Tag()->properties(), song);
			}
			else if (file->APETag())
			{
				loopForTags(file->APETag()->properties(), song);
			}
		}
		else if (TagLib::FLAC::File* file = dynamic_cast<TagLib::FLAC::File*>(fr.file()))
		{
			if (file->ID3v2Tag())
			{
				loopForTags(file->ID3v2Tag()->properties(), song);
			}
		}
		else if (TagLib::MP4::File* file = dynamic_cast<TagLib::MP4::File*>(fr.file()))
		{
			if (file->tag()) {
				loopForTags(file->tag()->properties(), song);
			}
		}
		else if (TagLib::ASF::File* file = dynamic_cast<TagLib::ASF::File*>(fr.file()))
		{
			if (file->tag()) {
				loopForTags(file->tag()->properties(), song);
			}
		}
		else if (TagLib::APE::File* file = dynamic_cast<TagLib::APE::File*>(fr.file()))
		{
			if (file->APETag())
			{
				loopForTags(file->APETag()->properties(), song);
			}
		}
		else if (TagLib::MPC::File* file = dynamic_cast<TagLib::MPC::File*>(fr.file()))
		{
			if (file->APETag())
			{
				loopForTags(file->APETag()->properties(), song);
			}
		}
		else if (TagLib::RIFF::WAV::File* file = dynamic_cast<TagLib::RIFF::WAV::File*>(fr.file())) {
			if (file->ID3v2Tag())
			{
				loopForTags(file->ID3v2Tag()->properties(), song);
			}
		}
		else if (TagLib::RIFF::AIFF::File* file = dynamic_cast<TagLib::RIFF::AIFF::File*>(fr.file())) {
			if (file->tag())
			{
				loopForTags(file->tag()->properties(), song);
			}
		}

		if (fr.tag() != NULL) {
			loopForTags(fr.tag()->properties(), song);
		}

		for (QJsonValueRef& sValue : libArray) {
			if (sValue.toObject().value("album") == song.value("album") && sValue.toObject().value("date") == song.value("date")) {

				if (sValue.toObject().value("albumartist") == song.value("albumartist")) { //if it has the same album artist, it's the same album
					newAlbum = false;
					song.insert("albumid", sValue.toObject().value("albumid"));
				}
				else if (newAlbum && sValue.toObject().value("artist") == song.value("artist")) {
					newAlbum = false;
					song.insert("albumid", sValue.toObject().value("albumid"));
				}
			}
		}

		if (newAlbum) { //previous part commented cuz it will fall here anyway - first ID will be 1 - IDs don't really matter, it's just a way of grouping albums, so I left it like this
			idNumber++;
			song.insert("albumid", idNumber);

			QImage albumCover; //preparing variable to recieve cover art

			CCover cover;

			if (cover.getCover(fr, albumCover, 300, 300, QFileInfo(fileDirString).canonicalPath())) {
				QString coverFileName = "react/canary-desktop/public/cache/covers/" + QString::number(idNumber) + ".jpg";

				albumCover.save(coverFileName, "JPG", 100);
			}
		}

		libArray.append(song);
	}

	QJsonDocument libDoc(libArray);

	QFile libFile("./userdata/library.json");
	bool cLib = libFile.open(QIODevice::ReadWrite);
	libFile.write(libDoc.toJson());
	//libFile.close();

	return cLib;
}

void Library::loopForTags(TagLib::PropertyMap sMap, QJsonObject& songObj) {
	for (auto it = sMap.begin(); it != sMap.end(); it++) {

		QJsonArray propertyValues;

		for (int i = 0; i < it->second.size(); i++) {
			QJsonValue val(QString::fromStdWString(it->second[i].toWString()));

			propertyValues.append(val);
		}

		songObj.insert(QString::fromStdWString(it->first.toWString()).toLower(), propertyValues);
	}
}

QVariantList Library::loadLib() {
	QFile libJSON("./userdata/library.json");
	bool readJSON = libJSON.open(QIODevice::ReadOnly | QIODevice::Text);
	QByteArray data = libJSON.readAll();
	QJsonDocument jdoc(QJsonDocument::fromJson(data));

	QVariantList listToPass = jdoc.array().toVariantList();

	return listToPass;
}

/*
getData
- switch album, loop, etc.
*/