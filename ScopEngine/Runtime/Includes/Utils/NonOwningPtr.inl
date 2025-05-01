#pragma once
#include <Utils/NonOwningPtr.h>

namespace Scop
{
	template<typename T>
	NonOwningPtr<T>::NonOwningPtr(T* ptr) : m_ptr(ptr) {}

	template<typename T>
	NonOwningPtr<T>::NonOwningPtr(NonOwningPtr<T>&& ptr) noexcept : m_ptr(ptr.m_ptr)
	{
		ptr.m_ptr = nullptr;
	}

	template<typename T>
	NonOwningPtr<T>& NonOwningPtr<T>::operator=(T* ptr)
	{
		m_ptr = ptr;
		return *this;
	}

	template<typename T>
	NonOwningPtr<T>& NonOwningPtr<T>::operator=(NonOwningPtr&& ptr) noexcept
	{
		m_ptr = ptr.m_ptr;
		ptr.m_ptr = nullptr;
		return *this;
	}

	template<typename T>
	NonOwningPtr<T>::operator bool() const noexcept
	{
		return m_ptr != nullptr;
	}

	template<typename T>
	T* NonOwningPtr<T>::Get() const noexcept
	{
		return m_ptr;
	}

	template<typename T>
	T* NonOwningPtr<T>::operator->() const noexcept
	{
		return m_ptr;
	}

	template<typename T>
	T& NonOwningPtr<T>::operator*() const noexcept
	{
		return *m_ptr;
	}
}
