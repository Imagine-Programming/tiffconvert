#include "TiffImage.hpp"
#include <stdexcept>

using namespace TiffConvert;

/// <summary>
/// Construct a new TiffImage instance from filepath.
/// </summary>
/// <param name="filepath">The Tiff file to load.</param>
TiffImage::TiffImage(const std::string& filepath) {
	auto image = tiff_image_open_a(filepath.c_str());
	if (!image)
		throw std::runtime_error("cannot open image");
	m_ImageHandle = image;
}

/// <summary>
/// Construct a new TiffImage instance from filepath.
/// </summary>
/// <param name="filepath">The Tiff file to load.</param>
TiffImage::TiffImage(const std::wstring& filepath) {
	auto image = tiff_image_open_w(filepath.c_str());
	if (!image)
		throw std::runtime_error("cannot open image");
	m_ImageHandle = image;
}

/// <summary>
/// Construct a new TiffImage instance from a buffer of Tiff data.
/// </summary>
/// <param name="buffer">A pointer to the Tiff image data.</param>
/// <param name="size">The size of the image.</param>
TiffImage::TiffImage(const void* buffer, uint32_t size) {
	auto image = tiff_image_open_p(reinterpret_cast<const tiff_header*>(buffer), size, false);
	if (!image)
		throw std::runtime_error("cannot open image");
	m_ImageHandle = image;
}

/// <summary>
/// The destructor will release the internal image object.
/// </summary>
TiffImage::~TiffImage() {
	if (m_ImageHandle != nullptr) {
		tiff_image_close(m_ImageHandle);
		m_ImageHandle = nullptr;
	}
}

/// <summary>
/// Get the number of Tiff pages, or IFDs, in the image data.
/// </summary>
/// <returns>The page count.</returns>
uint32_t TiffImage::GetPageCount() const noexcept {
	return static_cast<uint32_t>(tiff_image_page_count(m_ImageHandle));
}

/// <summary>
/// Get the width in pixels for a specific page.
/// </summary>
/// <param name="page">The page number.</param>
/// <returns>The width in pixels.</returns>
uint32_t TiffImage::GetPageWidth(uint32_t page) const noexcept {
	return static_cast<uint32_t>(tiff_image_page_width(m_ImageHandle, page));
}

/// <summary>
/// Get the height in pixels for a specific page.
/// </summary>
/// <param name="page">The page number.</param>
/// <returns>The height in pixels.</returns>
uint32_t TiffImage::GetPageHeight(uint32_t page) const noexcept {
	return static_cast<uint32_t>(tiff_image_page_height(m_ImageHandle, page));
}

/// <summary>
/// Scale a Tiff page to maximum dimensions, don't touch the image if the dimensions are already within bounds.
/// </summary>
/// <param name="page">The page number.</param>
/// <param name="width">The maximum width in pixels.</param>
/// <param name="height">The maximum height in pixels.</param>
/// <param name="smooth">Use interpolation while resizing.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::ScaleToMaximum(uint32_t page, uint32_t width, uint32_t height, bool smooth) const noexcept {
	return tiff_image_page_scale(m_ImageHandle, page, width, height, smooth);
}

/// <summary>
/// Encode a Tiff page to image file.
/// </summary>
/// <param name="page">The page number to encode.</param>
/// <param name="filename">The filename to save the page as.</param>
/// <param name="codec">The codec to use when encoding the image data.</param>
/// <param name="options">Options for the codec.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::ExportPage(uint32_t page, const std::string& filename, tiff_export_format codec, uint32_t options) {
	return tiff_image_export_page_a(m_ImageHandle, page, filename.c_str(), codec, options);
}

/// <summary>
/// Encode a Tiff page to image file.
/// </summary>
/// <param name="page">The page number to encode.</param>
/// <param name="filename">The filename to save the page as.</param>
/// <param name="codec">The codec to use when encoding the image data.</param>
/// <param name="options">Options for the codec.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::ExportPage(uint32_t page, const std::wstring& filename, tiff_export_format codec, uint32_t options) {
	return tiff_image_export_page_w(m_ImageHandle, page, filename.c_str(), codec, options);
}

/// <summary>
/// Encode a Tiff page to buffer.
/// </summary>
/// <param name="page">The page number to encode.</param>
/// <param name="resultSize">A reference to an uint32_t holding the resulting buffer size.</param>
/// <param name="codec">The codec to use when encoding the image data.</param>
/// <param name="options">Options for the codec.</param>
/// <returns>A shared pointer to a <see cref="DestructibleBuffer"/> instance of nullptr on failure.</returns>
std::shared_ptr<DestructibleBuffer>	TiffImage::ExportPage(uint32_t page, uint32_t& resultSize, tiff_export_format codec, uint32_t options) {
	uint32_t size;
	void*    buffer = tiff_image_export_page_p(m_ImageHandle, page, &size, codec, options);

	if (!buffer)
		return nullptr;

	resultSize = size;
	return std::make_shared<DestructibleBuffer>(buffer, size);
}

/// <summary>
/// Export the Tiff page collection as PDF.
/// </summary>
/// <param name="filepath">The filename to save the PDF as.</param>
/// <param name="codec">The codec to use to encode each page.</param>
/// <param name="options">Options for the codec.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::ExportPdf(const std::string& filepath, tiff_export_format codec, uint32_t options) {
	return tiff_image_export_pdf_a(m_ImageHandle, filepath.c_str(), codec, options);
}

/// <summary>
/// Export the Tiff page collection as PDF.
/// </summary>
/// <param name="filepath">The filename to save the PDF as.</param>
/// <param name="codec">The codec to use to encode each page.</param>
/// <param name="options">Options for the codec.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::ExportPdf(const std::wstring& filepath, tiff_export_format codec, uint32_t options) {
	return tiff_image_export_pdf_w(m_ImageHandle, filepath.c_str(), codec, options);
}

/// <summary>
/// Begin rendering to a specific page (or IFD).
/// </summary>
/// <param name="page">The page number to start rendering to.</param>
/// <returns>A HDC on success, NULL otherwise.</returns>
HDC TiffImage::BeginRender(uint32_t page) const {
	if (page >= GetPageCount())
		throw std::out_of_range("page number not within range of loaded image");

	return renderer_begin(m_ImageHandle, page);
}

/// <summary>
/// Stop rendering a previously started render.
/// </summary>
void TiffImage::EndRender() const noexcept {
	renderer_stop();
}

/// <summary>
/// Begin rendering on a specific page (of IFD) and invoke a user-function before ending the render.
/// </summary>
/// <param name="page">The page number to render to.</param>
/// <param name="f">The rendering function.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::Render(uint32_t page, std::function<void()> f) const {
	if (!BeginRender(page))
		return false;

	f();
	EndRender();
	return true;
}

/// <summary>
/// Begin rendering on a specific page (of IFD) and invoke a user-function before ending the render.
/// </summary>
/// <param name="page">The page number to render to.</param>
/// <param name="f">The rendering function.</param>
/// <returns>True when successful, false otherwise.</returns>
bool TiffImage::Render(uint32_t page, std::function<void(HDC dc)> f) const {
	auto dc = BeginRender(page);
	if (!dc)
		return false;

	f(dc);
	EndRender();
	return true;
}