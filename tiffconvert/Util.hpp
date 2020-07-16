#pragma once

#ifndef libtiffconvert_util_h
#define libtiffconvert_util_h

#include "libtiffconvert.h"
#include <string>
#include <vector>

namespace TiffConvert {
	/// <summary>
	/// Util is a class of static methods with useful utilities from the libtiffconvert library.
	/// </summary>
	class Util {
		public:
			/// <summary>
			/// Convert <see cref="std::string"/> to <see cref="std::wstring"/>.
			/// </summary>
			/// <param name="in">The input string.</param>
			/// <returns>The converted string.</returns>
			static std::wstring ToWideChar(const std::string& in);

			/// <summary>
			/// Convert <see cref="std::wstring"/> to <see cref="std::string"/>.
			/// </summary>
			/// <param name="in">The input string.</param>
			/// <returns>The converted string.</returns>
			static std::string ToChar(const std::wstring& in);

			/// <summary>
			/// Free a buffer previously allocated by an API call to libtiffconvert.
			/// </summary>
			/// <param name="in">The buffer to free.</param>
			static void Free(const void* in);

			/// <summary>
			/// Convert an <see cref="RGBQUAD"/> instance to 24-bit color.
			/// </summary>
			/// <param name="color">The <see cref="RGBQUAD"/> color to convert.</param>
			/// <returns>The 24-bit color.</returns>
			static uint32_t ColorToLong(const RGBQUAD& color);

			/// <summary>
			/// Convert an <see cref="RGBQUAD"/> instance to 32-bit color.
			/// </summary>
			/// <param name="color">The <see cref="RGBQUAD"/> color to convert.</param>
			/// <param name="alpha">The alpha value for the 32-bit color.</param>
			/// <returns>The 32-bit color.</returns>
			static uint32_t ColorToLong(const RGBQUAD& color, uint8_t alpha);

			/// <summary>
			/// Determine if a certain color is white.
			/// </summary>
			/// <param name="color">The color to check.</param>
			/// <param name="requireFullAlpha">Whether or not to require alpha=255 in the color to consider it to be white.</param>
			/// <returns>True when the color is white, false otherwise.</returns>
			static bool IsWhite(const uint32_t color, bool requireFullAlpha = false);

			/// <summary>
			/// Determine if a certain color is white.
			/// </summary>
			/// <param name="color">The color to check.</param>
			/// <returns>True when the color is white, false otherwise.</returns>
			static bool IsWhite(const RGBQUAD& color);

			/// <summary>
			/// Translate the coordinates in a point based on a bounds rectangle.
			/// </summary>
			/// <param name="bounds">The translation rectangle.</param>
			/// <param name="point">The point.</param>
			/// <param name="result">A reference to the resulting point.</param>
			static void TranslatePoint(const RECT& bounds, const POINT& point, POINT& result);

			/// <summary>
			/// Translate the coordinates in a point based on a bounds rectangle.
			/// </summary>
			/// <param name="bounds">The translation rectangle.</param>
			/// <param name="point">The point.</param>
			/// <returns>A new translated point.</returns>
			static POINT TranslatePoint(const RECT& bounds, const POINT& point);

			/// <summary>
			/// Translate the coordinates of all the points in a collection based on a bounds rectangle.
			/// </summary>
			/// <param name="bounds">The translation rectangle.</param>
			/// <param name="points">A collection of points to translate.</param>
			/// <param name="results">A reference to the resulting collection of points.</param>
			static void TranslatePoints(const RECT& bounds, const std::vector<POINT>& points, std::vector<POINT>& results);

			/// <summary>
			/// Translate the coordinates of all the points in a collection based on a bounds rectangle.
			/// </summary>
			/// <param name="bounds">The translation rectangle.</param>
			/// <param name="points">A collection of points to translate.</param>
			/// <returns>A collection of new translated points.</returns>
			static std::vector<POINT> TranslatePoints(const RECT& bounds, const std::vector<POINT>& points);
	};
}

#endif 