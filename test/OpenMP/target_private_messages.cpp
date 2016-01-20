// RUN: %clang_cc1 -verify -fopenmp %s

struct S1; // expected-note 2 {{declared here}} expected-note 2 {{forward declaration of 'S1'}}
extern S1 a;
class S2 {
  mutable int a;

public:
  S2() : a(0) {}
};
const S2 b;
const S2 ba[5];
class S3 {
  int a;

public:
  S3() : a(0) {}
};
const S3 ca[5];
class S4 {
  int a;
  S4(); // expected-note {{implicitly declared private here}}

public:
  S4(int v) : a(v) {}
};
class S5 {
  int a;
  S5() : a(0) {} // expected-note {{implicitly declared private here}}

public:
  S5(int v) : a(v) {}
};

S3 h;
#pragma omp threadprivate(h) // expected-note 2 {{defined as threadprivate or thread local}}

template <class I, class C>
int foomain(I argc, C **argv) {
  I e(4);
  I g(5);
  int i;
  int &j = i;
#pragma omp target private // expected-error {{expected '(' after 'private'}}
#pragma omp target private( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private() // expected-error {{expected expression}}
#pragma omp target private(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
#pragma omp target private(argc)
#pragma omp target private(S1) // expected-error {{'S1' does not refer to a value}}
#pragma omp target private(a, b) // expected-error {{private variable with incomplete type 'S1'}}
#pragma omp target private(argv[1]) // expected-error {{expected variable name}}
#pragma omp target private(e, g)
#pragma omp target private(h) // expected-error {{threadprivate or thread local variable cannot be private}}
#pragma omp target shared(i) // expected-error {{unexpected OpenMP clause 'shared' in directive '#pragma omp target'}}
#pragma omp parallel
  {
    int v = 0;
    int i;
#pragma omp target private(i)
    {}
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp target private(j)
#pragma omp target private(i)
  {}
  return 0;
}

void bar(S4 a[2]) {
#pragma omp parallel
#pragma omp target private(a)
  {}
}

namespace A {
double x;
#pragma omp threadprivate(x) // expected-note {{defined as threadprivate or thread local}}
}
namespace B {
using A::x;
}

int main(int argc, char **argv) {
  S4 e(4);
  S5 g(5);
  int i;
  int &j = i;
#pragma omp target private // expected-error {{expected '(' after 'private'}}
#pragma omp target private( // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private() // expected-error {{expected expression}}
#pragma omp target private(argc // expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private(argc, // expected-error {{expected expression}} expected-error {{expected ')'}} expected-note {{to match this '('}}
#pragma omp target private(argc > 0 ? argv[1] : argv[2]) // expected-error {{expected variable name}}
#pragma omp target private(argc)
#pragma omp target private(S1) // expected-error {{'S1' does not refer to a value}}
#pragma omp target private(a, b) // expected-error {{private variable with incomplete type 'S1'}}
#pragma omp target private(argv[1]) // expected-error {{expected variable name}}
#pragma omp target private(e, g) // expected-error {{calling a private constructor of class 'S4'}} expected-error {{calling a private constructor of class 'S5'}}
#pragma omp target private(h) // expected-error {{threadprivate or thread local variable cannot be private}}
#pragma omp target private(B::x) // expected-error {{threadprivate or thread local variable cannot be private}}
#pragma omp target shared(i) // expected-error {{unexpected OpenMP clause 'shared' in directive '#pragma omp target'}}
#pragma omp parallel
  {
    int i;
#pragma omp target private(i)
    {}
  }
#pragma omp parallel shared(i)
#pragma omp parallel private(i)
#pragma omp target private(j)
#pragma omp target private(i)
  {}
  static int si;
#pragma omp target private(si) // OK
  {}
  return 0;
}
