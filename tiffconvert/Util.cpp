#include "Util.hpp"

using namespace TiffConvert;

/// <summary>
/// Convert <see cref="std::string"/> to <see cref="std::wstring"/>.
/// </summary>
/// <param name="in">The input string.</param>
/// <returns>The converted string.</returns>
std::wstring Util::ToWideChar(const std::string& in) {
	if (in.empty())
		return L"";

	auto result = util_to_wchar(in.c_str());
	if (!result)
		return L"";

	std::wstring str(result);
	Free(reinterpret_cast<const void*>(result));
	return str;
}

/// <summary>
/// Convert <see cref="std::wstring"/> to <see cref="std::string"/>.
/// </summary>
/// <param name="in">The input string.</param>
/// <returns>The converted string.</returns>
std::string Util::ToChar(const std::wstring& in) {
	if (in.empty())
		return "";

	auto result = util_to_char(in.c_str());
	if (!result)
		return "";

	std::string str(result);
	Free(reinterpret_cast<const void*>(result));
	return str;
}

/// <summary>
/// Free a buffer previously allocated by an API call to libtiffconvert.
/// </summary>
/// <param name="in">The buffer to free.</param>
void Util::Free(const void* in) {
	util_free_buffer(in);
}

/// <summary>
/// Convert an <see cref="RGBQUAD"/> instance to 24-bit color.
/// </summary>
/// <param name="color">The <see cref="RGBQUAD"/> color to convert.</param>
/// <returns>The 24-bit color.</returns>
uint32_t Util::ColorToLong(const RGBQUAD& color) {
	return util_rgbquad_to_24(&color);
}

/// <summary>
/// Convert an <see cref="RGBQUAD"/> instance to 32-bit color.
/// </summary>
/// <param name="color">The <see cref="RGBQUAD"/> color to convert.</param>
/// <param name="alpha">The alpha value for the 32-bit color.</param>
/// <returns>The 32-bit color.</returns>
uint32_t Util::ColorToLong(const RGBQUAD& color, uint8_t alpha) {
	return util_rgbquad_to_32(&color, alpha);
}

/// <summary>
/// Determine if a certain color is white.
/// </summary>
/// <param name="color">The color to check.</param>
/// <param name="requireFullAlpha">Whether or not to require alpha=255 in the color to consider it to be white.</param>
/// <returns>True when the color is white, false otherwise.</returns>
bool Util::IsWhite(const uint32_t color, bool requireFullAlpha) {
	return util_is_white(color, requireFullAlpha);
}

/// <summary>
/// Determine if a certain color is white.
/// </summary>
/// <param name="color">The color to check.</param>
/// <returns>True when the color is white, false otherwise.</returns>
bool Util::IsWhite(const RGBQUAD& color) {
	return util_is_white(Util::ColorToLong(color), false);
}

/// <summary>
/// Translate the coordinates in a point based on a bounds rectangle.
/// </summary>
/// <param name="bounds">The translation rectangle.</param>
/// <param name="point">The point.</param>
/// <param name="result">A reference to the resulting point.</param>
void Util::TranslatePoint(const RECT& bounds, const POINT& point, POINT& result) {
	return util_translate_point(&bounds, &point, &result);
}

/// <summary>
/// Translate the coordinates in a point based on a bounds rectangle.
/// </summary>
/// <param name="bounds">The translation rectangle.</param>
/// <param name="point">The point.</param>
/// <returns>A new translated point.</returns>
POINT Util::TranslatePoint(const RECT& bounds, const POINT& point) {
	POINT result;
	TranslatePoint(bounds, point, result);
	return result;
}

/// <summary>
/// Translate the coordinates of all the points in a collection based on a bounds rectangle.
/// </summary>
/// <param name="bounds">The translation rectangle.</param>
/// <param name="points">A collection of points to translate.</param>
/// <param name="results">A reference to the resulting collection of points.</param>
void Util::TranslatePoints(const RECT& bounds, const std::vector<POINT>& points, std::vector<POINT>& results) {
	results.resize(points.size());

	if (points.empty())
		return;

	util_translate_points(&bounds, &points[0], &results[0], static_cast<uint32_t>(points.size()));
}

/// <summary>
/// Translate the coordinates of all the points in a collection based on a bounds rectangle.
/// </summary>
/// <param name="bounds">The translation rectangle.</param>
/// <param name="points">A collection of points to translate.</param>
/// <returns>A collection of new translated points.</returns>
std::vector<POINT> Util::TranslatePoints(const RECT& bounds, const std::vector<POINT>& points) {
	std::vector<POINT> results;
	TranslatePoints(bounds, points, results);
	return results;
}
