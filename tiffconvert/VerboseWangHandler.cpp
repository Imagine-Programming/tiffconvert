#include "VerboseWangHandler.hpp"
#include <iostream>

using namespace TiffConvert::Handlers;
using namespace TiffConvert;

#pragma warning ( push )
#pragma warning ( disable: 4100 ) // unreferenced formal parameter, this is an event handler interface; not all parameters are used in every implementation

/// <summary>
/// Construct a new verbose handler
/// </summary>
/// <param name="colorful">use colors in output.</param>
VerboseWangHandler::VerboseWangHandler(std::shared_ptr<TiffConvert::Cli::VerbosePrinter> printer)
	: m_Printer(printer) {

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
void VerboseWangHandler::RenderLine(const RECT& bounds, const std::vector<POINT>& points, const RGBQUAD& color, uint32_t size, bool highlight, bool transparent) {
	m_Printer->Section("LINE", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Number("THICKNESS", size, "px");
		m_Printer->RgbQuad("COLOR", color);
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
		m_Printer->Points("POINTS", points);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The fill color to use.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void VerboseWangHandler::RenderRect(const RECT& bounds, const RGBQUAD& color, bool highlight, bool transparent) {
	m_Printer->Section("RECT", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->RgbQuad("COLOR", color);
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
	});
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
void VerboseWangHandler::RenderBorderedRect(const RECT& bounds, const RGBQUAD& color, const RGBQUAD& borderColor, uint32_t lineSize, bool highlight, bool transparent) {
	m_Printer->Section("BORDERED RECT", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Number("THICKNESS", lineSize, "px");
		m_Printer->RgbQuad("COLOR", color);
		m_Printer->RgbQuad("BORDER COLOR", borderColor);
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an outlined rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The stroke color to use.</param>
/// <param name="lineSize">The size of the outline.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void VerboseWangHandler::RenderOutlinedRect(const RECT& bounds, const RGBQUAD& color, uint32_t lineSize, bool highlight, bool transparent) {
	m_Printer->Section("BORDERED RECT", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Number("THICKNESS", lineSize, "px");
		m_Printer->RgbQuad("BORDER COLOR", color);
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void VerboseWangHandler::RenderText(const std::string& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	m_Printer->Section("ANSI TEXT", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->RgbQuad("COLOR", color);
		m_Printer->Text("FONT", font.lfFaceName);
		m_Printer->Number("POINTS", font.lfHeight, "pt");
		m_Printer->Number("SCALE", info.uCreationScale);
		m_Printer->Number("ORIENT", info.nCurrentOrientation);
		m_Printer->Number("LENGTH", info.uAnoTextLength, " characters");
		m_Printer->Text("TEXT", text);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void VerboseWangHandler::RenderText(const std::wstring& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	m_Printer->Section("UNICODE TEXT", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->RgbQuad("COLOR", color);
		m_Printer->Text("FONT", font.lfFaceName);
		m_Printer->Number("POINTS", font.lfHeight, "pt");
		m_Printer->Number("SCALE", info.uCreationScale);
		m_Printer->Number("ORIENT", info.nCurrentOrientation);
		m_Printer->Number("LENGTH", info.uAnoTextLength, " characters");
		m_Printer->Text("TEXT", text);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a bitmask.
/// <br/>
/// Note: the bitmask is applied to a full IFD (image) and should prevent anything from being rendered on a masked area.
/// </summary>
/// <param name="filename">The path to the bitmask image.</param>
/// <param name="bounds">The bounding rectangle.</param>
/// <param name="rotation">Rotation information.</param>
void VerboseWangHandler::RenderMask(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation) {
	m_Printer->Section("MASK", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Text("FILENAME", filename);
	});
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image reference.
/// </summary>
/// <param name="filename">The path to the image file to render.</param>
/// <param name="bounds">The image bounding box.</param>
/// <param name="rotation">Rotation information.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void VerboseWangHandler::RenderImageReference(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, bool highlight, bool transparent) {
	m_Printer->Section("IMAGE FILE", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
		m_Printer->Text("FILENAME", filename);
	});
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
void VerboseWangHandler::RenderImage(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, const std::vector<uint8_t>& data, bool highlight, bool transparent) {
	m_Printer->Section("IMAGE DATA", [&]() {
		m_Printer->Rectangle("BOUNDS", bounds);
		m_Printer->Number("SIZE", data.size(), " bytes");
		m_Printer->Boolean("HILITE", highlight);
		m_Printer->Boolean("TRANSP", transparent);
		m_Printer->Text("FILENAME", filename);
	});	
}

#pragma warning ( pop )