/* { dg-do compile } */
/* Verify that store mappings match Table A.6's recommended mapping.  */
/* { dg-options "-O3" } */
/* { dg-remove-options riscv_ztso } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */
/* { dg-final { check-function-bodies "**" "" } } */

/*
** atomic_store_long_release:
**	l[wd]\t[atx][0-9]+,0\(a1\)
**	fence\trw,w
**	s[wd]\t[atx][0-9]+,0\(a0\)
**	ret
*/
void atomic_store_long_release (long* bar, long* baz)
{
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}

/*
** atomic_store_int_release:
**	lw\t[atx][0-9]+,0\(a1\)
**	fence\trw,w
**	sw\t[atx][0-9]+,0\(a0\)
**	ret
*/
void atomic_store_int_release (int* bar, int* baz)
{
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}

/*
** atomic_store_short_release:
**	lhu\t[atx][0-9]+,0\(a1\)
**	fence\trw,w
**	sh\t[atx][0-9]+,0\(a0\)
**	ret
*/
void atomic_store_short_release (short* bar, short* baz)
{
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}

/*
** atomic_store_char_release:
**	lbu\t[atx][0-9]+,0\(a1\)
**	fence\trw,w
**	sb\t[atx][0-9]+,0\(a0\)
**	ret
*/
void atomic_store_char_release (char* bar, char* baz)
{
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}

/*
** atomic_store_bool_release:
**	lbu\t[atx][0-9]+,0\(a1\)
**	fence\trw,w
**	sb\t[atx][0-9]+,0\(a0\)
**	ret
*/
void atomic_store_bool_release (_Bool* bar, _Bool* baz)
{
  __atomic_store(bar, baz, __ATOMIC_RELEASE);
}
