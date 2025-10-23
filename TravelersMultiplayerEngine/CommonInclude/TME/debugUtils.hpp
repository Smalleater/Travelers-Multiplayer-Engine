#ifndef TME_DEBUG_UTILS_HPP
#define TME_DEBUG_UTILS_HPP

#include <cassert>
#include <type_traits>

namespace tme
{
#ifdef NDEBUG
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) ((void)0)
#else
#define TME_ASSERT_REF_PTR_OR_COPIABLE(obj) \
	static_assert(RefPtrOrTriviallyCopiable<decltype(obj)>().m_value && #obj " is not a reference, a pointer, or copyable")
#endif
	template<typename T>
	struct RefPtrOrTriviallyCopiable
	{
		bool m_value = std::is_reference_v<T> || std::is_pointer_v<T> || std::is_trivially_copyable_v<T>;
	};
}

#endif
