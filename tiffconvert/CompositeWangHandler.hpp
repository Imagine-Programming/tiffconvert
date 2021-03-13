#pragma once

#ifndef composite_wang_handler_h
#define composite_wang_handler_h

#include <IWangAnnotationCallback.hpp>
#include <TiffFile.hpp>

namespace TiffConvert {
	namespace Handlers {

		/// <summary>
		/// CompositeWangHandler is an implementation of <see cref="TiffWang::Tiff::IWangAnnotationCallback"/> that handles events 
		/// from the <see cref="Tiffwang::Tiff::WangAnnotationReader"/>. These events describe annotations that are to be rendered 
		/// onto a Tiff page, this implementation applies multiple handlers for these events.
		/// </summary>
		class CompositeWangHandler : public TiffWang::Tiff::IWangAnnotationCallback {
			private:
				using _Handler = std::shared_ptr<TiffWang::Tiff::IWangAnnotationCallback>;
				using _HandlerCollection = std::vector<_Handler>;

				_HandlerCollection m_Handlers;

			public:
				/// <summary>
				/// Construct a new composite handler from a collection of handlers.
				/// </summary>
				/// <param name="handlers">The handlers to add to this collection</param>
				CompositeWangHandler(const _HandlerCollection& handlers);

				/// <summary>
				/// Construct a new composite handler without any handlers added to it.
				/// </summary>
				CompositeWangHandler() = default;

				/// <summary>
				/// Add a handler to the composition.
				/// </summary>
				/// <param name="handler">The handler to add.</param>
				void Add(const _Handler& handler) noexcept;

				/// <summary>
				/// A callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a line mark.
				/// </summary>
				/// <param name="bounds">The translation of the points.</param>
				/// <param name="points">All the points on the line.</param>
				/// <param name="color">The color.</param>
				/// <param name="size">The size (thickness).</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderLine(const RECT& bounds, const std::vector<POINT>& points, const RGBQUAD& color, uint32_t size, bool highlight, bool transparent) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled rectangle mark.
				/// </summary>
				/// <param name="bounds">The bounds for the rectangle.</param>
				/// <param name="color">The fill color to use.</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderRect(const RECT& bounds, const RGBQUAD& color, bool highlight, bool transparent) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a filled and outlined rectangle mark.
				/// </summary>
				/// <param name="bounds">The bounds for the rectangle.</param>
				/// <param name="color">The fill color to use.</param>
				/// <param name="borderColor">The stroke color to use.</param>
				/// <param name="lineSize">The size of the outline.</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderBorderedRect(const RECT& bounds, const RGBQUAD& color, const RGBQUAD& borderColor, uint32_t lineSize, bool highlight, bool transparent) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an outlined rectangle mark.
				/// </summary>
				/// <param name="bounds">The bounds for the rectangle.</param>
				/// <param name="color">The stroke color to use.</param>
				/// <param name="lineSize">The size of the outline.</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderOutlinedRect(const RECT& bounds, const RGBQUAD& color, uint32_t lineSize, bool highlight, bool transparent) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
				/// </summary>
				/// <param name="text">The string of text.</param>
				/// <param name="bounds">The bounding box for the text, if available.</param>
				/// <param name="font">The font information.</param>
				/// <param name="info">The text information.</param>
				/// <param name="color">The color.</param>
				void RenderText(const std::string& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters text.
				/// </summary>
				/// <param name="text">The string of text.</param>
				/// <param name="bounds">The bounding box for the text, if available.</param>
				/// <param name="font">The font information.</param>
				/// <param name="info">The text information.</param>
				/// <param name="color">The color.</param>
				void RenderText(const std::wstring& text, const RECT& bounds, const LOGFONTA& font, const OIAN_TEXTPRIVDATA& info, const RGBQUAD& color) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters a bitmask.
				/// <br/>
				/// Note: the bitmask is applied to a full IFD (image) and should prevent anything from being rendered on a masked area.
				/// </summary>
				/// <param name="filename">The path to the bitmask image.</param>
				/// <param name="bounds">The bounding rectangle.</param>
				/// <param name="rotation">Rotation information.</param>
				void RenderMask(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image reference.
				/// </summary>
				/// <param name="filename">The path to the image file to render.</param>
				/// <param name="bounds">The image bounding box.</param>
				/// <param name="rotation">Rotation information.</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderImageReference(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, bool highlight, bool transparent) override;

				/// <summary>
				/// The callback method which will be invoked when the <see cref="WangAnnotationReader"/> encounters an image block (DIB data).
				/// </summary>
				/// <param name="filename">The path to the image file to render.</param>
				/// <param name="bounds">The image bounding box.</param>
				/// <param name="rotation">Rotation information.</param>
				/// <param name="data">The standard DIB image data. Prepend a BITMAPFILEHEADER struct and you'll have a BMP you can decode.</param>
				/// <param name="highlight">Whether or not the highlight filter should be applied (only render on white).</param>
				/// <param name="transparent">Whether or not the transparent filter should be applied (don't render white).</param>
				void RenderImage(const std::string& filename, const RECT& bounds, const AN_NEW_ROTATE_STRUCT& rotation, const std::vector<uint8_t>& data, bool highlight, bool transparent) override;
		};

	}
}

#endif 