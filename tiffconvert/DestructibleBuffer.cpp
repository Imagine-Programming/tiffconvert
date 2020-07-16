#include "DestructibleBuffer.hpp"
#include "libtiffconvert.h"

using namespace TiffConvert;

/// <summary>
/// Construct a new DestructibleBuffer from void pointer and size.
/// </summary>
/// <param name="ptr">Pointer to data.</param>
/// <param name="size">Size of data.</param>
DestructibleBuffer::DestructibleBuffer(void* ptr, size_t size) {
	m_Buffer = ptr;
	m_Size   = size;
}

/// <summary>
/// The destructor will free the data.
/// </summary>
DestructibleBuffer::~DestructibleBuffer() {
	if (m_Buffer != nullptr) {
		util_free_buffer(m_Buffer);
		m_Buffer = nullptr;
	}
}

/// <summary>
/// Get a const pointer to the data
/// </summary>
/// <returns>A const pointer to the data.</returns>
const void* DestructibleBuffer::get() const noexcept {
	return m_Buffer;
}

/// <summary>
/// Get a non-const pointer to the data
/// </summary>
/// <returns>A non-const pointer to the data.</returns>
void* DestructibleBuffer::get() noexcept {
	return m_Buffer;
}

/// <summary>
/// Get a typed const pointer to the data.
/// </summary>
/// <typeparam name="TPointer">The type of the pointer.</typeparam>
/// <returns>A const pointer to the data.</returns>
template <typename TPointer, std::enable_if_t<std::is_pointer_v<TPointer>, int>>
const TPointer DestructibleBuffer::getas() const noexcept {
	return reinterpret_cast<TPointer>(m_Buffer);
}

/// <summary>
/// Get a typed non-const pointer to the data.
/// </summary>
/// <typeparam name="TPointer">The type of the pointer.</typeparam>
/// <returns>A non-const pointer to the data.</returns>
template <typename TPointer, std::enable_if_t<std::is_pointer_v<TPointer>, int>>
TPointer DestructibleBuffer::getas() noexcept {
	return reinterpret_cast<const TPointer>(m_Buffer);
}

/// <summary>
/// Return the size of the data pointed to.
/// </summary>
/// <returns>The size of the data.</returns>
const size_t DestructibleBuffer::size() const noexcept {
	return m_Size;
}