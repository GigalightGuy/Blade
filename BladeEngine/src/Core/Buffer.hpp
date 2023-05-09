#pragma once

#include <stdint.h>
#include <cstring>

namespace BladeEngine {

	struct Buffer
	{
		uint8_t* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(uint64_t size)
		{
			Allocate(size);
		}

		void Allocate(uint64_t size)
		{
			Release();

			Data = new uint8_t[size];
			Size = size;
		}

		void Release()
		{
			delete[] Data;
			Data = nullptr;
			Size = 0;
		}

		template <typename T>
		T* As()
		{
			return (T*)Data;
		}

		static Buffer Copy(Buffer other)
		{
			Buffer buffer(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);
			return buffer;
		}

		static Buffer Copy(uint8_t* data, uint64_t size)
		{
			Buffer buffer(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

	};

}
