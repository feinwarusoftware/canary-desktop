#include "library.h"

#include <QDebug>

CCover cover;

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

	if (!QDir("react/canary-desktop/public/cache").exists()) {
		QDir().mkdir("react/canary-desktop/public/cache");
	}

	if (!QDir("react/canary-desktop/public/cache/covers").exists()) {
		QDir().mkdir("react/canary-desktop/public/cache/covers");
	}

	createInfoList(listArray, libArray, 0);

	QJsonDocument libDoc(libArray);

	QFile libFile("./userdata/library.json");
	bool cLib = libFile.open(QIODevice::ReadWrite);
	libFile.write(libDoc.toJson());
	//libFile.close();

	return cLib;
}

void Library::createInfoList(QJsonArray fileList, QJsonArray& infoArray, int idNumber) {
	foreach(const QJsonValue & value, fileList) {
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

		for (QJsonValueRef& sValue : infoArray) {
			if (sValue.toObject().value("album") == song.value("album") && sValue.toObject().value("date") == song.value("date")) {

				if (sValue.toObject().value("albumartist") == song.value("albumartist")) { //if it has the same album artist, it's the same album
					newAlbum = false;
					song.insert("albumid", sValue.toObject().value("albumid"));
				}
				else if (newAlbum && sValue.toObject().value("artist") == song.value("artist")) { //TODO/test: "newAlbum &&" not necessary
					newAlbum = false;
					song.insert("albumid", sValue.toObject().value("albumid"));
				}
			}
		}

		if (newAlbum) { //previous part commented cuz it will fall here anyway - first ID will be 1 - IDs don't really matter, it's just a way of grouping albums, so I left it like this
			idNumber++;
			song.insert("albumid", idNumber);

			QImage albumCover; //preparing variable to recieve cover art

			if (cover.getCover(fr, albumCover, 300, 300, QFileInfo(fileDirString).canonicalPath())) {
				QString coverFileName = "react/canary-desktop/public/cache/covers/" + QString::number(idNumber) + ".jpg";

				albumCover.save(coverFileName, "JPG", 100);
			}
		}

		infoArray.append(song);
	}
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

void Library::updateLib(QStringList dirPath) {
	QFile fileListJson("./userdata/fileList.json");
	fileListJson.open(QIODevice::ReadOnly | QIODevice::Text);
	QJsonArray listArray = QJsonDocument::fromJson(fileListJson.readAll()).array();

	QJsonArray newLib, newItems, removedItems, newItemsInfo;

	for (QString& location : dirPath) { //looping for new items
		QDirIterator it(location, QDirIterator::Subdirectories);

		while (it.hasNext()) {
			QFileInfo fileInfo(it.next());
			if (fileInfo.isFile() && supportedFormats.indexOf(fileInfo.suffix()) != -1) { //if it's a file / if it's a supported file
				if (!listArray.contains(fileInfo.absoluteFilePath())) {
					newItems.append(fileInfo.absoluteFilePath());
				}

				newLib.append(fileInfo.absoluteFilePath());
			}
		}
	}

	for (QJsonValue item : listArray) {
		if (!newLib.contains(item)) {
			removedItems.append(item);
		}
	}

	fileListJson.close();

	int biggestAlbumId = 0;

	QFile libraryJSONfile("./userdata/library.json");
	libraryJSONfile.open(QIODevice::ReadOnly | QIODevice::Text);
	QJsonArray libraryArray = QJsonDocument::fromJson(libraryJSONfile.readAll()).array();

	for (QJsonValue v : libraryArray) {
		int albumid = v.toObject().value("albumid").toInt();
		if (albumid > biggestAlbumId) {
			biggestAlbumId = albumid;
		}
	}

	createInfoList(newItems, newItemsInfo, biggestAlbumId);

	QJsonArray::iterator lit;

	for (lit = libraryArray.begin(); lit != libraryArray.end(); lit++) {
		QJsonValue v(*lit);
		QJsonObject o(v.toObject());
		if (removedItems.contains(o.value("dir"))) {
			libraryArray.erase(lit);
		}
		//if album exists
		QJsonArray::iterator nil;
		for (nil = newItemsInfo.begin(); nil != newItemsInfo.end(); nil++) {
			QJsonValue nv(*nil);
			QJsonObject no(nv.toObject());
			if (no.value("album") == o.value("album") && no.value("date") == o.value("date") && no.value("albumid") != o.value("albumid")) {
				if (no.value("albumartist") == o.value("albumartist") || no.value("artist") == o.value("artist")) {
					QString coverToRemove = "react/canary-desktop/public/cache/covers/" + QString::number(no["albumid"].toInt()) + ".jpg";
					QFile toRemove(coverToRemove);
					toRemove.remove();
					no["albumid"] = o["albumid"]; //change value of temp object
					newItemsInfo.erase(nil);
					newItemsInfo.append(no);
				}
			}
		}
	}

	for (QJsonValue item : newItemsInfo) { //TODO: try concat both arrays
		libraryArray.append(item);
	}

	libraryJSONfile.close();

	libraryJSONfile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	QJsonDocument libJSON(libraryArray);
	libraryJSONfile.write(libJSON.toJson());
	libraryJSONfile.close();

	fileListJson.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	QJsonDocument jsonDoc(newLib);
	fileListJson.write(jsonDoc.toJson());
	fileListJson.close();
}

void Library::updateSong(int pos, QJsonObject song) {
	QFile libraryJSONfile("./userdata/library.json");
	libraryJSONfile.open(QIODevice::ReadOnly | QIODevice::Text);
	QJsonArray libraryArray = QJsonDocument::fromJson(libraryJSONfile.readAll()).array();

	libraryArray[pos] = song;

	libraryJSONfile.close();

	libraryJSONfile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	QJsonDocument libJSON(libraryArray);
	libraryJSONfile.write(libJSON.toJson());
	libraryJSONfile.close();
}
