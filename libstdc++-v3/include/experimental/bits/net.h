// Networking implementation details -*- C++ -*-

// Copyright (C) 2015 Free Software Foundation, Inc.
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

/** @file experimental/bits/net.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{experimental/networking}
 */

#ifndef _GLIBCXX_EXPERIMENTAL_NET_H
#define _GLIBCXX_EXPERIMENTAL_NET_H 1

#pragma GCC system_header

#if __cplusplus < 201402L
# include <bits/c++14_warning.h>
#else

#include <type_traits>
#include <experimental/netfwd>

namespace std _GLIBCXX_VISIBILITY(default)
{
namespace experimental
{
namespace net
{
inline namespace v1
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  /**
   * @ingroup networking
   * @{
   */

  template<typename _CompletionToken, typename _Signature, typename>
    class async_result;

  template<typename _CompletionToken, typename _Signature>
    using __deduced_t = typename
      async_result<decay_t<_CompletionToken>, _Signature, void>::return_type;

  // Trait to check for construction from const/non-const lvalue/rvalue.
  template<typename _Tp>
    using __is_value_constructible = typename __and_<
      is_copy_constructible<_Tp>, is_move_constructible<_Tp>,
      is_constructible<_Tp, _Tp&>, is_constructible<_Tp, const _Tp&&>
      >::type;

  /// @}

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#endif // C++14

#endif // _GLIBCXX_EXPERIMENTAL_NET_H
