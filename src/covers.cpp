/* Copyright (C) 2011 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include "covers.h"

#include <QDebug>

/*QString coverFileNames[2] = {"cover", "folder" };
QString coverFileFormats[2] = { "jpg", "png" };*/
QString coverFileTypes[4] = {
	"cover.png",
	"cover.jpg",
	"folder.png",
	"folder.jpg"
};

CCover::CCover(QObject* parent) : QObject(parent) {

}

bool drawCover(const TagLib::ByteVector& data, QImage& imgObj, int w, int h) { //TODO LATER: adapt this to receive QImages for manipulation later
	QImage image;

	bool imageLoad = image.loadFromData((const uchar*)data.data(), data.size());

	QImage resized = image.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	imgObj = resized;

	return imageLoad;
}

//TRANSFORMAR EM VERSÃO QIMAGE -> PASSAR PRA BASE64 NO PLAYER PQ ASSIM DÁ PRA APROVEITAR A FUNÇÃO PRA FAZER OS JPEG

//UPDATE -> GETGOVER / INJECTOVER / DRAWCOVER

/*
(comment in the previous line)
changes may be done here in the future to contemplate the pre-saving (caching) of un-resized QImages.
basically we would cache the cover of the album like we cache the metadata, to display it as a miniature and then bigger on the display, when it's played.
we would cache the QImage to we wouldn't need to extract it twice - only resize it in different sizes. since it's not being used right now (cover is only being used in album and current song)
not changes were made to the code yet
Personal note: I personally prefer to not use thumbnails at all. Too much RAM usage and visual polution, with really no great advantage to counter. I'd go for a "more Spotify" approach.
My suggested approcah by now, as I was about to create a info caching system is: to not have a live caching system, but rather use the information stored in Canary's libraries.
*/

//TODO/WARNING: RAM usage may (pretty sure it does) vary according to image size. Resize *before* inserting to QML recommended.

bool ExtractAPE(TagLib::APE::Tag* tag, QImage& target, int w, int h)
{
	const TagLib::APE::ItemListMap& listMap = tag->itemListMap();
	if (listMap.contains("COVER ART (FRONT)"))
	{
		const TagLib::ByteVector nullStringTerminator(1, 0);
		TagLib::ByteVector item = listMap["COVER ART (FRONT)"].value();
		const int pos = item.find(nullStringTerminator);	// Skip the filename.
		if (pos != -1)
		{
			const TagLib::ByteVector& pic = item.mid(pos + 1);
			return drawCover(pic, target, w, h);
		}
	}

	return false;
}

bool ExtractID3(TagLib::ID3v2::Tag* tag, QImage& target, int w, int h)
{
	const TagLib::ID3v2::FrameList& frameList = tag->frameList("APIC");
	if (!frameList.isEmpty())
	{
		// Just grab the first image.
		const auto* frame = (TagLib::ID3v2::AttachedPictureFrame*)frameList.front();
		return drawCover(frame->picture(), target, w, h);
	}

	return false;
}

bool ExtractFLAC(TagLib::FLAC::File* file, QImage& target, int w, int h)
{
	const TagLib::List<TagLib::FLAC::Picture*>& picList = file->pictureList();
	if (!picList.isEmpty())
	{
		// Just grab the first image.
		const TagLib::FLAC::Picture* pic = picList[0];
		return drawCover(pic->data(), target, w, h);
	}

	return false;
}

bool ExtractASF(TagLib::ASF::File* file, QImage& target, int w, int h)
{
	const TagLib::ASF::AttributeListMap& attrListMap = file->tag()->attributeListMap();
	if (attrListMap.contains("WM/Picture"))
	{
		const TagLib::ASF::AttributeList& attrList = attrListMap["WM/Picture"];
		if (!attrList.isEmpty())
		{
			// Let's grab the first cover. TODO: Check/loop for correct type.
			const TagLib::ASF::Picture& wmpic = attrList[0].toPicture();
			if (wmpic.isValid())
			{
				return drawCover(wmpic.picture(), target, w, h);
			}
		}
	}

	return false;
}

bool ExtractMP4(TagLib::MP4::File* file, QImage& target, int w, int h)
{
	TagLib::MP4::Tag* tag = file->tag();
	const TagLib::MP4::ItemListMap& itemListMap = tag->itemListMap();
	if (itemListMap.contains("covr"))
	{
		const TagLib::MP4::CoverArtList& coverArtList = itemListMap["covr"].toCoverArtList();
		if (!coverArtList.isEmpty())
		{
			const TagLib::MP4::CoverArt* pic = &(coverArtList.front());
			return drawCover(pic->data(), target, w, h);
		}
	}

	return false;
}


//TODO: WHEN NO COVER TRY COVER.JPG OR FOLDER.JPG / PNG
bool CCover::getCover(const TagLib::FileRef& fr, QImage& target, int w, int h, QString fileDir) {
	bool found = false;

	if (TagLib::MPEG::File* file = dynamic_cast<TagLib::MPEG::File*>(fr.file()))
	{
		if (file->ID3v2Tag())
		{
			found = ExtractID3(file->ID3v2Tag(), target, w, h);
		}
		if (!found && file->APETag())
		{
			found = ExtractAPE(file->APETag(), target, w, h);
		}
	}
	else if (TagLib::FLAC::File* file = dynamic_cast<TagLib::FLAC::File*>(fr.file()))
	{
		found = ExtractFLAC(file, target, w, h);

		if (!found && file->ID3v2Tag())
		{
			found = ExtractID3(file->ID3v2Tag(), target, w, h);
		}
	}
	else if (TagLib::MP4::File* file = dynamic_cast<TagLib::MP4::File*>(fr.file()))
	{
		found = ExtractMP4(file, target, w, h);
	}
	else if (TagLib::ASF::File* file = dynamic_cast<TagLib::ASF::File*>(fr.file()))
	{
		found = ExtractASF(file, target, w, h);
	}
	else if (TagLib::APE::File* file = dynamic_cast<TagLib::APE::File*>(fr.file()))
	{
		if (file->APETag())
		{
			found = ExtractAPE(file->APETag(), target, w, h);
		}
	}
	else if (TagLib::MPC::File* file = dynamic_cast<TagLib::MPC::File*>(fr.file()))
	{
		if (file->APETag())
		{
			found = ExtractAPE(file->APETag(), target, w, h);
		}
	}
	else if (TagLib::RIFF::WAV::File* file = dynamic_cast<TagLib::RIFF::WAV::File*>(fr.file())) {
		//this kind of file has ID3v2 tags https://taglib.org/api/classTagLib_1_1RIFF_1_1WAV_1_1File.html#a2bca63e227b0c2fa6cdd0c181360de96
		if (!found && file->ID3v2Tag())
		{
			found = ExtractID3(file->ID3v2Tag(), target, w, h);
		}
	}
	else if (TagLib::RIFF::AIFF::File* file = dynamic_cast<TagLib::RIFF::AIFF::File*>(fr.file())) {
		//this kind of file also has ID3v2 tags https://taglib.org/api/classTagLib_1_1RIFF_1_1AIFF_1_1File.html#add1a0d200c2356eb08c76057cdc54312
		//does this work or needs verification? note that ->tag() returns a pointer regardless of it having real tags or not
		if (!found && file->tag())
		{
			found = ExtractID3(file->tag(), target, w, h);
		}
	}

	if (!found) {
		for (int i = 0; i < 4; ++i) {
			if (QFile(fileDir + "/" + coverFileTypes[i]).exists()) {
				found = true;
				QImage img(fileDir + "/" + coverFileTypes[i]);
				QImage resized = img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				target = resized;
			}
		}
	}

	return found;
}