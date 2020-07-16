#pragma once

#ifndef libtiffconvert_destructible_buffer_h
#define libtiffconvert_destructible_buffer_h

#include <cstdint>
#include <type_traits>

namespace TiffConvert {
	/// <summary>
	/// DestructibleBuffer is designed to be used in std::unique_ptr or std::shared_ptr and holds an arbitrary amount of data, 
	/// but destroys that data the moment it is destroyed. Like this, one can use the reference counting in a smart pointer to 
	/// keep track of a large blob of data.
	/// </summary>
	class DestructibleBuffer {
		private:
			void*	m_Buffer = nullptr;
			size_t	m_Size = 0;

		public:
			/// <summary>
			/// Construct a new DestructibleBuffer from void pointer and size.
			/// </summary>
			/// <param name="ptr">Pointer to data.</param>
			/// <param name="size">Size of data.</param>
			DestructibleBuffer(void* ptr, size_t size);

			/// <summary>
			/// The destructor will free the data.
			/// </summary>
			~DestructibleBuffer();

			/// <summary>
			/// Get a const pointer to the data.
			/// </summary>
			/// <returns>A const pointer to the data.</returns>
			const void* get() const noexcept;

			/// <summary>
			/// Get a non-const pointer to the data.
			/// </summary>
			/// <returns>A non-const pointer to the data.</returns>
			void* get() noexcept;

			/// <summary>
			/// Get a typed const pointer to the data.
			/// </summary>
			/// <typeparam name="TPointer">The type of the pointer.</typeparam>
			/// <returns>A const pointer to the data.</returns>
			template <typename TPointer, std::enable_if_t<std::is_pointer_v<TPointer>, int> = 0>
			const TPointer getas() const noexcept;
			
			/// <summary>
			/// Get a typed non-const pointer to the data.
			/// </summary>
			/// <typeparam name="TPointer">The type of the pointer.</typeparam>
			/// <returns>A non-const pointer to the data.</returns>
			template <typename TPointer, std::enable_if_t<std::is_pointer_v<TPointer>, int> = 0>
			TPointer getas() noexcept;

			/// <summary>
			/// Return the size of the data pointed to.
			/// </summary>
			/// <returns>The size of the data.</returns>
			const size_t size() const noexcept;
	};
}

#endif 