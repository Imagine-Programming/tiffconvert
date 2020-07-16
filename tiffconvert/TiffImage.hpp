#pragma once

#ifndef libtiffconvert_tiff_image_h
#define libtiffconvert_tiff_image_h

#include "libtiffconvert.h"
#include "DestructibleBuffer.hpp"
#include <string>
#include <memory>
#include <functional>

namespace TiffConvert {
	/// <summary>
	/// The TiffImage class describes collection of loaded Tiff pages (1 or more) as decoded image.
	/// </summary>
	class TiffImage {
		private:
			const tiff_image* m_ImageHandle = nullptr;

		public:
			/// <summary>
			/// Construct a new TiffImage instance from filepath.
			/// </summary>
			/// <param name="filepath">The Tiff file to load.</param>
			TiffImage(const std::string& filepath);

			/// <summary>
			/// Construct a new TiffImage instance from filepath.
			/// </summary>
			/// <param name="filepath">The Tiff file to load.</param>
			TiffImage(const std::wstring& filepath);

			/// <summary>
			/// Construct a new TiffImage instance from a buffer of Tiff data.
			/// </summary>
			/// <param name="buffer">A pointer to the Tiff image data.</param>
			/// <param name="size">The size of the image.</param>
			TiffImage(const void* buffer, uint32_t size);
			
			/// <summary>
			/// The destructor will release the internal image object.
			/// </summary>
			~TiffImage();

			/// <summary>
			/// Get the number of Tiff pages, or IFDs, in the image data.
			/// </summary>
			/// <returns>The page count.</returns>
			uint32_t GetPageCount() const noexcept;

			/// <summary>
			/// Get the width in pixels for a specific page.
			/// </summary>
			/// <param name="page">The page number.</param>
			/// <returns>The width in pixels.</returns>
			uint32_t GetPageWidth(uint32_t page) const noexcept;

			/// <summary>
			/// Get the height in pixels for a specific page.
			/// </summary>
			/// <param name="page">The page number.</param>
			/// <returns>The height in pixels.</returns>
			uint32_t GetPageHeight(uint32_t page) const noexcept;

			/// <summary>
			/// Scale a Tiff page to maximum dimensions, don't touch the image if the dimensions are already within bounds.
			/// </summary>
			/// <param name="page">The page number.</param>
			/// <param name="width">The maximum width in pixels.</param>
			/// <param name="height">The maximum height in pixels.</param>
			/// <param name="smooth">Use interpolation while resizing.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool ScaleToMaximum(uint32_t page, uint32_t width, uint32_t height, bool smooth = false) const noexcept;

			/// <summary>
			/// Encode a Tiff page to image file.
			/// </summary>
			/// <param name="page">The page number to encode.</param>
			/// <param name="filename">The filename to save the page as.</param>
			/// <param name="codec">The codec to use when encoding the image data.</param>
			/// <param name="options">Options for the codec.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool ExportPage(uint32_t page, const std::string& filename, tiff_export_format codec, uint32_t options);

			/// <summary>
			/// Encode a Tiff page to image file.
			/// </summary>
			/// <param name="page">The page number to encode.</param>
			/// <param name="filename">The filename to save the page as.</param>
			/// <param name="codec">The codec to use when encoding the image data.</param>
			/// <param name="options">Options for the codec.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool ExportPage(uint32_t page, const std::wstring& filename, tiff_export_format codec, uint32_t options);

			/// <summary>
			/// Encode a Tiff page to buffer.
			/// </summary>
			/// <param name="page">The page number to encode.</param>
			/// <param name="resultSize">A reference to an uint32_t holding the resulting buffer size.</param>
			/// <param name="codec">The codec to use when encoding the image data.</param>
			/// <param name="options">Options for the codec.</param>
			/// <returns>A shared pointer to a <see cref="DestructibleBuffer"/> instance of nullptr on failure.</returns>
			std::shared_ptr<DestructibleBuffer>	ExportPage(uint32_t page, uint32_t& resultSize, tiff_export_format codec, uint32_t options);

			/// <summary>
			/// Export the Tiff page collection as PDF.
			/// </summary>
			/// <param name="filepath">The filename to save the PDF as.</param>
			/// <param name="codec">The codec to use to encode each page.</param>
			/// <param name="options">Options for the codec.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool ExportPdf(const std::string& filepath, tiff_export_format codec, uint32_t options);

			/// <summary>
			/// Export the Tiff page collection as PDF.
			/// </summary>
			/// <param name="filepath">The filename to save the PDF as.</param>
			/// <param name="codec">The codec to use to encode each page.</param>
			/// <param name="options">Options for the codec.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool ExportPdf(const std::wstring& filepath, tiff_export_format codec, uint32_t options);

			/// <summary>
			/// Begin rendering to a specific page (or IFD).
			/// </summary>
			/// <param name="page">The page number to start rendering to.</param>
			/// <returns>A HDC on success, NULL otherwise.</returns>
			HDC BeginRender(uint32_t page) const;

			/// <summary>
			/// Stop rendering a previously started render.
			/// </summary>
			void EndRender() const noexcept;

			/// <summary>
			/// Begin rendering on a specific page (of IFD) and invoke a user-function before ending the render.
			/// </summary>
			/// <param name="page">The page number to render to.</param>
			/// <param name="f">The rendering function.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool Render(uint32_t page, std::function<void()> f) const;

			/// <summary>
			/// Begin rendering on a specific page (of IFD) and invoke a user-function before ending the render.
			/// </summary>
			/// <param name="page">The page number to render to.</param>
			/// <param name="f">The rendering function.</param>
			/// <returns>True when successful, false otherwise.</returns>
			bool Render(uint32_t page, std::function<void(HDC)> f) const;
	};
}

#endif 