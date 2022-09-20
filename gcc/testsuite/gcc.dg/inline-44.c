/* Make sure we can't make main inline with -ffreestanding -fspecial-main. */
/* { dg-do compile } */
/* { dg-options "-std=gnu99 -ffreestanding -fspecial-main" } */

inline int main (void) { /* { dg-warning "cannot inline function" } */
	return 1;
}
