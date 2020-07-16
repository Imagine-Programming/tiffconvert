#pragma once

#ifndef libtiffconvert_image_h
#define libtiffconvert_image_h

#include "libtiffconvert.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <Windows.h>

namespace TiffConvert {
	class Image {
		private:
			const image_handle* m_Image = nullptr;

		public:
			/// <summary>
			/// Construct an Image from an existing image pointer.
			/// </summary>
			/// <param name="img">The image pointer.</param>
			Image(const image_handle* img);

			/// <summary>
			/// Construct an image from a vector of bytes, effectively decoding the image.
			/// </summary>
			/// <param name="data">The raw image data.</param>
			Image(const std::vector<uint8_t> data, bool isDib = false);

			/// <summary>
			/// Image destructor, will free the data allocated by the API.
			/// </summary>
			~Image();

			/// <summary>
			/// Get the image handle contained within this object.
			/// </summary>
			/// <returns>A pointer to the internal structure describing this image.</returns>
			const image_handle* get() const noexcept;

			/// <summary>
			/// Clone the image, effectively creating a new independant instance.
			/// </summary>
			/// <returns>A new std::shared_ptr to a new Image instance.</returns>
			std::shared_ptr<Image> Clone() const;

			/// <summary>
			/// Clone and rotate the image by 90, 180 or 270 degrees.
			/// </summary>
			/// <param name="mode">The rotation mode.</param>
			/// <returns>A new std::shared_ptr to a new Image instance.</returns>
			std::shared_ptr<Image> RotateFixed(image_rotation_mode mode) const;

			/// <summary>
			/// Clone and rotate the image by any angle.
			/// </summary>
			/// <param name="fDegrees">The rotation in degrees (0-360).</param>
			/// <param name="antiAlias">Whether or not to anti-alias the edges.</param>
			/// <param name="fillColor">The fill color for the background on angled rotations.</param>
			/// <returns>A new std::shared_ptr to a new Image instance.</returns>
			std::shared_ptr<Image> RotateFree(float fDegrees, bool antiAlias, uint32_t fillColor) const;

			/// <summary>
			/// Clone and rotate the image by any angle.
			/// </summary>
			/// <param name="fDegrees">The rotation in degrees (0-360).</param>
			/// <param name="antiAlias">Whether or not to anti-alias the edges.</param>
			/// <param name="fillColor">The fill color for the background on angled rotations.</param>
			/// <returns>A new std::shared_ptr to a new Image instance.</returns>
			std::shared_ptr<Image> RotateFree(float fDegrees, bool antiAlias, const RGBQUAD& fillColor) const;

			/// <summary>
			/// Clone and mirror the image.
			/// </summary>
			/// <param name="mode">The mirroring mode.</param>
			/// <returns>A new std::shared_ptr to a new Image instance.</returns>
			std::shared_ptr<Image> Mirror(image_mirror_mode mode) const;
	};
}

#endif 