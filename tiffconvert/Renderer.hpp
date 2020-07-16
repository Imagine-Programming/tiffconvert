#pragma once

#ifndef libtiffconvert_renderer_h
#define libtiffconvert_renderer_h

#include "libtiffconvert.h"
#include "Font.hpp"
#include "Image.hpp"
#include <vector>
#include <memory>

namespace TiffConvert {
	/// <summary>
	/// Renderer provides a class of static methods that help with rendering to an output created by libtiffconvert.
	/// </summary>
	class Renderer {
		public:
			/// <summary>
			/// The highlight filter only renders onto white.
			/// </summary>
			/// <param name="x">X coordinate of the pixel to filter.</param>
			/// <param name="y">Y coordinate of the pixel to filter.</param>
			/// <param name="top">The top (new) color.</param>
			/// <param name="bottom">The bottom (old) color.</param>
			/// <returns>The new filtered color.</returns>
			static uint64_t __stdcall HighlightFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom);

			/// <summary>
			/// The highlight-transparent filter only renders onto white and only renders if the new color is not white.
			/// </summary>
			/// <param name="x">X coordinate of the pixel to filter.</param>
			/// <param name="y">Y coordinate of the pixel to filter.</param>
			/// <param name="top">The top (new) color.</param>
			/// <param name="bottom">The bottom (old) color.</param>
			/// <returns>The new filtered color.</returns>
			static uint64_t __stdcall HighlightTransparentFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom);

			/// <summary>
			/// The transparent filter only renders the new color if it is not white.
			/// </summary>
			/// <param name="x">X coordinate of the pixel to filter.</param>
			/// <param name="y">Y coordinate of the pixel to filter.</param>
			/// <param name="top">The top (new) color.</param>
			/// <param name="bottom">The bottom (old) color.</param>
			/// <returns>The new filtered color.</returns>
			static uint64_t __stdcall TransparentFilter(uint64_t x, uint64_t y, uint64_t top, uint64_t bottom);

			/// <summary>
			/// Determines which filter to use based on the highlight and transparent state from the mark attributes.
			/// Returns nullptr if no filter should be applied.
			/// </summary>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>The render filter to use, or nullptr if no filtering should be applied.</returns>
			static renderer_filter GetHighlightFilter(bool highlight, bool transparent);

			/// <summary>
			/// Render a line between all the specified points, with a specific thickness and color.
			/// </summary>
			/// <param name="points">The list of points to draw a line in between.</param>
			/// <param name="lineSize">The thickness of the line.</param>
			/// <param name="color">The linecolor.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Line(const std::vector<POINT> points, uint32_t lineSize, uint32_t color, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Render a line between all the specified points, with a specific thickness and color.
			/// </summary>
			/// <param name="points">The list of points to draw a line in between.</param>
			/// <param name="lineSize">The thickness of the line.</param>
			/// <param name="color">The linecolor.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Line(const std::vector<POINT> points, uint32_t lineSize, const RGBQUAD& color, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Render a line between all the specified points, with a specific color.
			/// </summary>
			/// <param name="points">The list of points to draw a line in between.</param>
			/// <param name="color">The linecolor.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Line(const std::vector<POINT> points, uint32_t color, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Render a line between all the specified points, with a specific color.
			/// </summary>
			/// <param name="points">The list of points to draw a line in between.</param>
			/// <param name="color">The linecolor.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Line(const std::vector<POINT> points, const RGBQUAD& color, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Fill a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
			/// </summary>
			/// <param name="rectangle">The rectangle coordinates and sizes.</param>
			/// <param name="color">The fill color.</param>
			/// <param name="cornerRadius">The corner radius.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool FillRect(const RECT& rectangle, uint32_t color, uint32_t cornerRadius, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Fill a rectangle with a certain color and apply a corner radius to the rectangle if it not 0.
			/// </summary>
			/// <param name="rectangle">The rectangle coordinates and sizes.</param>
			/// <param name="color">The fill color.</param>
			/// <param name="cornerRadius">The corner radius.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool FillRect(const RECT& rectangle, const RGBQUAD& color, uint32_t cornerRadius, bool highlight = false, bool transparent = false);
			
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
			static bool StrokeRect(const RECT& rectangle, uint32_t color, uint32_t strokeSize, uint32_t cornerRadius, bool highlight = false, bool transparent = false);

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
			static bool StrokeRect(const RECT& rectangle, const RGBQUAD& color, uint32_t strokeSize, uint32_t cornerRadius, bool highlight = false, bool transparent = false);

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
			static bool FillAndStrokeRect(const RECT& rectangle, uint32_t fillColor, uint32_t strokeColor, uint32_t strokeSize, uint32_t cornerRadius, bool highlight = false, bool transparent = false);

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
			static bool FillAndStrokeRect(const RECT& rectangle, const RGBQUAD& fillColor, const RGBQUAD& strokeColor, uint32_t strokeSize, uint32_t cornerRadius, bool highlight = false, bool transparent = false);

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
			static bool Text(const RECT& bounds, const std::string& text, const Font& font, uint32_t color, bool highlight = false, bool transparent = false);

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
			static bool Text(const RECT& bounds, const std::wstring& text, const Font& font, uint32_t color, bool highlight = false, bool transparent = false);

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
			static bool Text(const RECT& bounds, const std::string& text, const Font& font, const RGBQUAD& color, bool highlight = false, bool transparent = false);

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
			static bool Text(const RECT& bounds, const std::wstring& text, const Font& font, const RGBQUAD& color, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Render another image onto the output.
			/// </summary>
			/// <param name="bounds">The image rectangle.</param>
			/// <param name="image">The image to render.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Image(const RECT& bounds, std::shared_ptr<const TiffConvert::Image> image, bool highlight = false, bool transparent = false);

			/// <summary>
			/// Render another image onto the output at a specific opacity.
			/// </summary>
			/// <param name="bounds">The image rectangle.</param>
			/// <param name="image">The image to render.</param>
			/// <param name="alpha">The opacity / alpha channel for the image.</param>
			/// <param name="highlight">Whether highlighting should be applied.</param>
			/// <param name="transparent">Whether transparency masking is applied.</param>
			/// <returns>True when successful, false otherwise.</returns>
			static bool Image(const RECT& bounds, std::shared_ptr<const TiffConvert::Image> image, uint8_t alpha, bool highlight = false, bool transparent = false);
	};
}

#endif 