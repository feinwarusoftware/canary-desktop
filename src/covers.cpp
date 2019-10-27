/*The following code is an adaptation of the original Rainmeeter project one. It is shared under the same license.*/

/* Copyright (C) 2011 Rainmeter Project Developers
 *
 * This Source Code Form is subject to the terms of the GNU General Public
 * License; either version 2 of the License, or (at your option) any later
 * version. If a copy of the GPL was not distributed with this file, You can
 * obtain one at <https://www.gnu.org/licenses/gpl-2.0.html>. */

#include "covers.h"

CCover::CCover(QObject* parent) : QObject(parent) {

}

bool drawCover(const TagLib::ByteVector& data, QObject* coverContainer) { //TODO LATER: adapt this to receive QImages for manipulation later
	QImage image;

	image.loadFromData((const uchar*)data.data(), data.size());

	QImage resized = image.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	resized.save(&buffer, "JPG", 100); //100 = uncompressed / implicit conversion to JPG https://forum.qt.io/post/258486, whatever the original file format might've been

	QString uri = "data:image/jpg;base64," + QString::fromLatin1(ba.toBase64().data());

	return coverContainer->setProperty("source", uri);
}

//TODO/WARNING: RAM usage may (pretty sure it does) vary according to image size. Resize *before* inserting to QML recommended.

bool ExtractAPE(TagLib::APE::Tag* tag, QObject* target)
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
			return drawCover(pic, target);
		}
	}

	return false;
}

bool ExtractID3(TagLib::ID3v2::Tag* tag, QObject* target)
{
	const TagLib::ID3v2::FrameList& frameList = tag->frameList("APIC");
	if (!frameList.isEmpty())
	{
		// Just grab the first image.
		const auto* frame = (TagLib::ID3v2::AttachedPictureFrame*)frameList.front();
		return drawCover(frame->picture(), target);
	}

	return false;
}

bool ExtractFLAC(TagLib::FLAC::File* file, QObject* target)
{
	const TagLib::List<TagLib::FLAC::Picture*>& picList = file->pictureList();
	if (!picList.isEmpty())
	{
		// Just grab the first image.
		const TagLib::FLAC::Picture* pic = picList[0];
		return drawCover(pic->data(), target);
	}

	return false;
}

bool ExtractASF(TagLib::ASF::File* file, QObject* target)
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
				return drawCover(wmpic.picture(), target);
			}
		}
	}

	return false;
}

bool ExtractMP4(TagLib::MP4::File* file, QObject* target)
{
	TagLib::MP4::Tag* tag = file->tag();
	const TagLib::MP4::ItemListMap& itemListMap = tag->itemListMap();
	if (itemListMap.contains("covr"))
	{
		const TagLib::MP4::CoverArtList& coverArtList = itemListMap["covr"].toCoverArtList();
		if (!coverArtList.isEmpty())
		{
			const TagLib::MP4::CoverArt* pic = &(coverArtList.front());
			return drawCover(pic->data(), target);
		}
	}

	return false;
}

bool CCover::getCover(const TagLib::FileRef& fr, QObject* target) {
	bool found = false;

	if (TagLib::MPEG::File * file = dynamic_cast<TagLib::MPEG::File*>(fr.file()))
	{
		if (file->ID3v2Tag())
		{
			found = ExtractID3(file->ID3v2Tag(), target);
		}
		if (!found && file->APETag())
		{
			found = ExtractAPE(file->APETag(), target);
		}
	}
	else if (TagLib::FLAC::File * file = dynamic_cast<TagLib::FLAC::File*>(fr.file()))
	{
		found = ExtractFLAC(file, target);

		if (!found && file->ID3v2Tag())
		{
			found = ExtractID3(file->ID3v2Tag(), target);
		}
	}
	else if (TagLib::MP4::File * file = dynamic_cast<TagLib::MP4::File*>(fr.file()))
	{
		found = ExtractMP4(file, target);
	}
	else if (TagLib::ASF::File * file = dynamic_cast<TagLib::ASF::File*>(fr.file()))
	{
		found = ExtractASF(file, target);
	}
	else if (TagLib::APE::File * file = dynamic_cast<TagLib::APE::File*>(fr.file()))
	{
		if (file->APETag())
		{
			found = ExtractAPE(file->APETag(), target);
		}
	}
	else if (TagLib::MPC::File * file = dynamic_cast<TagLib::MPC::File*>(fr.file()))
	{
		if (file->APETag())
		{
			found = ExtractAPE(file->APETag(), target);
		}
	}

	return found;
}