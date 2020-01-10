#include "library.h"

#include <QDebug>

Library::Library(QObject* parent) : QObject(parent) {

}

QList<QString> supportedFormats = {"mp3", "ogg", "wav", "aiff", "flac", "wv", "opus", "m4a", "caf", "ape", "aac", "ac3", "tta"};

bool Library::searchDir(QString dirPath) {
	QJsonArray jsonArr;

	QDirIterator it(dirPath, QDirIterator::Subdirectories);

	while (it.hasNext()) {
		//qDebug() << it.next();

		QFileInfo fileInfo(it.next());
		if (fileInfo.isFile() && supportedFormats.indexOf(fileInfo.suffix()) != -1) { //if it's a file / if it's a supported file
			//qDebug() << fileInfo.absoluteFilePath();
			jsonArr.append(fileInfo.absoluteFilePath());
		}
	}

	QFile jsonFile("./userdata/fileList.json");
	bool createJSON = jsonFile.open(QIODevice::ReadWrite);
	QJsonDocument jsonDoc(jsonArr);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();

	return createJSON;
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

bool Library::createAlbumLib(QJsonArray data) { //TODO: mix this with the album function?
	QJsonArray loadedAlbums;
	QJsonObject albumObject;

	foreach(const QJsonValue & songVal, data) {
		QJsonObject song = songVal.toObject();

		if (loadedAlbums.contains(song.value("albumid"))) { //song from an album that's already registered
			QString id = QString::number(song.value("albumid").toInt());
			QJsonObject tempObj = albumObject.value(id).toObject();
			QJsonArray tracks = tempObj.value("tracks").toArray();

			song.remove("artist");
			song.remove("date");
			song.remove("album");

			tracks.append(song);

			tempObj["tracks"] = tracks;

			albumObject.remove(id);
			albumObject.insert(id, tempObj);

			continue;
		}

		loadedAlbums.append(song.value("albumid"));

		QJsonObject album;

		if (song.value("album").isArray() && !song.value("album").toArray().isEmpty()) {
			album.insert("name", song.value("album").toArray()[0].toString());
		}
		else {
			album.insert("name", "");
		}

		if (song.value("artist").isArray() && !song.value("artist").toArray().isEmpty()) {
			album.insert("artist", song.value("artist").toArray()[0].toString());
		}
		else {
			album.insert("artist", "");
		}

		if (song.value("date").isArray() && !song.value("date").toArray().isEmpty()) {
			album.insert("date", song.value("date").toArray()[0].toString());
		}
		else {
			album.insert("date", "");
		}

		QJsonArray tracks;

		song.remove("artist");
		song.remove("date");
		song.remove("album");

		tracks.append(song);

		album.insert("tracks", tracks);

		albumObject.insert(QString::number(song.value("albumid").toInt()), album);
	}

	qDebug() << albumObject;

	QJsonDocument albumDoc(albumObject);

	QFile albumFile("./userdata/albums.json");
	bool createAlbumFile = albumFile.open(QIODevice::ReadWrite);
	albumFile.write(albumDoc.toJson());

	return createAlbumFile;
}

bool Library::createLib() {
	QFile fileListJson("./userdata/fileList.json");
	bool readJSON = fileListJson.open(QIODevice::ReadOnly/* | QIODevice::Text*/);
	QJsonDocument list = QJsonDocument::fromJson(fileListJson.readAll());
	QJsonArray listArray = list.array();

	QJsonArray libArray;

	int idNumber = 0;

	foreach(const QJsonValue & value, listArray) {
		QString fileDirString = value.toString();
		QByteArray fileName = QFile::encodeName(fileDirString); //using QFile to encode dir so it's readable to TagLib
		const char* encodedFileDir = fileName.constData();

		QJsonObject song;

		TagLib::FileRef fr(encodedFileDir);

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

		for (int u = 0; u < libArray.size(); u++) {
			if (libArray[u].toObject().value("album") == song.value("album") && libArray[u].toObject().value("date") == song.value("date")) {
				
				if (libArray[u].toObject().value("albumartist") == song.value("albumartist")) { //if it has the same album artist, it's the same album
					newAlbum = false;
					song.insert("albumid", libArray[u].toObject().value("albumid"));
				}
				
				if (newAlbum && libArray[u].toObject().value("artist") == song.value("artist")) { //elseif?
					newAlbum = false;
					song.insert("albumid", libArray[u].toObject().value("albumid"));
				}
			}
		}

		if (newAlbum) { //previous part commented cuz it will fall here anyway - first ID will be 1 - IDs don't really matter, it's just a way of grouping albums, so I left it like this
			idNumber++;
			song.insert("albumid", idNumber);

			if (!QDir("cache").exists()) {
				QDir().mkdir("cache");
			}

			if (!QDir("cache/covers").exists()) {
				QDir().mkdir("cache/covers");
			}

			QImage albumCover; //preparing variable to recieve cover art

			CCover cover;

			cover.getCover(fr, albumCover, 300, 300);

			QString coverFileName = "cache/covers/" + QString::number(idNumber) + ".png";

			albumCover.save(coverFileName, "PNG", 100);
		}

		libArray.append(song);
	}

	QJsonDocument libDoc(libArray);

	QFile libFile("./userdata/library.json");
	bool createLib = libFile.open(QIODevice::ReadWrite);
	libFile.write(libDoc.toJson());
	//libFile.close();

	return createLib;
}

//ADITIONAL DATA FUNCTIONS
QString Library::getData(QString fileName) {
	QString fileDir = "./userdata/" + fileName + ".json";
	QFile libJSON(fileDir);
	bool readJSON = libJSON.open(QIODevice::ReadOnly | QIODevice::Text);

	if (!readJSON) {
		return "err";
	}

	//qDebug() << QString::fromUtf8(libJSON.readAll().data());

	return QString::fromUtf8(libJSON.readAll().data());
}

QString Library::getAppLocation() {
	return QDir::current().absolutePath();
}