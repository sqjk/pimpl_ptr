#include <pimpl_ptr.hh>

class test {
public:
  struct impl;
  pimpl_ptr<impl, 2*sizeof(int)> pimpl;
public:
  test();
  test(int a);
  ~test();
  test(test const&);
  test& operator=(test const&);

  void set(int a);
  void print() const;

  void swap(test& o);
};
