#ifndef __SCOP_CPU_BUFFER__
#define __SCOP_CPU_BUFFER__

#include <cstdint>
#include <cstring>
#include <memory>
#include <Core/Logs.h>

namespace Scop
{
	class CPUBuffer
	{
		public:
			CPUBuffer() {}
			CPUBuffer(std::size_t size) try : m_data(size > 0 ? std::make_unique<std::uint8_t[]>(size) : nullptr), m_size(size)
			{}
			catch(...)
			{
				FatalError("memory allocation for a CPU buffer failed");
			}

			CPUBuffer(const CPUBuffer& other) try : m_data(other.m_size > 0 ? std::make_unique<std::uint8_t[]>(other.m_size) : nullptr), m_size(other.m_size)
			{
				if(m_data)
					std::memcpy(m_data.get(), other.m_data.get(), m_size);
			}
			catch (...)
			{
				FatalError("memory allocation for a CPU buffer failed");
			}

			CPUBuffer& operator=(const CPUBuffer& other)
			{
				if(this != &other)
				{
					if(other.m_data)
					{
						std::unique_ptr<std::uint8_t[]> new_data;
						try
						{
							new_data = std::make_unique<std::uint8_t[]>(other.m_size);
						}
						catch (...)
						{
							FatalError("memory allocation for a CPU buffer failed");
						}
						std::memcpy(new_data.get(), other.m_data.get(), other.m_size);

						m_data = std::move(new_data);
						m_size = other.m_size;
					}
					else
					{
						m_data.reset();
						m_size = 0;
					}
				}
				return *this;
			}

			[[nodiscard]] inline CPUBuffer Duplicate() const
			{
				CPUBuffer buffer(m_size);
				if(m_data)
					std::memcpy(buffer.GetData(), m_data.get(), m_size);
				return buffer;
			}

			inline void Allocate(std::size_t size)
			{
				if(m_data != nullptr)
					FatalError("cannot allocate an already allocated CPU buffer");
				try
				{
					m_data = std::make_unique<std::uint8_t[]>(size);
					m_size = size;
				}
				catch(...)
				{
					FatalError("memory allocation for a CPU buffer failed");
				}
			}

			inline bool Empty() const { return m_size == 0; }

			[[nodiscard]] inline std::size_t GetSize() const noexcept { return m_size; }

			template<typename T>
			[[nodiscard]] inline T* GetDataAs() const { return reinterpret_cast<T*>(m_data.get()); }
			[[nodiscard]] inline std::uint8_t* GetData() const { return m_data.get(); }
			inline operator bool() const { return (bool)m_data; }

			~CPUBuffer() = default;

		private:
			std::unique_ptr<std::uint8_t[]> m_data;
			std::size_t m_size = 0;
	};
}

#endif
