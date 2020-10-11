#pragma once

#include "pch.h"

#ifndef tiff_file_h
#define tiff_file_h

	namespace TiffWang {
		namespace Tiff {
			constexpr const char*	IntelEndian	 = "II";		// Start bytes of Tiff files, little endian.
			constexpr const char*	MotorolaEndian = "MM";		// Start bytes of Tiff files, big endian.
			constexpr uint16_t		Magic = 42;					// Magic number in Tiff header.

			/// <summary>
			/// A struct allowing for defining how to read integers (and floating point numbers) dynamically. This is
			/// used to read both little endian and big endian streams.
			/// </summary>
			struct TiffNumericReader {
				std::function<int16_t()>	Int16;
				std::function<uint16_t()>	Uint16;
				std::function<int32_t()>	Int32;
				std::function<uint32_t()>	Uint32;
				std::function<int64_t()>	Int64;
				std::function<uint64_t()>	Uint64;
				std::function<float()>		Float;
				std::function<double()>		Double;

				std::function<int16_t()>	Int16Le;
				std::function<uint16_t()>	Uint16Le;
				std::function<int32_t()>	Int32Le;
				std::function<uint32_t()>	Uint32Le;
				std::function<int64_t()>	Int64Le;
				std::function<uint64_t()>	Uint64Le;
				std::function<float()>		FloatLe;
				std::function<double()>		DoubleLe;
			};

#pragma pack( push, 1 )

			/// <summary>
			/// The Tiff file header
			/// </summary>
			struct TiffHeader {
				char		ByteOrder[2];		// Should be either II or MM.
				uint16_t	Magic;				// 42.
				uint32_t	OffsetFirstIfd;		// Offset from beginning of stream to first IFD.
			};

			/// <summary>
			/// A single tag in an Image File Directory
			/// </summary>
			struct TiffIfdEntry {
				TiffTagId	TagId;				// The identification of this tag.
				TiffTagType	TagType;			// The type of this tag.
				uint32_t	ValueCount;			// The number of values referenced by this tag (in bytes, that is sizeof(TagType) * ValueCount).
				uint32_t	ValueOffset;		// The offset (from the beginning of the stream) to the first value.
				bool		IsWangTag;			// True when this tag refers to an eiStream/Wang tag.
			};

#pragma pack( pop )

			/// <summary>
			/// The dimensions and resolution of an IFD (Tiff page).
			/// </summary>
			struct TiffDimensions {
				double				ResolutionX = 0;		// Horizontal resolution (i.e. 300 dpi).
				double				ResolutionY = 0;		// Vertical resolution (i.e. 300 dpi).
				uint32_t			Height = 0;				// The height, in pixels.
				uint32_t			Width  = 0;				// The width, in pixels.

				/// <summary>
				/// The resolution unit describes the unit of the <see cref="ResolutionX"/> and <see cref="ResolutionY"/> members.
				/// </summary>
				TiffResolutionUnit	ResolutionUnit = TiffResolutionUnit::NoAbsoluteMeasurement;
			};

			/// <summary>
			/// The TiffFile class is capable of handling a Tiff file on binary level, so not on graphical level. It does not decode 
			/// the image, it merely parses the structure of the file and enumerates all the tags. With this information, a developer
			/// can process the tags however they seem fit.
			/// </summary>
			class __EXPORTED_API TiffFile {
				/// <summary>
				/// <see cref="WangAnnotationReader"/> is allowed to access all private members of <see cref="TiffFile"/>.
				/// </summary>
				friend class WangAnnotationReader;

				using TiffIfdList		= std::vector<std::vector<TiffIfdEntry>>;	// A list of image file directories, which are lists of tags, i.e. [1] is the IFD of page 2.
				using TiffDimensionList = std::vector<TiffDimensions>;				// A list of IFD dimensions, i.e. [1] is the dimensions of page 2.
				using TiffStringList	= std::vector<std::string>;					// A simple alias for a vector of strings.

				private:
					#pragma warning ( push )
					#pragma warning ( disable: 4251 ) /* "needs to have dll-interface to be used by clients of class" - members are not public and have no friends outside the DLL */
					mutable std::ifstream	m_Stream;
					size_t					m_StreamSize;
					TiffNumericReader		m_IntegerReader;
					TiffHeader				m_Header;
					TiffIfdList				m_PageIfdCollection;
					TiffDimensionList		m_Dimensions;

					TiffStringList			m_Software;
					TiffStringList			m_DateTime;
					TiffStringList			m_Artist;

					#pragma warning ( pop )
				public:
					/// <summary>
					/// Construct a new TiffFile instance from ascii filepath.
					/// </summary>
					/// <param name="filepath">The Tiff-file to parse.</param>
					TiffFile(const std::string& filepath);

					/// <summary>
					/// Construct a new TiffFile instance from unicode filepath.
					/// </summary>
					/// <param name="filepath">The Tiff-file to parse.</param>
					TiffFile(const std::wstring& filepath);

				private:
					/// <summary>
					/// Initialize the opened file stream, read and process the file header.
					/// </summary>
					void Init();

				public:
					TiffFile(const TiffFile&) = delete;
					TiffFile& operator=(const TiffFile&) = delete;
					TiffFile(TiffFile&&) = delete;
					TiffFile& operator=(TiffFile&&) = delete;

					/// <summary>
					/// Process all the Image File Directories.
					/// </summary>
					void ReadIfdCollection();

					/// <summary>
					/// Get the total number of Image File Directories.
					/// </summary>
					/// <returns>The number of directories (or pages).</returns>
					size_t GetPageCount() const noexcept;

					/// <summary>
					/// Get the total number of tags in a specific IFD (page).
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <returns>The number of tags in this IFD.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					size_t GetPageIfdCount(size_t pageIndex) const;

					/// <summary>
					/// Get the IFD tag at a certain page index and IFD tag index.
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <param name="ifdIndex">The tag index.</param>
					/// <returns>A const reference to the tag entry.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					const TiffIfdEntry& GetPageIfd(size_t pageIndex, size_t ifdIndex) const;

					/// <summary>
					/// Get the dimensions for a specific IFD (page).
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <returns>A const reference to the dimensions and resolution instance.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					const TiffDimensions& GetDimensions(size_t pageIndex) const;

					/// <summary>
					/// Get the name of the software that wrote a specific IFD (page).
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <returns>The software name or an empty string if unavailable.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					const std::string& GetSoftware(size_t pageIndex) const;

					/// <summary>
					/// Get the formatted creation date and time or a specific IFD (page).
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <returns>The formatted timestamp or an empty string if unavailable.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					const std::string& GetDateTime(size_t pageIndex) const;

					/// <summary>
					/// Get the artist name or a specific IFD (page).
					/// </summary>
					/// <param name="pageIndex">The IFD (page) index.</param>
					/// <returns>The artist name or an empty string if unavailable.</returns>
					/// <exception cref="std::out_of_range">When indices are out of range.</exception>
					const std::string& GetArtist(size_t pageIndex) const;

				private:
					/// <summary>
					/// Correct the order of IFD collections based on page index tags, if any. If one of 
					/// the IFDs (pages) does not contain a page index tag, the order of reading is maintained.
					/// </summary>
					void CorrectIfdOrder();

					/// <summary>
					/// Determines how many bytes there are left in the stream to read.
					/// </summary>
					/// <returns>Number of bytes left</returns>
					size_t SizeLeft() const;

					/// <summary>
					/// Throw an exception if there is not enough data left to read.
					/// </summary>
					/// <param name="required">The number of bytes required in the stream.</param>
					/// <exception cref="std::runtime_error">When required &lt; SizeLeft().</exception>
					void AssertSizeLeft(size_t required) const;


					/// <summary>
					/// Throw an exception if a certain offset in a stream does not exist.
					/// </summary>
					/// <param name="offset">The offset to test</param>
					/// <exception cref="std::runtime_error">When the offset is out of bounds.</exception>
					void AssertOffset(size_t offset) const;

					/// <summary>
					/// Throw an exception if the IFD (page) index is out of range.
					/// </summary>
					/// <param name="pageIndex">The index to check for.</param>
					/// <exception cref="std::out_of_range">When index is out of bounds.</exception>
					void AssertPageIndex(size_t pageIndex) const;

					/// <summary>
					/// Throw an exception if either the IFD (page) index or if the IFD Tag index is out of range.
					/// </summary>
					/// <param name="pageIndex">The page index to check for.</param>
					/// <param name="ifdIndex">The tag index to check for.</param>
					/// <exception cref="std::out_of_range">When either index is out of bounds.</exception>
					void AssertPageIfdIndex(size_t pageIndex, size_t ifdIndex) const;

					/// <summary>
					/// Read a struct or class value from the stream.
					/// </summary>
					/// <param name="value">A reference to the value to read to.</param>
					/// <typeparam name="The type of struct or class to read."></typeparam>
					/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
					template <
						typename TValue,
						std::enable_if_t<std::is_class_v<TValue>, int> = 0
					>
					void Read(TValue& value);
					
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
						> = 0
					>
					TValue Read();

					/// <summary>
					/// Read all the associated tag data from a BYTE tag to a buffer.
					/// </summary>
					/// <param name="entry">The TiffIfdEntry to read, which should have BYTE as TagType.</param>
					/// <param name="buffer">The output buffer, which should be large enough to hold all of <see cref="TiffIfdEntry::ValueCount"/>.</param>
					/// <param name="size">The size of the output buffer.</param>
					/// <exception cref="::std::runtime_error">When insufficient data is available, the output buffer is too small or the type != BYTE an exception is thrown.</exception>
					void Read(const TiffIfdEntry& entry, uint8_t* buffer, size_t size) const;

					/// <summary>
					/// Read all the associated tag data from a BYTE tag to a buffer
					/// </summary>
					/// <param name="entry">The TiffIfdEntry to read, which should have BYTE as TagType.</param>
					/// <param name="result">The output buffer</param>
					/// <exception cref="::std::runtime_error">When insufficient data is available or the type != BYTE an exception is thrown.</exception>
					void Read(const TiffIfdEntry& entry, std::vector<uint8_t>& result) const;

					/// <summary>
					/// Read one or more TiffTagType::SHORT values
					/// </summary>
					/// <param name="entry">The TiffIfdEntry to read.</param>
					/// <returns>A vector of read unsigned shorts.</returns>
					/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
					std::vector<uint16_t> ReadUnsignedShortArray(const TiffIfdEntry& entry);

					/// <summary>
					/// Read a rational number (two TiffTagType::LONG values) and produce a rational number,
					/// by dividing the numerator by the denominator.
					/// </summary>
					/// <param name="entry">The TiffIfdEntry to read.</param>
					/// <returns>A double floating point value represented by the 2 read LONG values.</returns>
					/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
					double ReadRational(const TiffIfdEntry& entry);

					/// <summary>
					/// Read a NUL-terminated Ascii-string, ignoring the NUL character.
					/// </summary>
					/// <param name="entry">The TiffIfdEntry to read.</param>
					/// <returns>The resulting string, or an empty string when its length was 0.</returns>
					/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
					std::string ReadAsciiString(const TiffIfdEntry& entry);
			};

		}
	}

#endif 