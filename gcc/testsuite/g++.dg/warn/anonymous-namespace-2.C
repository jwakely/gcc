// Test for the warning of exposing types from an anonymous namespace
// { dg-do compile }
//
#include "anonymous-namespace-2.h"

namespace {
    struct good { };
}

struct g1 {
    good * A;
};
struct g2 {
    good * A[1];
};
struct g3 {
    good (*A)[1];
};

#line 21 "foo.C"
struct b1 { // { dg-warning "has internal linkage" "" { target c++11 } }
// { dg-warning "uses the anonymous namespace" "" { target c++98_only } .-1 }
    bad * B;
};
struct b2 { // { dg-warning "has internal linkage" "" { target c++11 } }
// { dg-warning "uses the anonymous namespace" "" { target c++98_only } .-1 }
    bad * B[1];
};
struct b3 { // { dg-warning "has internal linkage" "" { target c++11 } }
// { dg-warning "uses the anonymous namespace" "" { target c++98_only } .-1 }
    bad (*B)[1];
};
