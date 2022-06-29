// { dg-do compile { target c++11 } }

template<class, class> struct is_same { static constexpr bool value = false; };
template<class T> struct is_same<T,T> { static constexpr bool value = true; };

static_assert( is_same<__builtin_type_pack_element(0, int), int>::value, "" );
static_assert( is_same<__builtin_type_pack_element(0, long), long>::value, "" );
static_assert( is_same<__builtin_type_pack_element(1, float, char, int), char>::value, "" );

template<int N, class... T>
using Nth_type = __builtin_type_pack_element(N, T...);

static_assert( is_same<Nth_type<0, int>, int>::value, "" );
static_assert( is_same<Nth_type<0, long>, long>::value, "" );
static_assert( is_same<Nth_type<1, float, char, int>, char>::value, "" );

template<int N>
struct Nth_type_class_template
{
  using type = __builtin_type_pack_element(N, int, void, char, float, long);
};

static_assert( is_same<typename Nth_type_class_template<0>::type, int>::value, "" );
static_assert( is_same<typename Nth_type_class_template<1>::type, void>::value, "" );
