#include "PreRenderWangHandler.hpp"
#include "Renderer.hpp"
#include "Util.hpp"
#include "Font.hpp"
#include <iostream>

using namespace TiffConvert::Handlers;
using namespace TiffConvert;

#pragma warning ( push )
#pragma warning ( disable: 4100 ) // unreferenced formal parameter, this is an event handler interface; not all parameters are used in every implementation

/// <summary>
/// Construct a new PreRenderWangHandler from a reference to a tiff page dimensions instance and device context.
/// This handler burns eiStream/Wang annotations onto the current page, so that the annotations will not have to 
/// be parsed by any viewer anymore.
/// </summary>
/// <param name="dimensions">A reference to the dimensions of the current page.</param>
/// <param name="hDc">The device context for rendering, created by <see cref="TiffConvert::Renderer"/>.</param>
PreRenderWangHandler::PreRenderWangHandler(const TiffWang::Tiff::TiffDimensions& dimensions, const HDC hDc)
	: m_Dimensions(dimensions), m_hDC(hDc) {

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
void PreRenderWangHandler::RenderLine(const RECT& bounds, const std::vector<POINT>& points, const RGBQUAD& color, uint32_t size, bool highlight, bool transparent) {
	Renderer::Line(Util::TranslatePoints(bounds, points), size, Util::ColorToLong(color, 255), highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The fill color to use.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void PreRenderWangHandler::RenderRect(const RECT& bounds, const RGBQUAD& color, bool highlight, bool transparent) {
	Renderer::FillRect(bounds, Util::ColorToLong(color, 255), 0, highlight, transparent);
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
void PreRenderWangHandler::RenderBorderedRect(const RECT& bounds, const RGBQUAD& color, const RGBQUAD& borderColor, uint32_t lineSize, bool highlight, bool transparent) {
	Renderer::FillAndStrokeRect(bounds, Util::ColorToLong(color, 255), Util::ColorToLong(borderColor, 255), lineSize, 0, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an outlined rectangle mark.
/// </summary>
/// <param name="bounds">The bounds for the rectangle.</param>
/// <param name="color">The stroke color to use.</param>
/// <param name="lineSize">The size of the outline.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void PreRenderWangHandler::RenderOutlinedRect(const RECT& bounds, const RGBQUAD& color, uint32_t lineSize, bool highlight, bool transparent) {
	Renderer::StrokeRect(bounds, Util::ColorToLong(color, 255), lineSize, 0, highlight, transparent);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void PreRenderWangHandler::RenderText(const std::string& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	// TODO: implement nCurrentOrientation with TextRotated

	std::unique_ptr<Font> renderFont = nullptr;
	auto mutableFontInfo = font;

	mutableFontInfo.lfHeight = CalculateFontHeight(mutableFontInfo, info);

	try {
		renderFont = std::make_unique<Font>(mutableFontInfo);
	} catch (const std::exception&) {
		try {
			renderFont = std::make_unique<Font>("Arial", mutableFontInfo.lfHeight, Font::FlagsFromLogFont(mutableFontInfo));
		} catch (const std::exception& ex) {
			throw ex;
		}
	}

	Renderer::Text(bounds, text, *renderFont.get(), color);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
/// </summary>
/// <param name="text">The string of text.</param>
/// <param name="bounds">The bounding box for the text, if available.</param>
/// <param name="font">The font information.</param>
/// <param name="info">The text information.</param>
/// <param name="color">The color.</param>
void PreRenderWangHandler::RenderText(const std::wstring& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) {
	// TODO: implement nCurrentOrientation with TextRotated

	std::unique_ptr<Font> renderFont = nullptr;
	auto mutableFontInfo = font;

	mutableFontInfo.lfHeight = CalculateFontHeight(mutableFontInfo, info);

	try {
		renderFont = std::make_unique<Font>(mutableFontInfo);
	} catch (const std::exception&) {
		try {
			renderFont = std::make_unique<Font>("Arial", mutableFontInfo.lfHeight, Font::FlagsFromLogFont(mutableFontInfo));
		} catch (const std::exception& ex) {
			throw ex;
		}
	}

	Renderer::Text(bounds, text, *renderFont.get(), color);
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a bitmask.
/// <br/>
/// Note: the bitmask is applied to a full IFD (image) and should prevent anything from being rendered on a masked area.
/// </summary>
/// <param name="filename">The path to the bitmask image.</param>
/// <param name="bounds">The bounding rectangle.</param>
/// <param name="rotation">Rotation information.</param>
void PreRenderWangHandler::RenderMask(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation) {
	std::cout << "[WARN] forms (image masks) are not supported as they refer to a file on the filesystem, which is not considered secure.\n";
}

/// <summary>
/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image reference.
/// </summary>
/// <param name="filename">The path to the image file to render.</param>
/// <param name="bounds">The image bounding box.</param>
/// <param name="rotation">Rotation information.</param>
/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
void PreRenderWangHandler::RenderImageReference(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, bool highlight, bool transparent) {
	std::cout << "[WARN] images by reference are not supported as they refer to a file on the filesystem, which is not considered secure.\n";
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
void PreRenderWangHandler::RenderImage(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, const std::vector<uint8_t>& data, bool highlight, bool transparent) {
	// filename is to be ignored, it's the name of the original file that was embedded.

	std::shared_ptr<Image> image = nullptr;
	try {
		image = std::make_shared<Image>(data, true);
	} catch (const std::runtime_error&) {
		std::cout << "[WARN] the embedded image named '" << filename << "' could not be decoded by the implementation of libtiffconvert, codec might not be supported.\n";
		return;
	}

	try {
		switch (rotation.rotation) {
			case AN_ROTATE_TYPE::RotateRight:
				image = image->RotateFixed(image_rotation_mode::ROTATE_90);
				break;
			case AN_ROTATE_TYPE::Flip:
				image = image->RotateFixed(image_rotation_mode::ROTATE_180);
				break;
			case AN_ROTATE_TYPE::RotateLeft:
				image = image->RotateFixed(image_rotation_mode::ROTATE_270);
				break;
			case AN_ROTATE_TYPE::VerticalMirror:
				image = image->Mirror(image_mirror_mode::MIRROR_VERTICAL);
				break;
			case AN_ROTATE_TYPE::VerticalMirrorRotateRight:
				image = image->Mirror(image_mirror_mode::MIRROR_VERTICAL);
				image = image->RotateFixed(image_rotation_mode::ROTATE_90);
				break;
			case AN_ROTATE_TYPE::VerticalMirrorFlip:
				image = image->Mirror(image_mirror_mode::MIRROR_VERTICAL);
				image = image->RotateFixed(image_rotation_mode::ROTATE_180);
				break;
			case AN_ROTATE_TYPE::VerticalMirrorRotateLeft:
				image = image->Mirror(image_mirror_mode::MIRROR_VERTICAL);
				image = image->RotateFixed(image_rotation_mode::ROTATE_270);
				break;
		}
	} catch (const std::runtime_error&) {
		std::cout << "[WARN] the embedded image named '" << filename << "' should be rotated, flipped or mirrored. This operation could not be done, image is not rendered.\n";
		return;
	}

	Renderer::Image(bounds, image, highlight, transparent);
}

/// <summary>
/// Translate the lfHeight member of <paramref name="font"/> to a scaled value representable on the 
/// current output device context, based on the uCreationScale member in <paramref name="info"/>.
/// </summary>
/// <param name="font">The font to use.</param>
/// <param name="info">Scaling information.</param>
/// <returns>The translated font size, truncated (i.e. 43.75 becomes 43, required by libtiffconvert)</returns>
uint32_t PreRenderWangHandler::CalculateFontHeight(const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info) {
	auto dpiY		= GetDeviceCaps(m_hDC, LOGPIXELSY); /* get vertical resolution of the output device context */
	auto scale		= static_cast<double>(info.uCreationScale);
	auto lfHeight	= static_cast<double>(font.lfHeight);
	auto tscale		= 72000.0 / dpiY; /* target scale, 72000 divided by vertical res of output */
	auto tscalef	= tscale / scale; /* target scale factor */

	// return the scaled vertical size of the font.
	return static_cast<uint32_t>(lfHeight * tscalef);
}

#pragma warning ( pop )