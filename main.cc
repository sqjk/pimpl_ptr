#include <iostream>
#include "test.hh"

int main()
{
  test t; // constructed
  t.print();
  t.set(4);
  t.print();
  {
    test t2(-1); // constructed
    t2.swap(t);  // swapped
    t2.print();
    t2.set(2);
    t2.print();
  } // destructed
  t.print();
} // destructed
