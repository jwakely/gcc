// random -*- C++ -*-

// Copyright (C) 2012-2018 Free Software Foundation, Inc.
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

#define _GLIBCXX_USE_CXX11_ABI 1
#include <random>

#ifdef  _GLIBCXX_USE_C99_STDINT_TR1

#if defined __i386__ || defined __x86_64__
# include <cpuid.h>
# ifdef _GLIBCXX_X86_RDRAND
#  define USE_RDRAND 1
# endif
# ifdef _GLIBCXX_X86_RDSEED
#  define USE_RDSEED 1
# endif
#endif

#include <cerrno>
#include <cstdio>

#ifdef _GLIBCXX_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef _GLIBCXX_HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef _GLIBCXX_HAVE_LINUX_TYPES_H
# include <linux/types.h>
#endif

#ifdef _GLIBCXX_HAVE_LINUX_RANDOM_H
# include <linux/random.h>
#endif

#ifdef __WINNT__
# include <array>
# include <windows.h>
# include <ntsecapi.h>
# define USE_RTLGENRANDOM
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
  namespace
  {
#if USE_RDRAND
    unsigned int
    __attribute__ ((target("rdrnd")))
    __x86_rdrand(void*)
    {
      unsigned int retries = 100;
      unsigned int val;

      while (__builtin_ia32_rdrand32_step(&val) == 0)
	if (--retries == 0)
	  std::__throw_runtime_error(__N("random_device::__x86_rdrand(void)"));

      return val;
    }
#endif

#if USE_RDSEED
    unsigned int
    __attribute__ ((target("rdseed")))
    __x86_rdseed(void*)
    {
      unsigned int retries = 100;
      unsigned int val;

      while (__builtin_ia32_rdseed_si_step(&val) == 0)
	if (--retries == 0)
	  std::__throw_runtime_error(__N("random_device::__x86_rdseed(void)"));

      return val;
    }
#endif

#ifdef USE_RTLGENRANDOM
    struct rtlgenrandom_type
    {
      using result_type = random_device::result_type;
      using array = std::array<result_type, 128>;
      array data;
      array::const_iterator next = data.end();

      static result_type _S_gen(void* p)
      { return static_cast<rtlgenrandom_type*>(p)->_M_gen(); }

      result_type _M_gen()
      {
	if (next == data.end())
	  {
	    if (!RtlGenRandom(data.data(), data.size() * sizeof(result_type)))
	      __throw_runtime_error(__N("random_device got an error from"
					" RtlGenRandom"));
	    next = data.begin();
	  }
	return *next++;
      }

      ~rtlgenrandom_type()
      { SecureZeroMemory(data.data(), data.size() * sizeof(result_type)); }
    };
#endif
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-label"
  void
  random_device::_M_init(const std::string& token)
  {
    const char* fname = nullptr;

    if (token == "default")
      {
#if defined USE_RTLGENRANDOM
	goto use_rtlgenrandom;
#elif defined USE_RDRAND
	goto use_rdrand;
#elif defined USE_RDSEED
	goto use_rdseed;
#else
	fname = "/dev/urandom";
	goto use_device_file;
#endif
      }
#ifdef USE_RDRAND
    else if (token == "rdrand" || token == "rdrnd")
      {
use_rdrand:
	unsigned int eax, ebx, ecx, edx;
	// Check availability of cpuid and, for now at least, also the
	// CPU signature for Intel's
	if (__get_cpuid_max(0, &ebx) > 0 && ebx == signature_INTEL_ebx)
	  {
	    __cpuid(1, eax, ebx, ecx, edx);
	    if (ecx & bit_RDRND)
	      {
		_M_func = &__x86_rdrand;
		_M_file = nullptr;
		return;
	      }
	  }
      }
#endif
#ifdef USE_RDSEED
    else if (token == "rdseed")
      {
use_rdseed:
	unsigned int eax, ebx, ecx, edx;
	// Check availability of cpuid and, for now at least, also the
	// CPU signature for Intel's
	if (__get_cpuid_max(0, &ebx) > 0 && ebx == signature_INTEL_ebx)
	  {
	    __cpuid(1, eax, ebx, ecx, edx);
	    if (ebx & bit_RDSEED)
	      {
		_M_func = &__x86_rdseed;
		_M_file = nullptr;
		return;
	      }
	  }
      }
#endif
#ifdef USE_RTLGENRANDOM
    else if (token == "rtlgenrandom")
      {
use_rtlgenrandom:
	_M_file = new (&_M_buf) rtlgenrandom_type;
	_M_func = &rtlgenrandom_type::_S_gen;
	return;
      }
#endif
    else if (token == "/dev/urandom" || token == "/dev/random")
      {
	fname = token.c_str();
use_device_file:
	_M_file = static_cast<void*>(std::fopen(fname, "rb"));
	if (_M_file)
	  return;
      }
    else
      std::__throw_runtime_error(
	  __N("random_device::random_device(const std::string&):"
	      " device not recognized"));

    std::__throw_runtime_error(
	__N("random_device::random_device(const std::string&):"
	    " device not available"));
  }
#pragma GCC diagnostic pop

  void
  random_device::_M_init_pretr1(const std::string& token [[gnu::unused]])
  {
#ifdef USE_RTLGENRANDOM
    _M_init("rtlgenrandom");
#else
    unsigned long seed = 5489UL;
    if (token != "mt19937")
      {
	const char* nptr = token.c_str();
	char* endptr;
	seed = std::strtoul(nptr, &endptr, 0);
	if (*nptr == '\0' || *endptr != '\0')
	  std::__throw_runtime_error(__N("random_device::_M_strtoul"
					 "(const std::string&)"));
      }
    _M_mt.seed(seed);
#endif
  }

  void
  random_device::_M_fini()
  {
    if (!_M_file)
      return;

#ifdef USE_RTLGENRANDOM
    if (_M_func)
      {
	static_cast<rtlgenrandom_type*>(_M_file)->~rtlgenrandom_type();
	_M_func = nullptr;
	_M_file = nullptr;
	return;
      }
#endif

    std::fclose(static_cast<FILE*>(_M_file));
    _M_file = nullptr;
  }

  random_device::result_type
  random_device::_M_getval()
  {
#if defined USE_RDRAND || defined USE_RDSEED || defined USE_RTLGENRANDOM
    if (_M_func)
      return _M_func(_M_file);
#endif

    result_type __ret;
    void* p = &__ret;
    size_t n = sizeof(result_type);
#ifdef _GLIBCXX_HAVE_UNISTD_H
    do
      {
	const int e = read(fileno(static_cast<FILE*>(_M_file)), p, n);
	if (e > 0)
	  {
	    n -= e;
	    p = static_cast<char*>(p) + e;
	  }
	else if (e != -1 || errno != EINTR)
	  __throw_runtime_error(__N("random_device could not be read"));
      }
    while (n > 0);
#else
    const size_t e = std::fread(p, n, 1, static_cast<FILE*>(_M_file));
    if (e != 1)
      __throw_runtime_error(__N("random_device could not be read"));
#endif

    return __ret;
  }

  random_device::result_type
  random_device::_M_getval_pretr1()
  {
#ifdef USE_RTLGENRANDOM
    return _M_getval();
#else
    return _M_mt();
#endif
  }

  double
  random_device::_M_getentropy() const noexcept
  {
#if defined USE_RDRAND || defined USE_RDSEED || defined USE_RTLGENRANDOM
    if (_M_func)
      return 0.0;
#endif

#if defined _GLIBCXX_HAVE_SYS_IOCTL_H && defined RNDGETENTCNT
    if (!_M_file)
      return 0.0;

    const int fd = fileno(static_cast<FILE*>(_M_file));
    if (fd < 0)
      return 0.0;

    int ent;
    if (ioctl(fd, RNDGETENTCNT, &ent) < 0)
      return 0.0;

    if (ent < 0)
      return 0.0;

    const int max = sizeof(result_type) * __CHAR_BIT__;
    if (ent > max)
      ent = max;

    return static_cast<double>(ent);
#else
    return 0.0;
#endif
  }

  template class mersenne_twister_engine<
    uint_fast32_t,
    32, 624, 397, 31,
    0x9908b0dfUL, 11,
    0xffffffffUL, 7,
    0x9d2c5680UL, 15,
    0xefc60000UL, 18, 1812433253UL>;
}
#endif
