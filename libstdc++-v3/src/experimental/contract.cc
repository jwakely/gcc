// -*- C++ -*- std::experimental::contract_violation and friends

// Copyright (C) 2019-2022 Free Software Foundation, Inc.
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#include <experimental/contract>
#if _GLIBCXX_HOSTED && _GLIBCXX_VERBOSE
# include <iostream>
#endif

__attribute__ ((weak)) void
handle_contract_violation (const std::experimental::contract_violation &violation)
{
#if _GLIBCXX_HOSTED && _GLIBCXX_VERBOSE
  const char* modes[]{ "never", "maybe" }; // Must match enumerators in header.
  std::cerr << "contract violation in function " << violation.function_name()
    << " at " << violation.file_name() << ':' << violation.line_number()
    << ": " << violation.comment()
    << "\n[level:" << violation.assertion_level()
    << ", role:" << violation.assertion_role() << ", continuation mode:"
    << modes[(int)violation.continuation_mode()] << ']'
    << std::endl;
#endif
}
