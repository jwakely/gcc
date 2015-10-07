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

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// { dg-options "-std=gnu++14" }

#include <experimental/buffer>
#include <testsuite_hooks.h>

namespace net = std::experimental::net;

template<typename T>
bool is_mutable(const T&)
{ return std::is_same<T, net::mutable_buffer>::value; }

template<typename T>
bool is_const(const T&)
{ return std::is_same<T, net::const_buffer>::value; }

void
test01()
{
  bool test = false;

  auto b1 = net::buffer((void*)&test, sizeof(test));
  VERIFY( is_mutable(b1) );
  VERIFY( b1.data() == &test );
  VERIFY( b1.size() == sizeof(test) );

  auto b2 = net::buffer((const void*)&test, sizeof(test));
  VERIFY( is_const(b2) );
  VERIFY( b2.data() == &test );
  VERIFY( b1.size() == sizeof(test) );

  auto b3 = net::buffer(b1);
  VERIFY( is_mutable(b3) );
  VERIFY( b3.data() == b1.data() );
  VERIFY( b3.size() == b1.size() );

  auto b4 = net::buffer(b2);
  VERIFY( is_const(b4) );
  VERIFY( b4.data() == b2.data() );
  VERIFY( b4.size() == b2.size() );

  auto b5 = net::buffer(b1, 0);
  VERIFY( is_mutable(b5) );
  VERIFY( b5.data() == b1.data() );
  VERIFY( b5.size() == 0 );

  auto b6 = net::buffer(b2, 0);
  VERIFY( is_const(b6) );
  VERIFY( b6.data() == b2.data() );
  VERIFY( b6.size() == 0 );

}

int
main()
{
  test01();
}
