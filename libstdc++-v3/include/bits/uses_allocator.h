// Uses-allocator Construction -*- C++ -*-

// Copyright (C) 2010-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _USES_ALLOCATOR_H
#define _USES_ALLOCATOR_H 1

#if __cplusplus < 201103L
# include <bits/c++0x_warning.h>
#else

#include <type_traits>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /// [allocator.tag]
  struct allocator_arg_t { };

  /// A tag of type allocator_arg_t.
  constexpr allocator_arg_t allocator_arg = allocator_arg_t();

  template<typename _Tp, typename _Alloc, typename = __void_t<>>
    struct __uses_allocator_helper
    : false_type { };

  template<typename _Tp, typename _Alloc>
    struct __uses_allocator_helper<_Tp, _Alloc,
				   __void_t<typename _Tp::allocator_type>>
    : is_convertible<_Alloc, typename _Tp::allocator_type>::type
    { };

  /// [allocator.uses.trait]
  template<typename _Tp, typename _Alloc>
    struct uses_allocator
    : __uses_allocator_helper<_Tp, _Alloc>::type
    { };

  struct __uses_alloc_base { };

  /// A tag type indicating construction without an allocator.
  struct __uses_alloc0 : __uses_alloc_base
  {
    struct _Sink { void operator=(const void*) { } } _M_a;
  };

  /// A tag type indicating construction with allocator_arg_t.
  template<typename _Alloc>
    struct __uses_alloc1 : __uses_alloc_base { const _Alloc* _M_a; };

  /// A tag type indicating construction with an allocator argument at the end.
  template<typename _Alloc>
    struct __uses_alloc2 : __uses_alloc_base { const _Alloc* _M_a; };

  template<bool, typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc;

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<true, _Tp, _Alloc, _Args...>
    : conditional<
        is_constructible<_Tp, allocator_arg_t, _Alloc, _Args...>::value,
        __uses_alloc1<_Alloc>,
       	__uses_alloc2<_Alloc>>::type
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<false, _Tp, _Alloc, _Args...>
    : __uses_alloc0 { };

  /// A tag type indicating whether/how to construct with an allocator.
  template<typename _Tp, typename _Alloc, typename... _Args>
    using __uses_alloc_t =
      __uses_alloc<uses_allocator<_Tp, _Alloc>::value, _Tp, _Alloc, _Args...>;

  /// Make a tag type indicating how to use an allocator for construction.
  template<typename _Tp, typename _Alloc, typename... _Args>
    inline __uses_alloc_t<_Tp, _Alloc, _Args...>
    __use_alloc(const _Alloc& __a)
    {
      __uses_alloc_t<_Tp, _Alloc, _Args...> __ret;
      __ret._M_a = &__a;
      return __ret;
    }

  /// Check for required Allocator functions (only as an unevaluated operand).
  template<typename _Alloc>
    auto
    __can_allocate(_Alloc* __a)
    -> decltype(__a->deallocate(__a->allocate(1u), 1u));

  /// Primary template handles all cases that don't look like Allocators.
  template<typename _Alloc, typename = __void_t<>>
    struct __is_allocator_impl
    : false_type { };

  /// Specialization recognizes types that define value_type and can allocate.
  template<typename _Alloc>
    struct __is_allocator_impl<_Alloc,
			       __void_t<typename _Alloc::value_type,
				        decltype(__can_allocate<_Alloc>(0))>>
    : true_type { };

  /// Detect whether a type might be an Allocator.
  template<typename _Alloc>
    struct __is_allocator : __is_allocator_impl<_Alloc>::type
    { };

  /// Alias for allocator_arg_t that is only valid if _Alloc is an Allocator.
  template<typename _Alloc>
    using __alloc_arg_t = typename enable_if<__is_allocator<_Alloc>::value,
					     allocator_arg_t>::type;

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif
#endif
