#include "Renderer.hpp"
#include "Util.hpp"

using namespace TiffConvert;

/// <summary>
/// The highlight filter only renders onto white.
/// </summary>
/// <param name="x">X coordinate of the pixel to filter.</param>
/// <param name="y">Y coordinate of the pixel to filter.</param>
/// <param name="top">The top (new) color.</param>
/// <param name="bottom">The bottom (old) color.</param>
/// <returns>The new filtered color.</returns>
uint64_t __stdcall Renderer::HighlightFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom) {
	if (Util::IsWhite(static_cast<uint32_t>(bottom)))
		return top;

	return bottom;
}

/// <summary>
/// The highlight-transparent filter only renders onto white and only renders if the new color is not white.
/// </summary>
/// <param name="x">X coordinate of the pixel to filter.</param>
/// <param name="y">Y coordinate of the pixel to filter.</param>
/// <param name="top">The top (new) color.</param>
/// <param name="bottom">The bottom (old) color.</param>
/// <returns>The new filtered color.</returns>
uint64_t __stdcall Renderer::HighlightTransparentFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom) {
	if (Util::IsWhite(static_cast<uint32_t>(top)))
		return bottom;

	return HighlightFilter(x, y, top, bottom);
}

/// <summary>
/// The transparent filter only renders the new color if it is not white.
/// </summary>
/// <param name="x">X coordinate of the pixel to filter.</param>
/// <param name="y">Y coordinate of the pixel to filter.</param>
/// <param name="top">The top (new) color.</param>
/// <param name="bottom">The bottom (old) color.</param>
/// <returns>The new filtered color.</returns>
uint64_t __stdcall Renderer::TransparentFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom) {
	if (Util::IsWhite(static_cast<uint32_t>(top)))
		return bottom;

	return top;
}

/// <summary>
/// Determines which filter to use based on the highlight and transparent state from the mark attributes.
/// Returns nullptr if no filter should be applied.
/// </summary>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>The render filter to use, or nullptr if no filtering should be applied.</returns>
renderer_filter Renderer::GetHighlightFilter(bool highlight, bool transparent) {
	if (highlight && transparent)
		return HighlightTransparentFilter;
	if (highlight)
		return HighlightFilter;
	if (transparent)
		return TransparentFilter;
	return nullptr;
}

/// <summary>
/// Render a line between all the specified points, with a specific thickness and color.
/// </summary>
/// <param name="points">The list of points to draw a line in between.</param>
/// <param name="lineSize">The thickness of the line.</param>
/// <param name="color">The linecolor.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Line(const std::vector<POINT> points, uint32_t lineSize, uint32_t color, bool highlight, bool transparent) {
	return renderer_line(&points[0], static_cast<uint32_t>(points.size()), lineSize, color, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Render a line between all the specified points, with a specific thickness and color.
/// </summary>
/// <param name="points">The list of points to draw a line in between.</param>
/// <param name="lineSize">The thickness of the line.</param>
/// <param name="color">The linecolor.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Line(const std::vector<POINT> points, uint32_t lineSize, const RGBQUAD& color, bool highlight, bool transparent) {
	return Line(points, lineSize, Util::ColorToLong(color, 255), highlight);
}

/// <summary>
/// Render a line between all the specified points, with a specific color.
/// </summary>
/// <param name="points">The list of points to draw a line in between.</param>
/// <param name="color">The linecolor.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Line(const std::vector<POINT> points, uint32_t color, bool highlight, bool transparent) {
	return renderer_single_line(&points[0], static_cast<uint32_t>(points.size()), color, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Render a line between all the specified points, with a specific color.
/// </summary>
/// <param name="points">The list of points to draw a line in between.</param>
/// <param name="color">The linecolor.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Line(const std::vector<POINT> points, const RGBQUAD& color, bool highlight, bool transparent) {
	return Line(points, Util::ColorToLong(color, 255), highlight);
}

/// <summary>
/// Fill a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="color">The fill color.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::FillRect(const RECT& rectangle, uint32_t color, uint32_t cornerRadius, bool highlight, bool transparent) {
	return renderer_rect(&rectangle, color, 0, true, false, cornerRadius, 0, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Fill a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="color">The fill color.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::FillRect(const RECT& rectangle, const RGBQUAD& color, uint32_t cornerRadius, bool highlight, bool transparent) {
	return FillRect(rectangle, Util::ColorToLong(color, 255), cornerRadius, highlight);
}

/// <summary>
/// Stroke a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="color">The stroke color.</param>
/// <param name="strokeSize">The size, in pixels, of the stroke.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::StrokeRect(const RECT& rectangle, uint32_t color, uint32_t strokeSize, uint32_t cornerRadius, bool highlight, bool transparent) {
	return renderer_rect(&rectangle, 0, color, false, true, cornerRadius, strokeSize, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Stroke a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="color">The stroke color.</param>
/// <param name="strokeSize">The size, in pixels, of the stroke.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::StrokeRect(const RECT& rectangle, const RGBQUAD& color, uint32_t strokeSize, uint32_t cornerRadius, bool highlight, bool transparent) {
	return StrokeRect(rectangle, Util::ColorToLong(color, 255), strokeSize, cornerRadius, highlight);
}

/// <summary>
/// Fill and stroke a rectangle with certain colors and apply a corner radius to the rectangle if it is not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="fillColor">The fill color.</param>
/// <param name="strokeColor">The stroke color.</param>
/// <param name="strokeSize">The size, in pixels, of the stroke.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::FillAndStrokeRect(const RECT& rectangle, uint32_t fillColor, uint32_t strokeColor, uint32_t strokeSize, uint32_t cornerRadius, bool highlight, bool transparent) {
	return renderer_rect(&rectangle, fillColor, strokeColor, true, true, cornerRadius, strokeSize, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Fill and stroke a rectangle with certain colors and apply a corner radius to the rectangle if it is not 0.
/// </summary>
/// <param name="rectangle">The rectangle coordinates and sizes.</param>
/// <param name="fillColor">The fill color.</param>
/// <param name="strokeColor">The stroke color.</param>
/// <param name="strokeSize">The size, in pixels, of the stroke.</param>
/// <param name="cornerRadius">The corner radius.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::FillAndStrokeRect(const RECT& rectangle, const RGBQUAD& fillColor, const RGBQUAD& strokeColor, uint32_t strokeSize, uint32_t cornerRadius, bool highlight, bool transparent) {
	return FillAndStrokeRect(rectangle, Util::ColorToLong(fillColor, 255), Util::ColorToLong(strokeColor, 255), strokeSize, cornerRadius, highlight);
}

/// <summary>
/// Render text using a specific font and color.
/// </summary>
/// <param name="bounds">The bounding box for the text.</param>
/// <param name="text">The text to render.</param>
/// <param name="font">The font to use when rendering.</param>
/// <param name="color">The font color.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Text(const RECT& bounds, const std::string& text, const Font& font, uint32_t color, bool highlight, bool transparent) {
	return renderer_text_a(&bounds, text.c_str(), font.get(), color, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Render text using a specific font and color.
/// </summary>
/// <param name="bounds">The bounding box for the text.</param>
/// <param name="text">The text to render.</param>
/// <param name="font">The font to use when rendering.</param>
/// <param name="color">The font color.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Text(const RECT& bounds, const std::wstring& text, const Font& font, uint32_t color, bool highlight, bool transparent) {
	return renderer_text_w(&bounds, text.c_str(), font.get(), color, GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Render text using a specific font and color.
/// </summary>
/// <param name="bounds">The bounding box for the text.</param>
/// <param name="text">The text to render.</param>
/// <param name="font">The font to use when rendering.</param>
/// <param name="color">The font color.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Text(const RECT& bounds, const std::string& text, const Font& font, const RGBQUAD& color, bool highlight, bool transparent) {
	return Text(bounds, text, font, Util::ColorToLong(color, 255), highlight);
}

/// <summary>
/// Render text using a specific font and color.
/// </summary>
/// <param name="bounds">The bounding box for the text.</param>
/// <param name="text">The text to render.</param>
/// <param name="font">The font to use when rendering.</param>
/// <param name="color">The font color.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Text(const RECT& bounds, const std::wstring& text, const Font& font, const RGBQUAD& color, bool highlight, bool transparent) {
	return Text(bounds, text, font, Util::ColorToLong(color, 255), highlight);
}

/// <summary>
/// Render another image onto the output
/// </summary>
/// <param name="bounds">The image rectangle.</param>
/// <param name="image">The image to render.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Image(const RECT& bounds, std::shared_ptr<const TiffConvert::Image> image, bool highlight, bool transparent) {
	return renderer_image(&bounds, image->get(), GetHighlightFilter(highlight, transparent));
}

/// <summary>
/// Render another image onto the output at a specific opacity.
/// </summary>
/// <param name="bounds">The image rectangle.</param>
/// <param name="image">The image to render.</param>
/// <param name="alpha">The opacity / alpha channel for the image.</param>
/// <param name="highlight">Whether highlighting should be applied.</param>
/// <param name="transparent">Whether transparency masking is applied.</param>
/// <returns>True when successful, false otherwise.</returns>
bool Renderer::Image(const RECT& bounds, std::shared_ptr<const TiffConvert::Image> image, uint8_t alpha, bool highlight, bool transparent) {
	return renderer_image_alpha(&bounds, image->get(), alpha, GetHighlightFilter(highlight, transparent));
}