#include "Image.hpp"
#include "Util.hpp"
#include <stdexcept>

using namespace TiffConvert;

/// <summary>
/// Construct an Image from an existing image pointer.
/// </summary>
/// <param name="img">The image pointer.</param>
Image::Image(const image_handle* img)
	: m_Image(img) {

}

/// <summary>
/// Construct an image from a vector of bytes, effectively decoding the image.
/// </summary>
/// <param name="data">The raw image data.</param>
Image::Image(const std::vector<uint8_t> data, bool isDib) {
	if (!isDib) {
		auto image = image_open_p(&data[0], static_cast<uint32_t>(data.size()));
		if (!image)
			throw std::runtime_error("cannot decode image from memory, could be an unsupported codec");
		m_Image = image;

		return;
	}

    std::vector<uint8_t> bitmap(data.size() + sizeof(BITMAPFILEHEADER));
    auto pbfh = reinterpret_cast<BITMAPFILEHEADER*>(&bitmap[0]);
    auto pbmi = reinterpret_cast<BITMAPINFO*>(&bitmap[0] + sizeof(BITMAPFILEHEADER));
    memcpy(reinterpret_cast<void*>(pbmi), reinterpret_cast<const void*>(&data[0]), data.size());

    if (!pbmi->bmiHeader.biClrUsed && pbmi->bmiHeader.biBitCount != 24) 
        pbmi->bmiHeader.biClrUsed = (1 << pbmi->bmiHeader.biBitCount);

    uint32_t dwDIBSize = pbmi->bmiHeader.biSize + (pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD));
    uint32_t dwBmBitsSize = 0;

    if (pbmi->bmiHeader.biCompression == BI_RLE8 || pbmi->bmiHeader.biCompression == BI_RLE4) {
        dwDIBSize += pbmi->bmiHeader.biSizeImage;
    } else {
        dwBmBitsSize = (((pbmi->bmiHeader.biWidth * pbmi->bmiHeader.biBitCount) + 31) / 32 * 4) * pbmi->bmiHeader.biHeight;
        dwDIBSize += dwBmBitsSize;
        pbmi->bmiHeader.biSizeImage = dwBmBitsSize;
    }

    pbfh->bfType = ('M' << 8) | 'B';
    pbfh->bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
    pbfh->bfReserved1 = 0;
    pbfh->bfReserved2 = 0;
    pbfh->bfOffBits = sizeof(BITMAPFILEHEADER) + pbmi->bmiHeader.biSize + pbmi->bmiHeader.biClrUsed * sizeof(RGBQUAD);

    auto image = image_open_p(&bitmap[0], static_cast<uint32_t>(bitmap.size()));
    if (!image)
        throw std::runtime_error("cannot decode bitmap from DIB block");
    m_Image = image;
}

/// <summary>
/// Image destructor, will free the data allocated by the API.
/// </summary>
Image::~Image() {
	if (m_Image != nullptr)
		image_free(m_Image);
}

/// <summary>
/// Get the image handle contained within this object.
/// </summary>
/// <returns>A pointer to the internal structure describing this image.</returns>
const image_handle* Image::get() const noexcept {
	return m_Image;
}

/// <summary>
/// Clone the image, effectively creating a new independant instance.
/// </summary>
/// <returns>A new std::shared_ptr to a new Image instance.</returns>
std::shared_ptr<Image> Image::Clone() const {
	auto copy = image_clone(m_Image);
	if (!copy)
		throw std::runtime_error("cannot clone image");
	return std::make_shared<Image>(copy);
}

/// <summary>
/// Clone and rotate the image by 90, 180 or 270 degrees.
/// </summary>
/// <param name="mode">The rotation mode.</param>
/// <returns>A new std::shared_ptr to a new Image instance.</returns>
std::shared_ptr<Image> Image::RotateFixed(image_rotation_mode mode) const {
	auto copy = image_rotate_fixed(m_Image, mode);
	if (!copy)
		throw std::runtime_error("cannot rotate image");
	return std::make_shared<Image>(copy);
}

/// <summary>
/// Clone and rotate the image by any angle.
/// </summary>
/// <param name="fDegrees">The rotation in degrees (0-360).</param>
/// <param name="antiAlias">Whether or not to anti-alias the edges.</param>
/// <param name="fillColor">The fill color for the background on angled rotations.</param>
/// <returns>A new std::shared_ptr to a new Image instance.</returns>
std::shared_ptr<Image> Image::RotateFree(float fDegrees, bool antiAlias, uint32_t fillColor) const {
	auto copy = image_rotate_free(m_Image, fDegrees, antiAlias, fillColor);
	if (!copy)
		throw std::runtime_error("cannot rotate image");
	return std::make_shared<Image>(copy);
}

/// <summary>
/// Clone and rotate the image by any angle.
/// </summary>
/// <param name="fDegrees">The rotation in degrees (0-360).</param>
/// <param name="antiAlias">Whether or not to anti-alias the edges.</param>
/// <param name="fillColor">The fill color for the background on angled rotations.</param>
/// <returns>A new std::shared_ptr to a new Image instance.</returns>
std::shared_ptr<Image> Image::RotateFree(float fDegrees, bool antiAlias, const RGBQUAD& fillColor) const {
	return RotateFree(fDegrees, antiAlias, Util::ColorToLong(fillColor, 255));
}

/// <summary>
/// Clone and mirror the image.
/// </summary>
/// <param name="mode">The mirroring mode.</param>
/// <returns>A new std::shared_ptr to a new Image instance.</returns>
std::shared_ptr<Image> Image::Mirror(image_mirror_mode mode) const {
	auto copy = image_mirror(m_Image, mode);
	if (!copy)
		throw std::runtime_error("cannot mirror image");
	return std::make_shared<Image>(copy);
}