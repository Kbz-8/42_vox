#ifndef __SCOPE_UTILS_NON_OWNING_PTR__
#define __SCOPE_UTILS_NON_OWNING_PTR__

namespace Scop
{
	template<typename T>
	class NonOwningPtr
	{
		public:
			NonOwningPtr(T* ptr = nullptr);
			NonOwningPtr(const NonOwningPtr&) = default;
			NonOwningPtr(NonOwningPtr&& ptr) noexcept;

			NonOwningPtr& operator=(T* ptr);
			NonOwningPtr& operator=(const NonOwningPtr&) = default;
			NonOwningPtr& operator=(NonOwningPtr&& ptr) noexcept;

			[[nodiscard]] inline operator bool() const noexcept;

			[[nodiscard]] inline T* Get() const noexcept;
			[[nodiscard]] inline T* operator->() const noexcept;
			[[nodiscard]] inline T& operator*() const noexcept;

			~NonOwningPtr() = default;

		private:
			T* m_ptr = nullptr;
	};
}

#include <Utils/NonOwningPtr.inl>

#endif
