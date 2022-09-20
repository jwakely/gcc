/* Make sure we can't make main inline with -ffreestanding -fspecial-main. */
/* { dg-do compile } */
/* { dg-options "-ffreestanding -fspecial-main" } */

inline int main () { /* { dg-error "cannot declare .* to be inline" } */
	return 1;
}
