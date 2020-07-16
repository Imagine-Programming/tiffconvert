#include "pch.h"
#include "WangAnnotationReader.hpp"

using namespace TiffWang::Tiff;

/*
	A little note on named blocks, a little sequence:

		1. global properties are encountered, these are applied to all new marks and never reset (but might be overwritten)
		2. a new mark (attributes) is encountered, the global properties are used to initialize the local properties for this mark
		   any previously defined local properties are set to undefined through flags.
		3. local properties are encountered, these are set to the local state (possibly overwriting other local properties)
		4. any number of global properties are encountered, but these do not affect (2)
		5. a new mark (attributes) is encountered, the previous mark is emitted using only the local properties
		   a repeat of (2) is done.
*/

/// <summary>
/// Construct a new WangAnnotationReader from an opened <see cref="TiffFile"/>, looking at a specific <see cref="TiffIfdEntry"/>.
/// </summary>
/// <param name="file">The opened Tiff file.</param>
/// <param name="tag">The eiStream/Wang tag to process.</param>
/// <exception cref="std::runtime_error">Thrown when the <paramref name="tag"/> is not an eiStream/Wang tag or when insufficient data is available to read.</exception>
WangAnnotationReader::WangAnnotationReader(TiffFile& file, const TiffIfdEntry& tag)
	: m_File(file), m_Tag(tag) {

	if (m_Tag.TagId != TiffTagId::TIFF_WANG_TAG || m_Tag.TagType != TiffTagType::BYTE)
		throw std::runtime_error("cannot process provided TiffIfdEntry as eiStream/WANG Annotation data");

	if (m_Tag.ValueCount == 0)
		throw std::runtime_error("cannot process provided TiffIfdEntry as eiStream/WANG Annotation data, entry holds no values");

	file.Read(tag, m_AnnotationData);
	m_Size = m_AnnotationData.size();
}

/// <summary>
/// Start reading the eiStream/Wang tag block.
/// </summary>
void WangAnnotationReader::Read() {
	TiffWangEntry		 entry;
	TiffWangNamedBlock	 globalNamedBlock, localNamedBlock;
	TiffWangMark		 currentMark;
	std::string			 blockName;

	Seek(4); /* skip reserved header we can't do anything with */

	bool is16BitMode = (Read<TiffWangIntegerMode>() == TiffWangIntegerMode::Intel16Bit);
	bool keepParsing = true;

	while (keepParsing) {
		if (!Read(entry))
			break;

		switch (entry.DataType) {
			case TiffWangDataType::GLOBAL_NAMED_BLOCK: /* default settings for newly created mark */
				if (!Read(globalNamedBlock) || !ProcessNamedBlock(globalNamedBlock, currentMark, is16BitMode, true)) 
					keepParsing = false;
				break;

			case TiffWangDataType::ATTRIBUTE_DATA: /* new mark */
				// invoke handler callback to notify of completed mark, if we have one already.
				if (currentMark.HasAttributes())
					EmitMark(currentMark);

				// copy the global settings to local, effectively resetting any previous locals first.
				// EmitMark should only work with local properties.
				currentMark.AssignGlobalToLocal();

				// read the new attributes to start a new mark.
				if (!Read(currentMark.Attributes())) {
					keepParsing = false;
					break;
				}

				// set the flag for attributes, so we know we found a mark on a next encounter.
				currentMark.SetAttributes();
				break;
			

			case TiffWangDataType::LOCAL_NAMED_BLOCK: /* overruling settings for preceeding mark */
				if (!Read(localNamedBlock) || !ProcessNamedBlock(localNamedBlock, currentMark, is16BitMode, false)) 
					keepParsing = false;
				break;
		}

		// keep parsing if we have not reached the end of the tag yet.
		keepParsing = keepParsing && (!Eof() && SizeLeft() >= 20);
	}

	// invoke render callbacks for last found mark, if we found any at all.
	if (currentMark.HasAttributes())
		EmitMark(currentMark);
}

/// <summary>
/// Set the event handler instance to use when processing marks, invoke this method before calling <see cref="Read"/>.
/// </summary>
/// <param name="h">A shared pointer to an implementation of <see cref="IWangAnnotationCallback"/>.</param>
void WangAnnotationReader::SetHandler(std::shared_ptr<IWangAnnotationCallback> h) {
	m_Handler = h;
}

/// <summary>
/// Emit a mark to the event handler.
/// </summary>
/// <param name="mark">A reference to the current <see cref="TiffWangMark"/> data.</param>
void WangAnnotationReader::EmitMark(TiffWangMark& mark) {
	const auto& attributes = mark.Attributes();

	switch (attributes.uType) {
		// Text (and optionally a rectangle)
		case OAIN_MARK_TYPE::AttachANote:
		case OAIN_MARK_TYPE::TextStamp:
		case OAIN_MARK_TYPE::TypedText:
		case OAIN_MARK_TYPE::TextFromFile: { /* TODO: implement separate function for TextFromFile, to be able to work with file references. */
			// no handler or mark is not visible?
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			// break if not text was encountered before
			if (!mark.AnySet({ TiffWangMarkSet::LocalAsciiTextSet, TiffWangMarkSet::LocalUnicodeTextSet }))
				break;

			auto color = mark.Attributes().rgbColor1;
			if (attributes.uType == OAIN_MARK_TYPE::AttachANote) { /* AttachANote requires a rectangle as well, rgbColor1 is now fill color and rgbColor2 is text color */
				m_Handler->RenderRect(attributes.lrBounds, attributes.rgbColor1, attributes.bHighlighting, attributes.bTransparent);
				color = attributes.rgbColor2;
			}

			if (mark.IsSet(TiffWangMarkSet::LocalAsciiTextSet)) {
				m_Handler->RenderText(mark.LocalAsciiText(), attributes.lrBounds, attributes.lfFont, mark.LocalText(), color);
			}  else if (mark.IsSet(TiffWangMarkSet::LocalUnicodeTextSet)) {
				m_Handler->RenderText(mark.LocalUnicodeText(), attributes.lrBounds, attributes.lfFont, mark.LocalText(), color);
			}

			break;
		}

		// Lines 
		case OAIN_MARK_TYPE::StraightLine: /* 2 entires in PointList */
		case OAIN_MARK_TYPE::FreehandLine: /* n entries in PointList, no idea why there is 2 types - distinction could've been made by comparing n to 2 */
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			if (!mark.IsSet(TiffWangMarkSet::LocalPointsSet))
				break;

			m_Handler->RenderLine(
				attributes.lrBounds, 
				mark.LocalPointList(), 
				attributes.rgbColor1, 
				attributes.uLineSize,
				attributes.bHighlighting,
				attributes.bTransparent);

			break;

		// Filled rectangle
		case OAIN_MARK_TYPE::FilledRectangle:
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			m_Handler->RenderRect(
				attributes.lrBounds,
				attributes.rgbColor1, 
				attributes.bHighlighting, 
				attributes.bTransparent);

			break;

		// Outlined rectangle 
		case OAIN_MARK_TYPE::HollowRectangle:
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			m_Handler->RenderOutlinedRect(
				attributes.lrBounds, 
				attributes.rgbColor1, 
				attributes.uLineSize,
				attributes.bHighlighting,
				attributes.bTransparent);

			break;

		// A render mask, not supported as it has to be loaded from a file. 
		case OAIN_MARK_TYPE::Form:
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			if (!mark.AnySet({ TiffWangMarkSet::LocalFilenameSet, TiffWangMarkSet::LocalRotationSet }))
				break;

			m_Handler->RenderMask(mark.LocalFileName(), attributes.lrBounds, mark.LocalRotation());

			break;

		case OAIN_MARK_TYPE::ImageReference:
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			if (!mark.AnySet({ TiffWangMarkSet::LocalFilenameSet, TiffWangMarkSet::LocalRotationSet }))
				break;

			m_Handler->RenderImageReference(
				mark.LocalFileName(),
				attributes.lrBounds,
				mark.LocalRotation(), 
				attributes.bHighlighting,
				attributes.bTransparent);

			break;

		case OAIN_MARK_TYPE::ImageEmbedded:
			if (m_Handler == nullptr || !attributes.bVisible)
				break;

			if (!mark.IsSet(TiffWangMarkSet::LocalDibInfoSet))
				break;

			if (!mark.IsSet(TiffWangMarkSet::LocalFilenameSet))
				mark.FileNameSetter(false) = "<unknown image name>";

			m_Handler->RenderImage(
				mark.LocalFileName(),
				attributes.lrBounds,
				mark.LocalRotation(), 
				mark.LocalDibInfo(),
				attributes.bHighlighting,
				attributes.bTransparent);

			break;

		case OAIN_MARK_TYPE::OCRRegion:
			// ignore.
			break;

		default:
			DBGPRINT("Unrecognized Attribute: %d\n", attributes.uType);
			break;
	}
}

/// <summary>
/// Process a eiStream/Wang named block. This method will process the global or local settings and properties that come
/// by in the stream. Upon encountering new attributes, the previous attributes combined with all these settings and 
/// properties will be emitted to the event handler as an annotation mark.
/// </summary>
/// <param name="block">The named block struct.</param>
/// <param name="mark">The current state of the mark.</param>
/// <param name="is16bit">Whether or not we're reading in 16-bit mode.</param>
/// <param name="isGlobal">Is this a global or local named block.</param>
/// <returns>True is returned when processing this named block succeeded.</returns>
[[nodiscard]] bool WangAnnotationReader::ProcessNamedBlock(TiffWangNamedBlock& block, TiffWangMark& mark, bool is16bit, bool isGlobal) {
	auto next = static_cast<std::streamoff>(m_Offset) + block.Size + (is16bit ? 4 : 0);

	std::string name(block.Name, strnlen_s(block.Name, 8));

	if (name == "OiAnoDat") {
		if (mark.HasAttributes()) {
			switch (mark.Attributes().uType) {
				case OAIN_MARK_TYPE::FreehandLine:		/* AN_POINTS */
				case OAIN_MARK_TYPE::StraightLine:
					if (!Read(mark.PointsSetter(isGlobal)))
						return false;

					mark.PointListSetter(isGlobal).resize(mark.Points().nPoints);
					for (size_t i = 0; i < mark.Points().nPoints; ++i)
						if (!Read(mark.PointListSetter(isGlobal)[i]))
							return false;
					break;

				case OAIN_MARK_TYPE::ImageEmbedded:		/* AN_NEW_ROTATE_STRUCT */
				case OAIN_MARK_TYPE::ImageReference:
					if (!Read(mark.RotationSetter(isGlobal)))
						return false;

					DBGPRINT("Encountered Named Block: %s, global: %s\n", name.c_str(), isGlobal ? "yes" : "no");
					DBGPRINT(" - Rotation: %d\n", mark.Rotation.rotation);

					break;
			}
		}
	}

	else if (name == "OiFilNam") {
		if (!Read(mark.FileNameSetter(isGlobal), block.Size))
			return false;

		DBGPRINT("Encountered Named Block: %s, global: %s\n", name.c_str(), isGlobal ? "yes" : "no");
	}

	else if (name == "OiDIB") {
		if (!Read(mark.DibInfoSetter(isGlobal), block.Size))
			return false;

		DBGPRINT("Encountered Named Block: %s, global: %s\n", name.c_str(), isGlobal ? "yes" : "no");
	}

	else if (name == "OiGroup") {
		if (!Read(mark.GroupSetter(isGlobal), block.Size))
			return false;

		DBGPRINT("Encountered Named Block: %s, global: %s, value: %s\n", name.c_str(), isGlobal ? "yes" : "no", mark.Group.c_str());
	}

	else if (name == "OiIndex") {
		if (!Read(mark.IndexSetter(isGlobal), block.Size))
			return false;

		DBGPRINT("Encountered Named Block: %s, global: %s, value: %s\n", name.c_str(), isGlobal ? "yes" : "no", mark.Index.c_str());
	}

	else if (name == "OiAnText") {
		if (!Read(mark.TextSetter(isGlobal)))
			return false;

		if (mark.Text(isGlobal).uAnoTextLength == 0) {
			mark.AsciiTextSetter(isGlobal) = "";
		} else {
			// read the raw bytes of this named block, we decide how to interpret this later.
			std::vector<uint8_t> text;
			if (!Read(text, static_cast<size_t>(mark.Text(isGlobal).uAnoTextLength)))
				return false;

			if (!(text.size() & 1)) {/* an even number as length, could be unicode */
				// try detecting non-ansi characters
				bool ansi = true;
				for (size_t i = 0, l = text.size(); i < l - 1; ++i) {
					if (text[i] == 0) { /* don't check text[i] & 0x80 in the case of extended-ascii */
						ansi = false;
						break;
					}
				}

				if (ansi) {
					mark.AsciiTextSetter(isGlobal) = std::string(text.begin(), text.end());
				} else {
					std::vector<uint16_t> unicodeText(text.size() / 2);
					for (size_t i = 0, l = text.size(); i < l; i += 2)
						unicodeText[i / 2] = (text[i] << 8) | text[i + 1];

					mark.UnicodeTextSetter(isGlobal) = std::wstring(unicodeText.begin(), unicodeText.end());
				}

			} else {
				mark.AsciiTextSetter(isGlobal) = std::string(text.begin(), text.end());
			}
		}
	}

	else if (name == "OiHypLnk") {
		// ignore hyperlinks
	}

	try { Seek(next, std::ios::beg); }
	catch (...) { return false; }

	return true;
}

/// <summary>
/// Determines the amount of bytes left in the eiStream/Wang tag.
/// </summary>
/// <returns>The size left to read in the tag.</returns>
size_t WangAnnotationReader::SizeLeft() const {
	if (m_Offset >= m_Size)
		return 0;
	return m_Size - m_Offset;
}

/// <summary>
/// Determines when the end of the eiStream/Wang tag has been reached.
/// </summary>
/// <returns>True is returned when SizeLeft() == 0.</returns>
bool WangAnnotationReader::Eof() const {
	return SizeLeft() == 0;
}

/// <summary>
/// Throws an exception when <see cref="SizeLeft"/>() &lt; <paramref name="required"/>.
/// </summary>
/// <param name="required">The amount of bytes required to be left in the eiStream/Wang tag data.</param>
/// <exception cref="std::runtime_error">Thrown when <see cref="SizeLeft"/>() &lt; <paramref name="required"/>.</exception>
void WangAnnotationReader::AssertSizeLeft(size_t required) const {
	if (SizeLeft() < required)
		throw std::runtime_error("insufficient data left in the eiStream/WANG block");
}

/// <summary>
/// Seek to a location in the eiStream/Wang tag data.
/// </summary>
/// <param name="offset">The relative or absolute offset to seek to.</param>
/// <param name="dir">The direction to seek in (use the std::ios seek directions).</param>
/// <exception cref="std::out_of_range">Thrown when seeking out of the range of the eiStream/Wang data block.</exception>
void WangAnnotationReader::Seek(std::streamoff offset, std::ios::seekdir dir) {
	std::streamoff newOffset;

	switch (dir) {
		case std::ios::beg:
			if (offset < 0 || offset >= static_cast<std::streamoff>(m_Size))
				throw std::out_of_range("seek offset is out of range");
			m_Offset = static_cast<size_t>(offset);
			break;
		case std::ios::end:
			newOffset = static_cast<std::streamoff>(m_Offset + m_Size) + offset;
			if (newOffset < 0 || newOffset >= static_cast<std::streamoff>(m_Size))
				throw std::out_of_range("seek offset is out of range");
			m_Offset = static_cast<size_t>(newOffset);
			break;
		case std::ios::cur:
			newOffset = static_cast<std::streamoff>(m_Offset) + offset;
			if (newOffset < 0 || newOffset >= static_cast<std::streamoff>(m_Size))
				throw std::out_of_range("seek offset is out of range");
			m_Offset = static_cast<size_t>(newOffset);
			break;
	}
}

/// <summary>
/// Get a pointer to the value at the current (or specific) offset.
/// </summary>
/// <typeparam name="TValue">The value type to get a pointer for.</typeparam>
/// <returns>The cast pointer.</returns>
template <
	typename TValue,
	std::enable_if_t<
		std::conjunction<
			std::negation<std::is_pointer<TValue>>,
			std::negation<std::is_reference<TValue>>
		>::value,
		int
	>
>
TValue* WangAnnotationReader::GetAddress() {
	return reinterpret_cast<TValue*>(&m_AnnotationData[m_Offset]);
}

/// <summary>
/// Read an integral or floating point value from the current state.
/// </summary>
/// <typeparam name="TValue">The type of integral or floating point value to read.</typeparam>
/// <returns>The read value.</returns>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
template <typename TValue,
	std::enable_if_t<
		std::conjunction<
			std::disjunction<
				std::is_integral<TValue>, 
				std::is_floating_point<TValue>,
				std::is_enum<TValue>
			>,
			std::negation<std::is_pointer<TValue>>,
			std::negation<std::is_reference<TValue>>
		>::value,
		int
	>
>
TValue WangAnnotationReader::Read() {
	AssertSizeLeft(sizeof(TValue));
	
	auto result = *GetAddress<TValue>();
	m_Offset += sizeof(TValue);
	return result;
}

/// <summary>
/// Read a class or struct value from the current state.
/// </summary>
/// <typeparam name="TValue">The type of value to read.</typeparam>
/// <param name="value">A reference to the result.</param>
/// <returns>true on success, false when end of stream is reached.</returns>
template <typename TValue, std::enable_if_t<std::is_class_v<TValue>, int >>
[[nodiscard]] bool WangAnnotationReader::Read(TValue& value) {
	if (SizeLeft() < sizeof(TValue))
		return false;
	auto ptr = GetAddress<TValue>();
	memcpy(reinterpret_cast<void*>(&value), reinterpret_cast<const void*>(ptr), sizeof(TValue));
	m_Offset += sizeof(TValue);
	return true;
}

/// <summary>
/// Read a string
/// </summary>
/// <param name="value">A reference to the resulting string.</param>
/// <param name="length">The length to read.</param>
/// <returns>true on success, false when end of stream is reached.</returns>
[[nodiscard]] bool WangAnnotationReader::Read(std::string& value, size_t length) {
	if (SizeLeft() < length)
		return false;
	value = std::string(reinterpret_cast<const char*>(&m_AnnotationData[m_Offset]), length);
	m_Offset += length;
	return true;
}

/// <summary>
/// Read a vector of bytes.
/// </summary>
/// <param name="value">A reference to the resulting vector.</param>
/// <param name="length">The length to read.</param>
/// <returns>true on success, false when end of stream is reached.</returns>
[[nodiscard]] bool WangAnnotationReader::Read(std::vector<uint8_t>& value, size_t length) {
	if (SizeLeft() < length)
		return false;
	value.resize(length);
	memcpy(reinterpret_cast<void*>(&value[0]), reinterpret_cast<const void*>(&m_AnnotationData[m_Offset]), length);
	m_Offset += length;
	return true;
}