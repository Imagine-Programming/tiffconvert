#include "pch.h"
#include "TiffFile.hpp"

using namespace TiffWang::Tiff;

namespace fs = std::filesystem;

/// <summary>
/// Construct a new TiffFile instance from ascii filepath.
/// </summary>
/// <param name="filepath">The Tiff-file to parse.</param>
TiffFile::TiffFile(const std::string& filepath)
	: m_Stream(std::ifstream(filepath, std::ios::in | std::ios::binary)), 
	  m_StreamSize(static_cast<size_t>(fs::file_size(filepath))),
	  m_Header({}) {

	if (!m_Stream.is_open())
		throw std::runtime_error("cannot open file");

	Init();
}

/// <summary>
/// Construct a new TiffFile instance from unicode filepath.
/// </summary>
/// <param name="filepath">The Tiff-file to parse.</param>
TiffFile::TiffFile(const std::wstring& filepath) 
	: m_Stream(std::ifstream(filepath, std::ios::in | std::ios::binary)), 
	  m_StreamSize(static_cast<size_t>(fs::file_size(filepath))),
	  m_Header({}) {

	if (!m_Stream.is_open())
		throw std::runtime_error("cannot open file");

	Init();
}

/// <summary>
/// Initialize the opened file stream, read and process the file header.
/// </summary>
void TiffFile::Init() {
	// Read TiffHeader
	Read(m_Header);

	// Is it little endian?
	if (_stricmp(m_Header.ByteOrder, IntelEndian)) {
		m_IntegerReader = {
			[&]() { return static_cast<int16_t>(le_to_host_ushort(Read<uint16_t>())); },
			[&]() { return le_to_host_ushort(Read<uint16_t>()); },
			[&]() { return static_cast<int32_t>(le_to_host_ulong(Read<uint32_t>())); },
			[&]() { return le_to_host_ulong(Read<uint32_t>()); },
			[&]() { return static_cast<int64_t>(le_to_host_uint64(Read<uint64_t>())); },
			[&]() { return le_to_host_uint64(Read<uint64_t>()); },
			[&]() { return le_to_host_float(Read<float>()); },
			[&]() { return le_to_host_double(Read<double>()); },

			[&]() { return static_cast<int16_t>(le_to_host_ushort(Read<uint16_t>())); },
			[&]() { return le_to_host_ushort(Read<uint16_t>()); },
			[&]() { return static_cast<int32_t>(le_to_host_ulong(Read<uint32_t>())); },
			[&]() { return le_to_host_ulong(Read<uint32_t>()); },
			[&]() { return static_cast<int64_t>(le_to_host_uint64(Read<uint64_t>())); },
			[&]() { return le_to_host_uint64(Read<uint64_t>()); },
			[&]() { return le_to_host_float(Read<float>()); },
			[&]() { return le_to_host_double(Read<double>()); }
		};

		m_Header.Magic			= le_to_host_ushort(m_Header.Magic);
		m_Header.OffsetFirstIfd = le_to_host_ulong(m_Header.OffsetFirstIfd);
	} 
	
	// Is it big endian?
	else if (_stricmp(m_Header.ByteOrder, MotorolaEndian)) {
		m_IntegerReader = {
			[&]() { return static_cast<int16_t>(be_to_host_ushort(Read<uint16_t>())); },
			[&]() { return be_to_host_ushort(Read<uint16_t>()); },
			[&]() { return static_cast<int32_t>(be_to_host_ulong(Read<uint32_t>())); },
			[&]() { return be_to_host_ulong(Read<uint32_t>()); },
			[&]() { return static_cast<int64_t>(be_to_host_uint64(Read<uint64_t>())); },
			[&]() { return be_to_host_uint64(Read<uint64_t>()); },
			[&]() { return be_to_host_float(Read<float>()); },
			[&]() { return be_to_host_double(Read<double>()); },

			[&]() { return static_cast<int16_t>(le_to_host_ushort(Read<uint16_t>())); },
			[&]() { return le_to_host_ushort(Read<uint16_t>()); },
			[&]() { return static_cast<int32_t>(le_to_host_ulong(Read<uint32_t>())); },
			[&]() { return le_to_host_ulong(Read<uint32_t>()); },
			[&]() { return static_cast<int64_t>(le_to_host_uint64(Read<uint64_t>())); },
			[&]() { return le_to_host_uint64(Read<uint64_t>()); },
			[&]() { return le_to_host_float(Read<float>()); },
			[&]() { return le_to_host_double(Read<double>()); }
		};

		m_Header.Magic			= be_to_host_ushort(m_Header.Magic);
		m_Header.OffsetFirstIfd = be_to_host_ulong(m_Header.OffsetFirstIfd);
	} 

	// Unknown endianness, stop. Must be malformed image.
	else { 
		throw std::runtime_error("malformed or unsupported TIFF header, byte order indication not 'II' or 'MM'");
	}

	// Verify that the magic number is equal to what we expect.
	if (m_Header.Magic != Magic)
		throw std::runtime_error("malformed or unsupported TIFF header, magic number is not the expected '" + std::to_string(Magic) + "'");
}

/// <summary>
/// Process all the Image File Directories.
/// </summary>
void TiffFile::ReadIfdCollection() {
	// Seek to the first IFD and keep reading until we cannot.
	m_Stream.seekg(m_Header.OffsetFirstIfd, std::ios::beg);

	while (true) {
		auto  entries = m_IntegerReader.Uint16();			/* the number of tags in this IFD */
		auto& ifd	  = m_PageIfdCollection.emplace_back(); /* construct a new list of tags in-place */

		// read the entries in this directory
		for (auto i = 0; i < entries; ++i) {
			auto& entry       = ifd.emplace_back(); /* construct a new tag in place */
			entry.TagId       = static_cast<TiffTagId>(m_IntegerReader.Uint16());
			entry.TagType     = static_cast<TiffTagType>(m_IntegerReader.Uint16());
			entry.ValueCount  = m_IntegerReader.Uint32();
			entry.ValueOffset = m_IntegerReader.Uint32();
			entry.IsWangTag   = entry.TagId == TiffTagId::TIFF_WANG_TAG && entry.TagType == TiffTagType::BYTE;
		}

		// determine if we have more IFD to read
		auto nextOffset = m_IntegerReader.Uint32();
		if (nextOffset == 0)
			break; /* all IFD's read */

		// seek to the next directory and continue the loop
		m_Stream.seekg(nextOffset, std::ios::beg);
	}

	// Fix the IFD order, as the order they were written in might not match the render order.
	CorrectIfdOrder();

	// Prepare collections for some IFD-specific information.
	m_Dimensions.resize(GetPageCount());
	m_Software.resize(GetPageCount());
	m_Artist.resize(GetPageCount());
	m_DateTime.resize(GetPageCount());

	// Try to find some common tags that might be useful.
	for (size_t pageIndex = 0; pageIndex < GetPageCount(); ++pageIndex) {
		const auto& page = m_PageIfdCollection[pageIndex];

		for (const auto& entry : page) {
			switch (entry.TagId) {
				case TiffTagId::TIFF_IMAGE_YRESOLUTION:
					m_Dimensions[pageIndex].ResolutionY = ReadRational(entry);
					break;
				case TiffTagId::TIFF_IMAGE_XRESOLUTION:
					m_Dimensions[pageIndex].ResolutionX = ReadRational(entry);
					break;
				case TiffTagId::TIFF_IMAGE_LENGTH_TAG:
					m_Dimensions[pageIndex].Height = entry.ValueOffset;
					break;
				case TiffTagId::TIFF_IMAGE_WIDTH_TAG:
					m_Dimensions[pageIndex].Width = entry.ValueOffset;
					break;
				case TiffTagId::TIFF_IMAGE_RESOLUTION_UNIT:
					m_Dimensions[pageIndex].ResolutionUnit = static_cast<TiffResolutionUnit>(entry.ValueOffset);
					break;
				case TiffTagId::TIFF_IMAGE_SOFTWARE:
					m_Software[pageIndex] = ReadAsciiString(entry);
					break;
				case TiffTagId::TIFF_IMAGE_DATETIME:
					m_DateTime[pageIndex] = ReadAsciiString(entry);
					break;
				case TiffTagId::TIFF_IMAGE_ARTIST:
					m_Artist[pageIndex] = ReadAsciiString(entry);
					break;
			}
		}
	}
}

/// <summary>
/// Get the total number of Image File Directories.
/// </summary>
/// <returns>The number of directories (or pages).</returns>
size_t TiffFile::GetPageCount() const noexcept {
	return m_PageIfdCollection.size();
}

/// <summary>
/// Get the total number of tags in a specific IFD (page).
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <returns>The number of tags in this IFD.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
size_t TiffFile::GetPageIfdCount(size_t pageIndex) const {
	AssertPageIndex(pageIndex);

	return m_PageIfdCollection.at(pageIndex).size();
}

/// <summary>
/// Get the IFD tag at a certain page index and IFD tag index.
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <param name="ifdIndex">The tag index.</param>
/// <returns>A const reference to the tag entry.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
const TiffIfdEntry& TiffFile::GetPageIfd(size_t pageIndex, size_t ifdIndex) const {
	AssertPageIfdIndex(pageIndex, ifdIndex);

	return m_PageIfdCollection.at(pageIndex).at(ifdIndex);
}

/// <summary>
/// Get the dimensions for a specific IFD (page).
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <returns>A const reference to the dimensions and resolution instance.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
const TiffDimensions& TiffFile::GetDimensions(size_t pageIndex) const {
	AssertPageIndex(pageIndex);

	return m_Dimensions[pageIndex];
}

/// <summary>
/// Get the name of the software that wrote a specific IFD (page).
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <returns>The software name or an empty string if unavailable.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
const std::string& TiffFile::GetSoftware(size_t pageIndex) const {
	AssertPageIndex(pageIndex);
	return m_Software[pageIndex];
}

/// <summary>
/// Get the formatted creation date and time or a specific IFD (page).
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <returns>The formatted timestamp or an empty string if unavailable.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
const std::string& TiffFile::GetDateTime(size_t pageIndex) const {
	AssertPageIndex(pageIndex);
	return m_DateTime[pageIndex];
}

/// <summary>
/// Get the artist name or a specific IFD (page).
/// </summary>
/// <param name="pageIndex">The IFD (page) index.</param>
/// <returns>The artist name or an empty string if unavailable.</returns>
/// <exception cref="std::out_of_range">When indices are out of range.</exception>
const std::string& TiffFile::GetArtist(size_t pageIndex) const {
	AssertPageIndex(pageIndex);
	return m_Artist[pageIndex];
}

/// <summary>
/// Correct the order of IFD collections based on page index tags, if any. If one of 
/// the IFDs (pages) does not contain a page index tag, the order of reading is maintained.
/// </summary>
void TiffFile::CorrectIfdOrder() {
	// The resulting ordered list of IFD's
	TiffIfdList orderedList(m_PageIfdCollection.size());
	
	// Iterate over the current list.
	for (size_t unorderedIndex = 0; unorderedIndex < m_PageIfdCollection.size(); ++unorderedIndex) {
		const auto& collection = m_PageIfdCollection[unorderedIndex];
		size_t		correctIndex = 0;
		bool		correctIndexFound = false;

		// try to find the page number tag.
		for (size_t ifdIndex = 0; ifdIndex < collection.size(); ++ifdIndex) {
			const auto& entry = collection[ifdIndex];

			if (entry.TagId == TiffTagId::TIFF_PAGE_NUMBER) {
				auto pageInfo     = ReadUnsignedShortArray(entry);
				correctIndexFound = true;
				correctIndex      = pageInfo[0];

				if (correctIndex >= GetPageCount()) /* invalid data, can't reorder as written index exceeds number of IFD's */
					return;

				break;
			}
		}

		// no page number stored in this IFD, so we can't reorder.
		if (!correctIndexFound)
			return;

		// Place at correct index.
		orderedList[correctIndex] = collection;
	}

	// When we made it to here, we reordered the list successfully.
	m_PageIfdCollection = orderedList;
}

size_t TiffFile::SizeLeft() const {
	if (m_Stream.tellg() >= static_cast<std::streampos>(m_StreamSize))
		return 0;
	return static_cast<size_t>(m_StreamSize - m_Stream.tellg());
}

void TiffFile::AssertSizeLeft(size_t required) const {
	if (SizeLeft() < required)
		throw std::runtime_error("insufficient data left in stream");
}

void TiffFile::AssertPageIndex(size_t pageIndex) const {
	if (pageIndex >= GetPageCount())
		throw std::out_of_range("requested page index is out of range");
}

void TiffFile::AssertPageIfdIndex(size_t pageIndex, size_t ifdIndex) const {
	AssertPageIndex(pageIndex);

	if (ifdIndex >= GetPageIfdCount(pageIndex))
		throw std::out_of_range("requested IFD index is out of range");
}

/// <summary>
/// Read a struct or class value from the stream.
/// </summary>
/// <param name="value">A reference to the value to read to.</param>
/// <typeparam name="The type of struct or class to read."></typeparam>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
template <
	typename TValue,
	std::enable_if_t<std::is_class_v<TValue>, int>
>
void TiffFile::Read(TValue& value) {
	AssertSizeLeft(sizeof(TValue));
	m_Stream.read(reinterpret_cast<char*>(&value), sizeof(TValue));
}

/// <summary>
/// Read an integral or floating point value from the stream.
/// </summary>
/// <typeparam name="TValue">The type of integral or floating point value to read.</typeparam>
/// <returns>The read value.</returns>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
template <typename TValue,
	std::enable_if_t<
	std::conjunction<
	std::disjunction<std::is_integral<TValue>, std::is_floating_point<TValue>>,
	std::negation<std::is_pointer<TValue>>,
	std::negation<std::is_reference<TValue>>
	>::value,
	int
	>
>
TValue TiffFile::Read() {
	TValue value;
	m_Stream.read(reinterpret_cast<char*>(&value), sizeof(TValue));
	return value;
}

/// <summary>
/// Read all the associated tag data from a BYTE tag to a buffer.
/// </summary>
/// <param name="entry">The TiffIfdEntry to read, which should have BYTE as TagType.</param>
/// <param name="buffer">The output buffer, which should be large enough to hold all of <see cref="TiffIfdEntry::ValueCount"/>.</param>
/// <param name="size">The size of the output buffer.</param>
/// <exception cref="::std::runtime_error">When insufficient data is available, the output buffer is too small or the type != BYTE an exception is thrown.</exception>
void TiffFile::Read(const TiffIfdEntry& entry, uint8_t* buffer, size_t size) const {
	if (entry.TagType != TiffTagType::BYTE)
		throw std::runtime_error("unexpected type for IFD tag encountered, should be BYTE");

	AssertSizeLeft(size);

	if (size < entry.ValueCount)
		throw std::runtime_error("insufficient data in output buffer, should be " + std::to_string(entry.ValueCount) + " bytes large");

	auto offset = m_Stream.tellg();
	m_Stream.seekg(entry.ValueOffset, std::ios::beg);
	m_Stream.read(reinterpret_cast<char*>(buffer), entry.ValueCount);
	m_Stream.seekg(offset, std::ios::beg);
}

/// <summary>
/// Read all the associated tag data from a BYTE tag to a buffer
/// </summary>
/// <param name="entry">The TiffIfdEntry to read, which should have BYTE as TagType.</param>
/// <param name="result">The output buffer</param>
/// <exception cref="::std::runtime_error">When insufficient data is available or the type != BYTE an exception is thrown.</exception>
void TiffFile::Read(const TiffIfdEntry& entry, std::vector<uint8_t>& result) const {
	if (entry.TagType != TiffTagType::BYTE)
		throw std::runtime_error("cannot process provided TiffIfdEntry, type is not the expected BYTE");

	if (entry.ValueCount == 0)
		throw std::runtime_error("cannot process provided TiffIfdEntry, entry holds no values");

	result.resize(static_cast<size_t>(entry.ValueCount));
	Read(entry, &result[0], result.size());
}

/// <summary>
/// Read one or more TiffTagType::SHORT values
/// </summary>
/// <param name="entry">The TiffIfdEntry to read.</param>
/// <returns>A vector of read unsigned shorts.</returns>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
std::vector<uint16_t> TiffFile::ReadUnsignedShortArray(const TiffIfdEntry& entry) {
	std::vector<uint16_t> result;

	if (entry.TagType != TiffTagType::SHORT)
		throw std::runtime_error("unexpected type for IFD tag encountered, should be SHORT");

	auto offset = m_Stream.tellg();

	m_Stream.seekg(entry.ValueOffset, std::ios::beg);

	result.resize(entry.ValueCount);
	for (uint32_t i = 0; i < entry.ValueCount; ++i)
		result[i] = m_IntegerReader.Uint16();

	m_Stream.seekg(offset, std::ios::beg);

	return result;
}

/// <summary>
/// Read a rational number (two TiffTagType::LONG values) and produce a rational number,
/// by dividing the numerator by the denominator.
/// </summary>
/// <param name="entry">The TiffIfdEntry to read.</param>
/// <returns>A double floating point value represented by the 2 read LONG values.</returns>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
double TiffFile::ReadRational(const TiffIfdEntry& entry) {
	if (entry.TagType != TiffTagType::RATIONAL)
		throw std::runtime_error("unexpected type for IFD tag encountered, should be RATIONAL");

	auto offset = m_Stream.tellg();

	m_Stream.seekg(entry.ValueOffset, std::ios::beg);

	auto numerator = (double)m_IntegerReader.Uint32();
	auto denominator = (double)m_IntegerReader.Uint32();
	auto result = 0.0;

	m_Stream.seekg(offset, std::ios::beg);

	if (denominator == 0)
		return result;

	return numerator / denominator;
}

/// <summary>
/// Read a NUL-terminated Ascii-string, ignoring the NUL character.
/// </summary>
/// <param name="entry">The TiffIfdEntry to read.</param>
/// <returns>The resulting string, or an empty string when its length was 0.</returns>
/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
std::string TiffFile::ReadAsciiString(const TiffIfdEntry& entry) {
	if (entry.TagType != TiffTagType::ASCII)
		throw std::runtime_error("unexpected type for IFD tag encountered, should be ASCII");

	// only NUL-character
	if (entry.ValueCount <= 1)
		return "";

	auto offset = m_Stream.tellg();
	std::string result(static_cast<size_t>(entry.ValueCount - 1), ' ');

	m_Stream.seekg(entry.ValueOffset, std::ios::beg);
	m_Stream.read(&result[0], entry.ValueCount - 1);
	m_Stream.seekg(offset, std::ios::beg);

	return result;
}