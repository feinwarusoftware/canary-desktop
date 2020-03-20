/*
	<one line to give the library's name and an idea of what it does.>
	Copyright (C) 2012  Vishesh Handa <me@vhanda.in>

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "taglibextractor.h"

// Taglib includes
#include <taglib/taglib.h>
#include <taglib/tag.h>
#include <taglib/tfilestream.h>
#include <taglib/tpropertymap.h>
#include <taglib/aifffile.h>
#include <taglib/apefile.h>
#include <taglib/asffile.h>
#include <taglib/flacfile.h>
#include <taglib/mp4file.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/opusfile.h>
#include <taglib/speexfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/wavfile.h>
#include <taglib/wavpackfile.h>
#include <taglib/asftag.h>
#include <taglib/asfattribute.h>
#include <taglib/id3v2tag.h>
#include <taglib/mp4tag.h>
#include <taglib/popularimeterframe.h>

#include <QDebug>

TagLibExtractor::TagLibExtractor(QObject* parent) : QObject(parent) {

}

const QStringList supportedMimeTypes = {
	  QStringLiteral("audio/flac"),
		QStringLiteral("audio/mp4"),
		QStringLiteral("audio/mpeg"),
		QStringLiteral("audio/mpeg3"),
		QStringLiteral("audio/ogg"),
		QStringLiteral("audio/opus"),
		QStringLiteral("audio/speex"),
		QStringLiteral("audio/wav"),
		QStringLiteral("audio/x-aiff"),
		QStringLiteral("audio/x-aifc"),
		QStringLiteral("audio/x-ape"),
		QStringLiteral("audio/x-mpeg"),
		QStringLiteral("audio/x-ms-wma"),
		QStringLiteral("audio/x-musepack"),
		QStringLiteral("audio/x-opus+ogg"),
		QStringLiteral("audio/x-speex+ogg"),
		QStringLiteral("audio/x-vorbis+ogg"),
		QStringLiteral("audio/x-wav"),
		QStringLiteral("audio/x-wavpack"),
};


void extractAudioProperties(TagLib::File* file, QJsonObject& song)
{
	TagLib::AudioProperties* audioProp = file->audioProperties();
	if (audioProp) {
		if (audioProp->length()) {
			// What about the xml duration?
			//result->add(Property::Duration, audioProp->length());
			song.insert("length", audioProp->length());
		}

		if (audioProp->lengthInSeconds()) {
			song.insert("lengthInSeconds", audioProp->lengthInSeconds());
		}

		if (audioProp->bitrate()) {
			//result->add(Property::BitRate, audioProp->bitrate() * 1000);
			song.insert("bitrate", audioProp->bitrate());
		}

		if (audioProp->channels()) {
			//result->add(Property::Channels, audioProp->channels());
			song.insert("channels", audioProp->channels());
		}

		if (audioProp->sampleRate()) {
			//result->add(Property::SampleRate, audioProp->sampleRate());
			song.insert("sampleRate", audioProp->sampleRate());
		}
	}
}

QJsonArray convertList(TagLib::StringList list) {
	QJsonArray a;
	for (TagLib::String& s : list) {
		a.append(QString::fromStdWString(s.toWString()));
	}

	return a;
}

QString convertString(TagLib::StringList list) {
	return QString::fromStdWString(list[0].toWString());
}

void readGenericProperties(const TagLib::PropertyMap& savedProperties, QJsonObject& song)
{
	if (savedProperties.isEmpty()) {
		return;
	}
	if (savedProperties.contains("TITLE")) {
		//result->add(Property::Title, TStringToQString(savedProperties["TITLE"].toString()).trimmed());
		//song.insert("title", convertList(savedProperties["TITLE"]));
		song.insert("title", convertString(savedProperties["TITLE"]));
	}
	if (savedProperties.contains("ALBUM")) {
		//result->add(Property::Album, TStringToQString(savedProperties["ALBUM"].toString()).trimmed());
		//song.insert("album", convertList(savedProperties["ALBUM"]));
		song.insert("album", convertString(savedProperties["ALBUM"]));
	}
	if (savedProperties.contains("COMMENT")) {
		//result->add(Property::Comment, TStringToQString(savedProperties["COMMENT"].toString()).trimmed());
		song.insert("comment", convertString(savedProperties["COMMENT"]));
	}
	if (savedProperties.contains("TRACKNUMBER")) {
		//result->add(Property::TrackNumber, savedProperties["TRACKNUMBER"].toString().toInt());
		song.insert("tracknumber", savedProperties["TRACKNUMBER"].toString().toInt());
	}
	if (savedProperties.contains("DATE")) {
		//result->add(Property::ReleaseYear, savedProperties["DATE"].toString().toInt());
		song.insert("date", convertList(savedProperties["DATE"]));
	}
	if (savedProperties.contains("OPUS")) {
		//result->add(Property::Opus, savedProperties["OPUS"].toString().toInt());
		song.insert("opus", savedProperties["OPUS"].toString().toInt());
	}
	if (savedProperties.contains("DISCNUMBER")) {
		//result->add(Property::DiscNumber, savedProperties["DISCNUMBER"].toString().toInt());
		song.insert("discnumber", savedProperties["DISCNUMBER"].toString().toInt());
	}
	if (savedProperties.contains("RATING")) {
		/*
		 * There is no standard regarding ratings. Mimic MediaMonkey's behavior
		 * with a range of 0 to 100 (stored in steps of 10) and make it compatible
		 * with baloo rating with a range from 0 to 10
		 */
		//result->add(Property::Rating, savedProperties["RATING"].toString().toInt() / 10);
		song.insert("rating", savedProperties["RATING"].toString().toInt() / 10);
	}
	if (savedProperties.contains("LOCATION")) {
		//result->add(Property::Location, TStringToQString(savedProperties["LOCATION"].toString()).trimmed());
		song.insert("location", convertString(savedProperties["LOCATION"]));
	}
	if (savedProperties.contains("LANGUAGE")) {
		//result->add(Property::Language, TStringToQString(savedProperties["LANGUAGE"].toString()).trimmed());
		song.insert("language", convertString(savedProperties["LANGUAGE"]));
	}
	if (savedProperties.contains("LICENSE")) {
		//result->add(Property::License, TStringToQString(savedProperties["LICENSE"].toString()).trimmed());
		song.insert("license", convertString(savedProperties["LICENSE"]));
	}
	if (savedProperties.contains("PUBLISHER")) {
		//result->add(Property::Publisher, TStringToQString(savedProperties["PUBLISHER"].toString()).trimmed());
		song.insert("publisher", convertString(savedProperties["PUBLISHER"]));
	}
	if (savedProperties.contains("COPYRIGHT")) {
		//result->add(Property::Copyright, TStringToQString(savedProperties["COPYRIGHT"].toString()).trimmed());
		song.insert("copyright", convertString(savedProperties["COPYRIGHT"]));
	}
	if (savedProperties.contains("LABEL")) {
		//result->add(Property::Label, TStringToQString(savedProperties["LABEL"].toString()).trimmed());
		song.insert("label", convertString(savedProperties["LABEL"]));
	}
	if (savedProperties.contains("ENSEMBLE")) {
		//result->add(Property::Ensemble, TStringToQString(savedProperties["ENSEMBLE"].toString()).trimmed());
		song.insert("ensemble", convertString(savedProperties["ENSEMBLE"]));
	}
	if (savedProperties.contains("COMPILATION")) {
		//result->add(Property::Compilation, TStringToQString(savedProperties["COMPILATION"].toString()).trimmed());
		song.insert("compilation", convertString(savedProperties["COMPILATION"]));
	}
	if (savedProperties.contains("LYRICS")) {
		//result->add(Property::Lyrics, TStringToQString(savedProperties["LYRICS"].toString()).trimmed());
		song.insert("lyrics", convertString(savedProperties["LYRICS"]));
	}
	if (savedProperties.contains("ARTIST")) {
		song.insert("artist", convertList(savedProperties["ARTIST"]));
	}
	if (savedProperties.contains("GENRE")) {
		song.insert("genre", convertList(savedProperties["GENRE"]));
	}
	if (savedProperties.contains("ALBUMARTIST")) {
		song.insert("albumartist", convertList(savedProperties["ALBUMARTIST"]));
	}
	if (savedProperties.contains("COMPOSER")) {
		song.insert("composer", convertList(savedProperties["COMPOSER"]));
	}
	if (savedProperties.contains("LYRICIST")) {
		/*const auto lyricists = savedProperties["LYRICIST"];
		for (const auto& lyricist : lyricists) {
			result->add(Property::Lyricist, TStringToQString(lyricist).trimmed());
		}*/
		song.insert("lyricist", convertList(savedProperties["LYRICIST"]));
	}
	if (savedProperties.contains("CONDUCTOR")) {
		/*const auto conductors = savedProperties["CONDUCTOR"];
		for (const auto& conductor : conductors) {
			result->add(Property::Conductor, TStringToQString(conductor).trimmed());
		}*/
		song.insert("conductor", convertList(savedProperties["CONDUCTOR"]));
	}
	if (savedProperties.contains("ARRANGER")) {
		/*const auto arrangers = savedProperties["ARRANGER"];
		for (const auto& arranger : arrangers) {
			result->add(Property::Arranger, TStringToQString(arranger).trimmed());
		}*/
		song.insert("arranger", convertList(savedProperties["ARRANGER"]));
	}
	if (savedProperties.contains("PERFORMER")) {
		/*const auto performers = savedProperties["PERFORMER"];
		for (const auto& performer : performers) {
			result->add(Property::Performer, TStringToQString(performer).trimmed());
		}*/
		song.insert("performer", convertList(savedProperties["PERFORMER"]));
	}
	if (savedProperties.contains("AUTHOR")) {
		/*const auto authors = savedProperties["AUTHOR"];
		for (const auto& author : authors) {
			result->add(Property::Author, TStringToQString(author).trimmed());
		}*/
		song.insert("author", convertList(savedProperties["AUTHOR"]));
	}

	if (savedProperties.contains("REPLAYGAIN_TRACK_GAIN")) {
		QString trackGainString = QString::fromStdWString(savedProperties["REPLAYGAIN_TRACK_GAIN"].toString(";").toWString()).trimmed();
		// remove " dB" suffix
		if (trackGainString.endsWith(QStringLiteral(" dB"), Qt::CaseInsensitive)) {
			trackGainString.chop(3);
		}
		bool success = false;
		double replayGainTrackGain = trackGainString.toDouble(&success);
		if (success) {
			song.insert("ReplayGainTrackGain", replayGainTrackGain);
		}
	}
	if (savedProperties.contains("REPLAYGAIN_ALBUM_GAIN")) {
		QString albumGainString = QString::fromStdWString(savedProperties["REPLAYGAIN_ALBUM_GAIN"].toString(";").toWString()).trimmed();
		// remove " dB" suffix
		if (albumGainString.endsWith(QStringLiteral(" dB"), Qt::CaseInsensitive)) {
			albumGainString.chop(3);
		}
		bool success = false;
		double replayGainAlbumGain = albumGainString.toDouble(&success);
		if (success) {
			song.insert("ReplayGainAlbumGain", replayGainAlbumGain);
		}
	}
	if (savedProperties.contains("REPLAYGAIN_TRACK_PEAK")) {
		QString trackPeakString = QString::fromStdWString(savedProperties["REPLAYGAIN_TRACK_PEAK"].toString(";").toWString()).trimmed();
		bool success = false;
		double replayGainTrackPeak = trackPeakString.toDouble(&success);
		if (success) {
			song.insert("ReplayGainTrackPeak", replayGainTrackPeak);
		}
	}
	if (savedProperties.contains("REPLAYGAIN_ALBUM_PEAK")) {
		QString albumPeakString = QString::fromStdWString(savedProperties["REPLAYGAIN_ALBUM_PEAK"].toString(";").toWString()).trimmed();
		bool success = false;
		double replayGainAlbumPeak = albumPeakString.toDouble(&success);
		if (success) {
			song.insert("ReplayGainAlbumPeak", replayGainAlbumPeak);
		}
	}
}

void extractId3Tags(TagLib::ID3v2::Tag* Id3Tags, QJsonObject& song)
{
	if (Id3Tags->isEmpty()) {
		return;
	}
	TagLib::ID3v2::FrameList lstID3v2;
	QJsonObject id3;

	/*
	 * Publisher.
	 * Special handling because TagLib::PropertyMap maps "TPUB" to "LABEL"
	 * Insert manually for Publisher.
	 */
	lstID3v2 = Id3Tags->frameListMap()["TPUB"];
	if (!lstID3v2.isEmpty()) {
		//result->add(Property::Publisher, TStringToQString(lstID3v2.front()->toString()));
		id3.insert("publisher", convertString(lstID3v2.front()->toString()));
	}

	// Compilation.
	lstID3v2 = Id3Tags->frameListMap()["TCMP"];
	if (!lstID3v2.isEmpty()) {
		//result->add(Property::Compilation, TStringToQString(lstID3v2.front()->toString()));
		id3.insert("compilation", convertString(lstID3v2.front()->toString()));
	}

	/*
	 * Rating.
	 * There is no standard regarding ratings. Most of the implementations match
	 * a 5 stars rating to a range of 0-255 for MP3.
	 * Map it to baloo rating with a range of 0 - 10.
	 */
	lstID3v2 = Id3Tags->frameListMap()["POPM"];
	if (!lstID3v2.isEmpty()) {
		TagLib::ID3v2::PopularimeterFrame* ratingFrame = static_cast<TagLib::ID3v2::PopularimeterFrame*>(lstID3v2.front());
		int rating = ratingFrame->rating();
		if (rating == 0) {
			rating = 0;
		}
		else if (rating == 1) {
			TagLib::String ratingProvider = ratingFrame->email();
			if (ratingProvider == "no@email" || ratingProvider == "org.kde.kfilemetadata") {
				rating = 1;
			}
			else {
				rating = 2;
			}
		}
		else if (rating >= 1 && rating <= 255) {
			rating = static_cast<int>(0.032 * rating + 2);
		}
		//result->add(Property::Rating, rating);
		id3.insert("rating", rating);
	}

	song.insert("id3", id3);
}

void extractMp4Tags(TagLib::MP4::Tag* mp4Tags, QJsonObject& song)
{
	if (mp4Tags->isEmpty()) {
		return;
	}
	TagLib::MP4::ItemListMap allTags = mp4Tags->itemListMap();
	QJsonObject mp4;

	/*
	 * There is no standard regarding ratings. Mimic MediaMonkey's behavior
	 * with a range of 0 to 100 (stored in steps of 10) and make it compatible
	 * with baloo rating with a range from 0 to 10.
	 */
	TagLib::MP4::ItemListMap::Iterator itRating = allTags.find("rate");
	if (itRating != allTags.end()) {
		//result->add(Property::Rating, itRating->second.toStringList().toString().toInt() / 10);
		mp4.insert("rating", itRating->second.toStringList().toString().toInt() / 10);
	}

	song.insert("mp4", mp4);
}

void extractAsfTags(TagLib::ASF::Tag* asfTags, QJsonObject& song)
{
	if (asfTags->isEmpty()) {
		return;
	}

	TagLib::ASF::AttributeList lstASF = asfTags->attribute("WM/SharedUserRating");
	QJsonObject asf;
	if (!lstASF.isEmpty()) {
		int rating = lstASF.front().toString().toInt();
		/*
		 * Map the rating values of WMP to Baloo rating.
		 * 0->0, 1->2, 25->4, 50->6, 75->8, 99->10
		 */
		if (rating == 0) {
			rating = 0;
		}
		else if (rating == 1) {
			rating = 2;
		}
		else {
			rating = static_cast<int>(0.09 * rating + 2);
		}
		//result->add(Property::Rating, rating);
		asf.insert("rating", rating);
	}

	lstASF = asfTags->attribute("Author");
	if (!lstASF.isEmpty()) {
		const auto attribute = lstASF.front();
		//result->add(Property::Author, TStringToQString(attribute.toString()).trimmed());
		asf.insert("author", QString::fromStdWString((attribute.toString().toWString())));
	}

	// Lyricist is called "WRITER" for wma/asf files
	lstASF = asfTags->attribute("WM/Writer");
	if (!lstASF.isEmpty()) {
		const auto attribute = lstASF.front();
		//result->add(Property::Lyricist, TStringToQString(attribute.toString()).trimmed());
		asf.insert("lyricist", QString::fromStdWString((attribute.toString().toWString())));
	}

	/*
	 * TagLib exports "WM/PUBLISHER" as "LABEL" in the PropertyMap,
	 * add it manually to Publisher.
	 */
	lstASF = asfTags->attribute("WM/Publisher");
	if (!lstASF.isEmpty()) {
		const auto attribute = lstASF.front();
		//result->add(Property::Publisher, TStringToQString(attribute.toString()).trimmed());
		asf.insert("publisher", QString::fromStdWString((attribute.toString().toWString())));
	}
}

void TagLibExtractor::extract(QJsonObject& song, QByteArray fileName, QMimeType type)
{
	const char* fn = fileName.toStdString().c_str();
	QString mimeType = type.name();

	qDebug() << mimeType;

	if (mimeType == QLatin1String("audio/mpeg") || mimeType == QLatin1String("audio/mpeg3") || mimeType == QLatin1String("audio/x-mpeg")) {
		TagLib::MPEG::File file(fn, TagLib::ID3v2::FrameFactory::instance(), true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
			if (file.hasID3v2Tag()) {
				extractId3Tags(file.ID3v2Tag(), song);
				//extract cover, return qimage, save qimage (nesse caso, outros mostrar por exemplo)
			}
		}
	}
	else if (mimeType == QLatin1String("audio/x-aiff") || mimeType == QLatin1String("audio/x-aifc")) {
		TagLib::RIFF::AIFF::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
			if (file.hasID3v2Tag()) {
				extractId3Tags(file.tag(), song);
			}
		}
	}
	else if (mimeType == QLatin1String("audio/wav") || mimeType == QLatin1String("audio/x-wav")) {
		TagLib::RIFF::WAV::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
			if (file.hasID3v2Tag()) {
				extractId3Tags(file.tag(), song);
			}
		}
	}
	else if (mimeType == QLatin1String("audio/x-musepack")) {
		TagLib::MPC::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/x-ape")) {
		TagLib::APE::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/x-wavpack")) {
		TagLib::WavPack::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/mp4")) {
		TagLib::MP4::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
			extractMp4Tags(file.tag(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/flac")) {
		TagLib::FLAC::File file(fn, TagLib::ID3v2::FrameFactory::instance(), true);
		if (file.isValid()) {
			//qDebug() << "é flac válido!";
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/ogg") || mimeType == QLatin1String("audio/x-vorbis+ogg")) {
		TagLib::Ogg::Vorbis::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/opus") || mimeType == QLatin1String("audio/x-opus+ogg")) {
		TagLib::Ogg::Opus::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/speex") || mimeType == QLatin1String("audio/x-speex+ogg")) {
		TagLib::Ogg::Speex::File file(fn, true);
		// Workaround for buggy taglib:
		// isValid() returns true for invalid files, but XiphComment* tag() returns a nullptr
		if (file.isValid() && file.tag()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
		}
	}
	else if (mimeType == QLatin1String("audio/x-ms-wma")) {
		TagLib::ASF::File file(fn, true);
		if (file.isValid()) {
			extractAudioProperties(&file, song);
			readGenericProperties(file.properties(), song);
			extractAsfTags(file.tag(), song);
		}
	}
}

// TAG information (incomplete).
// https://xiph.org/vorbis/doc/v-comment.html
// https://help.mp3tag.de/main_tags.html
// http://id3.org/
// https://www.legroom.net/2009/05/09/ogg-vorbis-and-flac-comment-field-recommendations
// https://kodi.wiki/view/Music_tagging#Tags_Kodi_reads
// https://wiki.hydrogenaud.io/index.php?title=Tag_Mapping
// https://picard.musicbrainz.org/docs/mappings/
// -- FLAC/OGG --
// Artist:          ARTIST, PERFORMER
// Album artist:    ALBUMARTIST
// Composer:        COMPOSER
// Lyricist:        LYRICIST
// Conductor:       CONDUCTOR
// Disc number:     DISCNUMBER
// Total discs:     TOTALDISCS, DISCTOTAL
// Track number:    TRACKNUMBER
// Total tracks:    TOTALTRACKS, TRACKTOTAL
// Genre: GENRE
// -- ID3v2 --
// Artist:                          TPE1
// Album artist:                    TPE2
// Composer:                        TCOM
// Lyricist:                        TEXT
// Conductor:                       TPE3
// Disc number[/total dics]:        TPOS
// Track number[/total tracks]:     TRCK
// Genre:                           TCON
