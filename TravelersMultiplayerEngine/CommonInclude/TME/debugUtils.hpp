#ifndef TME_DEBUG_UTILS_HPP
#define TME_DEBUG_UTILS_HPP

#include <cassert>
#include <type_traits>
#include <iostream>
#include <mutex>

namespace tme
{
	inline std::mutex m_logMutex;

#ifdef NDEBUG
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) ((void)0)
#define TME_LOG_DEBUG(fmt, ...) ((void)0)

#else
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) \
	static_assert(RefPtrOrTriviallyCopiable<decltype(obj)>().m_value && #obj " is not a reference, a pointer, or copyable")
#define TME_LOG_DEBUG(fmt, ...) \
	{ \
		std::lock_guard<std::mutex> lock(m_logMutex); \
		std::cout << "[DEBUG] " << printf(fmt, ##__VA_ARGS__) << std::endl; \
	}\

#endif

	template<typename T>
	struct RefPtrOrTriviallyCopiable
	{
		bool m_value = std::is_reference_v<T> || std::is_pointer_v<T> || std::is_trivially_copyable_v<T>;
	};
}

#endif
