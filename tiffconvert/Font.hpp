#pragma once

#ifndef libtiffconvert_font_h
#define libtiffconvert_font_h

#include "libtiffconvert.h"
#include <string>
#include <cstdint>
#include <Windows.h>

namespace TiffConvert {
	/// <summary>
	/// Flags that specify how a font should be loaded.
	/// </summary>
	enum FontConfig : uint32_t {
		FONT_BOLD		= (1 << 0),
		FONT_ITALIC		= (1 << 1),
		FONT_UNDERLINE  = (1 << 2),
		FONT_STRIKEOUT	= (1 << 3),
		FONT_ANTIALIAS	= (1 << 4)
	};

	/// <summary>
	/// Font describes a font that is loaded through libtiffconvert.
	/// </summary>
	class Font {
		private:
			const font_handle* m_Font = nullptr;

		public:
			/// <summary>
			/// Convert a LOGFONTA descriptor to usable flags.
			/// </summary>
			/// <param name="descriptor">The LOGFONTA instance.</param>
			/// <param name="hq">Whether or not anti-aliasing should be applied to the font.</param>
			/// <returns>Combined flags.</returns>
			static uint32_t FlagsFromLogFont(const LOGFONTA& descriptor, bool hq = true);

			/// <summary>
			/// Construct a new font by name, height and flags.
			/// </summary>
			/// <param name="name">Font family name.</param>
			/// <param name="height">Font height, in points.</param>
			/// <param name="flags">Font style flags.</param>
			Font(const std::string& name, uint32_t height, uint32_t flags = 0);

			/// <summary>
			/// Construct a new font by name, height and flags.
			/// </summary>
			/// <param name="name">Font family name.</param>
			/// <param name="height">Font height, in points.</param>
			/// <param name="flags">Font style flags.</param>
			Font(const std::wstring& name, uint32_t height, uint32_t flags = 0);

			/// <summary>
			/// Construct a new font from LOGFONTA descriptor.
			/// </summary>
			/// <param name="descriptor">The LOGFONTA instance.</param>
			Font(const LOGFONTA& descriptor);

			/// <summary>
			/// The destructor frees the internal font.
			/// </summary>
			~Font();

			/// <summary>
			/// Get a const pointer to the internal font object.
			/// </summary>
			/// <returns>A const pointer to the internal font object.</returns>
			const font_handle* get() const noexcept;
	};
}

#endif 