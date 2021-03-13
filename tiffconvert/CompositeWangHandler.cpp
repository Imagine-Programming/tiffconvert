#include "CompositeWangHandler.hpp"

using namespace TiffConvert::Handlers;
using namespace TiffConvert;

#pragma warning ( push )
#pragma warning ( disable: 4100 ) // unreferenced formal parameter, this is an event handler interface; not all parameters are used in every implementation

/// <summary>
/// Construct a new composite handler from a collection of handlers.
/// </summary>
/// <param name="handlers">The handlers to add to this collection</param>
CompositeWangHandler::CompositeWangHandler(const _HandlerCollection& handlers) 
	: m_Handlers(handlers) {

}

/// <summary>
/// Add a handler to the composition.
/// </summary>
/// <param name="handler">The handler to add.</param>
void CompositeWangHandler::Add(const _Handler& handler) noexcept {
	m_Handlers.push_back(handler);
}

/// <summary>
/// A callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a line mark.
/// </summary>
/// <param name="bounds">The translation of the points.</param>
/// <param name="points">All the points on the line.</param>
/// <param name="color">The color.</param>
/// <param name="size">The size (thickness).</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderLine(const RECT& bounds, const std::vector<POINT>& points, const RGBQUAD& color, uint32_t size, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderLine(bounds, points, color, size, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The fill color to use.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderRect(const RECT& bounds, const RGBQUAD& color, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderRect(bounds, color, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled and outlined rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The fill color to use.</param>
/// <param name="borderColor">The stroke color to use.</param>
/// <param name="lineSize">The size of the outline.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderBorderedRect(const RECT& bounds, const RGBQUAD& color, const RGBQUAD& borderColor, uint32_t lineSize, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderBorderedRect(bounds, color, borderColor, lineSize, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an outlined rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The stroke color to use.</param>
/// <param name="lineSize">The size of the outline.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderOutlinedRect(const RECT& bounds, const RGBQUAD& color, uint32_t lineSize, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderOutlinedRect(bounds, color, lineSize, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void CompositeWangHandler::RenderText(const std::string& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	for (const auto& handler : m_Handlers)
		handler->RenderText(text, bounds, font, info, color);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void CompositeWangHandler::RenderText(const std::wstring& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	for (const auto& handler : m_Handlers)
		handler->RenderText(text, bounds, font, info, color);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a bitmask.
/// <br/>
/// Note: the bitmask is applied to a full IFD (image) and should prevent anything from being rendered on a masked area.
/// </summary>
/// <param name="filename">The path to the bitmask image.</param>
/// <param name="bounds">The bounding rectangle.</param>
/// <param name="rotation">Rotation information.</param>
void CompositeWangHandler::RenderMask(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation) {
	for (const auto& handler : m_Handlers)
		handler->RenderMask(filename, bounds, rotation);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image reference.
/// </summary>
/// <param name="filename">The path to the image file to render.</param>
/// <param name="bounds">The image bounding box.</param>
/// <param name="rotation">Rotation information.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderImageReference(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderImageReference(filename, bounds, rotation, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image block (DIB data).
/// </summary>
/// <param name="filename">The path to the image file to render.</param>
/// <param name="bounds">The image bounding box.</param>
/// <param name="rotation">Rotation information.</param>
/// <param name="data">The standard DIB image data. Prepend a BITMAPFILEHEADER struct and you'll have a BMP you can decode.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void CompositeWangHandler::RenderImage(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, const std::vector<uint8_t>& data, bool highlight, bool transparent) {
	for (const auto& handler : m_Handlers)
		handler->RenderImage(filename, bounds, rotation, data, highlight, transparent);
}

#pragma warning ( pop )