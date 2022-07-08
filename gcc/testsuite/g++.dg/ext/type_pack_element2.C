// { dg-do compile { target c++11 } }
// { dg-options -w }

using X1 = __builtin_type_pack_element(1);  // { dg-error "one or more type" }
using X2 = __builtin_type_pack_element(1, 1); // { dg-error "expected type" }
int i;
using X3 = __builtin_type_pack_element(1, i); // { dg-error "does not name a type" }
using X4 = __builtin_type_pack_element(1, int); // { dg-error "out of range" }

using X5 = __builtin_type_pack_element(-1, int); // { dg-error "out of range" }
using X6 = __builtin_type_pack_element(nullptr, int); // { dg-error "integral" }

template<int N, class T>
struct uninstantiated_template
{
  using X = __builtin_type_pack_element(2, int); // { dg-error "out of range" }
  using Y = __builtin_type_pack_element(2, T);   // { dg-bogus "out of range" }
  using Z = __builtin_type_pack_element(N, int); // { dg-bogus "." }
};


template<int N, class T>
struct instantiated_template
{
  using Y = __builtin_type_pack_element(2, T); // { dg-error "out of range" }
  using Z = __builtin_type_pack_element(N, T); // { dg-error "out of range" }
};

using Y = typename instantiated_template<0, int>::Y;
using Z = typename instantiated_template<1, int>::Z;
// { dg-prune-output "invalid combination of multiple type-specifiers" }

template<class... T>
using bad = __builtin_type_pack_element(sizeof(T), T...); // { dg-error "packs not expanded" }
bad<int> b; // { dg-error "does not name a type" }
