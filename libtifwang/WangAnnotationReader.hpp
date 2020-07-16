#pragma once

#include "pch.h"

#ifndef wang_annotation_reader_h
#define wang_annotation_reader_h
	#include "TiffFile.hpp"
	#include "IWangAnnotationCallback.hpp"

	namespace TiffWang {
		namespace Tiff {
			/// <summary>
			/// The WangAnnotationReader class is capable of processing an eiStream/Wang annotation tag from a Tiff file
			/// and trigger an event on each mark it encounters. It does not create a list of encountered marks, rather it 
			/// calls a user function on each mark found.
			/// </summary>
			class __EXPORTED_API WangAnnotationReader {
				private:
					#pragma warning ( push )
					#pragma warning ( disable: 4251 ) /* "needs to have dll-interface to be used by clients of class" - members are not public */
					TiffFile&				m_File;
					const TiffIfdEntry&		m_Tag;
					std::vector<uint8_t>	m_AnnotationData;
					size_t					m_Offset = 0;
					size_t					m_Size;

					// The IWangAnnotationCallback implementation responsible for handling found marks.
					std::shared_ptr<IWangAnnotationCallback> m_Handler = nullptr;
					#pragma warning ( pop )

				public:
					/// <summary>
					/// Construct a new WangAnnotationReader from an opened <see cref="TiffFile"/>, looking at a specific <see cref="TiffIfdEntry"/>.
					/// </summary>
					/// <param name="file">The opened Tiff file.</param>
					/// <param name="tag">The eiStream/Wang tag to process.</param>
					/// <exception cref="std::runtime_error">Thrown when the <paramref name="tag"/> is not an eiStream/Wang tag or when insufficient data is available to read.</exception>
					WangAnnotationReader(TiffFile& file, const TiffIfdEntry& tag);

					WangAnnotationReader(const WangAnnotationReader&) = delete;
					WangAnnotationReader& operator=(const WangAnnotationReader&) = delete;
					WangAnnotationReader(WangAnnotationReader&&) = delete;
					WangAnnotationReader& operator=(WangAnnotationReader&&) = delete;

					/// <summary>
					/// Start reading the eiStream/Wang tag block.
					/// </summary>
					void Read();

					/// <summary>
					/// Set the event handler instance to use when processing marks, invoke this method before calling <see cref="Read"/>.
					/// </summary>
					/// <param name="h">A shared pointer to an implementation of <see cref="IWangAnnotationCallback"/>.</param>
					void SetHandler(std::shared_ptr<IWangAnnotationCallback> h);
				private:
					/// <summary>
					/// Emit a mark to the event handler.
					/// </summary>
					/// <param name="mark">A reference to the current <see cref="TiffWangMark"/> data.</param>
					void EmitMark(TiffWangMark& mark);

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
					[[nodiscard]] bool ProcessNamedBlock(TiffWangNamedBlock& block, TiffWangMark& mark, bool is16bit, bool isGlobal);

					/// <summary>
					/// Determines the amount of bytes left in the eiStream/Wang tag.
					/// </summary>
					/// <returns>The size left to read in the tag.</returns>
					size_t SizeLeft() const;

					/// <summary>
					/// Determines when the end of the eiStream/Wang tag has been reached.
					/// </summary>
					/// <returns>True is returned when SizeLeft() == 0.</returns>
					bool Eof() const;

					/// <summary>
					/// Throws an exception when <see cref="SizeLeft"/>() &lt; <paramref name="required"/>.
					/// </summary>
					/// <param name="required">The amount of bytes required to be left in the eiStream/Wang tag data.</param>
					/// <exception cref="std::runtime_error">Thrown when <see cref="SizeLeft"/>() &lt; <paramref name="required"/>.</exception>
					void AssertSizeLeft(size_t required) const;

					/// <summary>
					/// Seek to a location in the eiStream/Wang tag data.
					/// </summary>
					/// <param name="offset">The relative or absolute offset to seek to.</param>
					/// <param name="dir">The direction to seek in (use the std::ios seek directions).</param>
					/// <exception cref="std::out_of_range">Thrown when seeking out of the range of the eiStream/Wang data block.</exception>
					void Seek(std::streamoff offset, std::ios::seekdir dir = std::ios::cur);

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
						> = 0
					>
					[[nodiscard]] TValue* GetAddress();

					/// <summary>
					/// Read an integral or floating point value from the current state.
					/// </summary>
					/// <typeparam name="TValue">The type of integral or floating point value to read.</typeparam>
					/// <returns>The read value.</returns>
					/// <exception cref="::std::runtime_error">When insufficient data is available, an exception is thrown.</exception>
					template <
						typename TValue,
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
						> = 0
					>
					[[nodiscard]] TValue Read();

					/// <summary>
					/// Read a class or struct value from the current state.
					/// </summary>
					/// <typeparam name="TValue">The type of value to read.</typeparam>
					/// <param name="value">A reference to the result.</param>
					/// <returns>true on success, false when end of stream is reached.</returns>
					template <typename TValue, std::enable_if_t<std::is_class_v<TValue>, int > = 0>
					[[nodiscard]] bool Read(TValue& value);

					/// <summary>
					/// Read a string.
					/// </summary>
					/// <param name="value">A reference to the resulting string.</param>
					/// <param name="length">The length to read.</param>
					/// <returns>true on success, false when end of stream is reached.</returns>
					[[nodiscard]] bool Read(std::string& value, size_t length);

					/// <summary>
					/// Read a vector of bytes.
					/// </summary>
					/// <param name="value">A reference to the resulting vector.</param>
					/// <param name="length">The length to read.</param>
					/// <returns>true on success, false when end of stream is reached.</returns>
					[[nodiscard]] bool Read(std::vector<uint8_t>& value, size_t length);
			};
		}
	}

#endif