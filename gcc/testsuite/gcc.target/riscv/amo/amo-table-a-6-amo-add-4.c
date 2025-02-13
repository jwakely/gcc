/* { dg-do compile } */
/* Verify that atomic op mappings match Table A.6's recommended mapping.  */
/* { dg-options "-O3" } */
/* { dg-add-options riscv_zaamo } */
/* { dg-remove-options riscv_ztso } */
/* { dg-skip-if "" { *-*-* } { "-g" "-flto"} } */
/* { dg-final { check-function-bodies "**" "" } } */

/*
** foo:
**	amoadd\.w\.aqrl\tzero,a1,0\(a0\)
**	ret
*/
void foo (int* bar, int baz)
{
  __atomic_add_fetch(bar, baz, __ATOMIC_ACQ_REL);
}
