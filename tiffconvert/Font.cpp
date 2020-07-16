#include "Font.hpp"
#include "Util.hpp"
#include <stdexcept>
#include <string>

using namespace TiffConvert;

/// <summary>
/// Convert a LOGFONTA descriptor to usable flags.
/// </summary>
/// <param name="descriptor">The LOGFONTA instance.</param>
/// <param name="hq">Whether or not anti-aliasing should be applied to the font.</param>
/// <returns>Combined flags.</returns>
uint32_t Font::FlagsFromLogFont(const LOGFONTA& descriptor, bool hq) {
	uint32_t flags = hq ? FontConfig::FONT_ANTIALIAS : 0;

	if (descriptor.lfWeight > FW_MEDIUM)
		flags |= FontConfig::FONT_BOLD;

	if (descriptor.lfItalic)
		flags |= FontConfig::FONT_ITALIC;

	if (descriptor.lfUnderline)
		flags |= FontConfig::FONT_UNDERLINE;

	if (descriptor.lfStrikeOut)
		flags |= FontConfig::FONT_STRIKEOUT;

	return flags;
}

/// <summary>
/// Construct a new font by name, height and flags.
/// </summary>
/// <param name="name">Font family name.</param>
/// <param name="height">Font height, in points.</param>
/// <param name="flags">Font style flags.</param>
Font::Font(const std::string& name, uint32_t height, uint32_t flags) {
	auto font = font_open_a(
		name.c_str(),
		height,
		flags & FontConfig::FONT_BOLD,
		flags & FontConfig::FONT_ITALIC,
		flags & FontConfig::FONT_UNDERLINE,
		flags & FontConfig::FONT_STRIKEOUT,
		flags & FontConfig::FONT_ANTIALIAS);

	if (!font)
		throw std::runtime_error("cannot open font " + name);

	m_Font = font;
}

/// <summary>
/// Construct a new font by name, height and flags.
/// </summary>
/// <param name="name">Font family name.</param>
/// <param name="height">Font height, in points.</param>
/// <param name="flags">Font style flags.</param>
Font::Font(const std::wstring& name, uint32_t height, uint32_t flags) {
	auto font = font_open_w(
		name.c_str(),
		height,
		flags & FontConfig::FONT_BOLD,
		flags & FontConfig::FONT_ITALIC,
		flags & FontConfig::FONT_UNDERLINE,
		flags & FontConfig::FONT_STRIKEOUT,
		flags & FontConfig::FONT_ANTIALIAS);

	if (!font)
		throw std::runtime_error("cannot open font " + Util::ToChar(name));

	m_Font = font;
}

/// <summary>
/// Construct a new font from LOGFONTA descriptor.
/// </summary>
/// <param name="descriptor">The LOGFONTA instance.</param>
Font::Font(const LOGFONTA& descriptor) 
	: Font(descriptor.lfFaceName, descriptor.lfHeight, FlagsFromLogFont(descriptor)) {

}

/// <summary>
/// The destructor frees the internal font.
/// </summary>
Font::~Font() {
	if (m_Font)
		font_close(m_Font);
}

/// <summary>
/// Get a const pointer to the internal font object.
/// </summary>
/// <returns>A const pointer to the internal font object.</returns>
const font_handle* Font::get() const noexcept {
	return m_Font;
}