#include "test.hh"
#include <iostream>

struct test::impl {
  impl(int a=0, int b=0)
    : a(a), b(b)
  {
    std::cout << "constructed\n";
  }
  
  impl(const impl& o)
    : a(o.a), b(o.b)
  {
    std::cout << "copyconstructed\n";
  }

  impl& operator=(impl const& o)
  {
    a = o.a; b = o.b;
    std::cout << "copied\n";
    return *this;
  }

  ~impl()
  {
    std::cout << "destructed\n";
  }

  int a, b;
};

void swap(test::impl& l, test::impl& r)
{
  std::cout << "swapped\n";
  std::swap(l.a, r.a);
  std::swap(l.b, r.b);
}

test::test() =default;
test::test(int a) : pimpl(a) {}
test::~test() =default;
test::test(test const&) =default;
test& test::operator=(test const&) =default;

void test::set(int a) 
{
  pimpl->a = a; ++pimpl->b;
}

void test::print() const
{
  std::cout << pimpl->a << " " << pimpl->b << '\n';
}

void test::swap(test& o)
{
  using std::swap; swap(pimpl, o.pimpl);
}
