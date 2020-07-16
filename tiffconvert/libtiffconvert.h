#pragma once

#ifndef libtiffconvert_h
#define libtiffconvert_h

#include <cstdint>
#include <Windows.h>

// define the method of importing API and calling conventions
#define __API  __declspec(dllimport)
#define __CONV __cdecl

#ifdef __cplusplus
extern "C" {
#endif 
	typedef struct __TIFF_IMAGE		tiff_image;		// internal tiff_image object
	typedef struct __FONT_HANDLE	font_handle;	// internal font object
	typedef struct __IMAGE_HANDLE	image_handle;	// internal image object

#pragma pack ( push, 1 )
	/// <summary>
	/// The Tiff-file header, describing byte order, magic number and offset to the first IFD.
	/// </summary>
	typedef struct __TIFF_HEADER {
		uint16_t	ByteOrder;
		uint16_t	Magic;
		uint32_t	IFD0;
	} tiff_header;
#pragma pack ( pop )

	/// <summary>
	/// The codecs supported by libtiffconvert.
	/// </summary>
	enum class tiff_export_format : uint32_t {
		TIFF_EXPORT_PNG,
		TIFF_EXPORT_JPEG,
		TIFF_EXPORT_JPEG2000,
		TIFF_EXPORT_BITMAP
	};

	/// <summary>
	/// The fixed rotation modes supported by libtiffconvert (optimized rotation).
	/// </summary>
	enum class image_rotation_mode : uint32_t {
		ROTATE_90,
		ROTATE_180,
		ROTATE_270
	};

	/// <summary>
	/// The mirroring modes supported by libtiffconvert.
	/// </summary>
	enum class image_mirror_mode : uint32_t {
		MIRROR_HORIZONTAL,
		MIRROR_VERTICAL
	};

	/* rendering filter prototype */
	typedef uint64_t (__stdcall *renderer_filter)(uint64_t x, uint64_t y, uint64_t source, uint64_t target);

	/* utility module */

	__API uint64_t			__CONV util_char_to_wchar(const char* in, const wchar_t* out, uint32_t inlen, uint32_t outlen); 
	__API uint64_t			__CONV util_wchar_to_char(const wchar_t* in, const char* out, uint32_t inlen, uint32_t outlen);
	__API const wchar_t*	__CONV util_to_wchar(const char* in);
	__API const char*		__CONV util_to_char(const wchar_t* in);
	__API void				__CONV util_free_buffer(const void* buffer);
	__API uint32_t			__CONV util_rgbquad_to_24(const RGBQUAD* color);
	__API uint32_t			__CONV util_rgbquad_to_32(const RGBQUAD* color, uint8_t alpha);
	__API void				__CONV util_translate_point(const RECT* bounds, const POINT* point, const POINT* result);
	__API void				__CONV util_translate_points(const RECT* bounds, const POINT points[], const POINT results[], uint32_t count);
	__API uint64_t			__CONV util_is_white(uint32_t color, uint32_t requireFullAlpha = false);

	/* tiff image module */

	__API tiff_image*		__CONV tiff_image_open_p(const tiff_header* buffer, uint32_t size, uint32_t release_raw = false);
	__API tiff_image*		__CONV tiff_image_open_a(const char* filename);
	__API tiff_image*		__CONV tiff_image_open_w(const wchar_t* filename);
	__API uint64_t			__CONV tiff_image_page_count(const tiff_image* handle);
	__API void				__CONV tiff_image_close(const tiff_image* handle);
	
	__API uint64_t			__CONV tiff_image_page_width(const tiff_image* handle, uint32_t page);
	__API uint64_t			__CONV tiff_image_page_height(const tiff_image* handle, uint32_t page);
	__API uint64_t			__CONV tiff_image_page_scale(const tiff_image* handle, uint32_t page, uint32_t maxwidth, uint32_t maxheight, uint32_t smooth);

	__API uint64_t			__CONV tiff_image_export_page_a(const tiff_image* handle, uint32_t page, const char* filename, tiff_export_format codec, uint32_t options);
	__API uint64_t			__CONV tiff_image_export_page_w(const tiff_image* handle, uint32_t page, const wchar_t* filename, tiff_export_format codec, uint32_t options);
	__API void*				__CONV tiff_image_export_page_p(const tiff_image* handle, uint32_t page, uint32_t* lpdwSize, tiff_export_format codec, uint32_t options);
	
	__API uint64_t			__CONV tiff_image_export_pdf_a(const tiff_image* handle, const char* filepath, tiff_export_format codec, uint32_t options);
	__API uint64_t			__CONV tiff_image_export_pdf_w(const tiff_image* handle, const wchar_t* filepath, tiff_export_format codec, uint32_t options);

	/* rendering module */

	__API HDC				__CONV renderer_begin(const tiff_image* handle, uint32_t page);
	__API void				__CONV renderer_stop();
	__API uint64_t			__CONV renderer_line(const POINT points[], uint32_t count, uint32_t size, uint32_t color, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_single_line(const POINT points[], uint32_t count, uint32_t color, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_rect(const RECT* bounds, uint32_t fillColor, uint32_t strokeColor, uint32_t fill, uint32_t stroke, uint32_t radius = 0, uint32_t strokeThickness = 0, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_text_a(const RECT* point, const char* text, const font_handle* font, uint32_t color, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_text_w(const RECT* point, const wchar_t* text, const font_handle* font, uint32_t color, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_image(const RECT* bounds, const image_handle* image, renderer_filter filter = nullptr);
	__API uint64_t			__CONV renderer_image_alpha(const RECT* bounds, const image_handle* image, uint8_t alpha, renderer_filter filter = nullptr);

	/* font module */

	__API font_handle*		__CONV font_open_a(const char* fontname, uint32_t height, uint32_t bold, uint32_t italic, uint32_t underline, uint32_t strikeout, uint32_t antialias);
	__API font_handle*		__CONV font_open_w(const wchar_t* fontname, uint32_t height, uint32_t bold, uint32_t italic, uint32_t underline, uint32_t strikeout, uint32_t antialias);
	__API void				__CONV font_close(const font_handle* font);

	/* image module */
	
	__API image_handle*		__CONV image_open_p(const uint8_t* buffer, uint32_t size);
	__API void				__CONV image_free(const image_handle* image);
	__API image_handle*		__CONV image_clone(const image_handle* image);
	__API image_handle*		__CONV image_rotate_fixed(const image_handle* image, image_rotation_mode mode);
	__API image_handle*		__CONV image_rotate_free(const image_handle* image, float degrees, uint32_t antiAlias, uint32_t fillColor);
	__API image_handle*		__CONV image_mirror(const image_handle* image, image_mirror_mode axis);
#ifdef __cplusplus
}
#endif 

#endif
