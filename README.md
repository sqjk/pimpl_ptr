Helper class for the Fast Pimpl Idiom
=====================================

The [Pimpl Idiom](http://herbsutter.com/gotw/_100/) is a great way to
reduce compilation dependencies.

However, you introduce an additional indirection, which leads you
performance penalties.  There is a hack that stores the internal data
into a char array. Look at [GotW #28](http://www.gotw.ca/gotw/028.htm)
why this is a bad idea.

How does the solution look like?
--------------------------------

The typical implementation of the pimpl idiom looks like the
following:

```cpp
class C {
  struct impl;
  std::unique_ptr<impl> pimpl;
public:
  test();
  test(test const&);
 ~test();
  test& operator=(test const&);
};
```
The functions need to be defined in the implementation file where
`C::impl` is known.

Sometimes, performance really matters and you don't want this
indirection.  Then, you want to reserve exactly as many bytes as the
`impl` class would use.  Here is how it looks like using the
`pimpl_ptr`:

```cpp
class C {
  struct impl;
  pimpl_ptr<impl, /* size to be reserved */> pimpl;
public:
  test();
  test(test const&);
 ~test();
  test& operator=(test const&);
};
```

`pimpl_ptr` can be declared without knowing about the implementation
of impl but can't be constructed, destructed, copied or moved.  So not
declaring for example a copy constructor results in a compiler error.
Also, giving the wrong `/* size to be reserved */`, a `static_assert`
fails and get another compiler error.

Opaque types in C++11
---------------------

C++11 introduces a template called `std::aligned_storage`.  This
allowes you to align the impl data in a standard-conformant way.

Here is how the `pimpl_ptr` could be defined:

```cpp
template <typename T, std::size_t Size, int Align>
class pimpl_ptr {
  typename std::aligned_storage<Size, Align>::type data;

public:
  template <typename... Args>
  pimpl_ptr(Args&&... args)
  { new (&data) T(std::forward<Args>(args)...); }
  pimpl_ptr(pimpl_ptr const& o) { new (&data) T(*o); }
  pimpl_ptr(pimpl_ptr&  o)      { new (&data) T(*o); }
  pimpl_ptr(pimpl_ptr&& o)      { new (&data) T(std::move(*o)); }
 ~pimpl_ptr() { reinterpret_cast<T*>(&data)->~T(); }

  pimpl_ptr& operator=(pimpl_ptr const& o) { **this = *o; return *this; }
  pimpl_ptr& operator=(pimpl_ptr&& o)      { **this = std::move(*o); return *this; }

  T      & operator*()        { return *reinterpret_cast<T      *>(&data); }
  T const& operator*()  const { return *reinterpret_cast<T const*>(&data); }
  T      * operator->()       { return &**this; }
  T const* operator->() const { return &**this; }

  friend inline void swap(pimpl_ptr& lhs, pimpl_ptr& rhs)
  { using std::swap; swap(*lhs, *rhs); }
};
```

In fact, the whole `pimpl_ptr` is a little bit more complicated.
First of all, every function has its own `static_assert` to ensure
that `Size` and `Align` was supplied right.  Furthermore, `Align` is
made an optional parameter.

You can enjoy the full code there: [pimpl_ptr.hh](https://github.com/sqjk/pimpl_ptr/blob/master/include/pimpl_ptr.hh).

When should I use this class?
-----------------------------

The `pimpl_ptr` encapsulates a hack.  And it still is a hack.  But
it's a hack made safe.

Using `impl` itself needs some effort.  Switching to from
`std::unique_ptr` to `pimpl_ptr` only involves minor changes.

The good thing is, that you can't make anything wrong if it compiles.
However, it will very often break compiling.

In sum, you can trade development and porting effort for runtime
performance.  Very often, this is not what you want.  So decide
wisely.

Is there a minimal working example?
-----------------------------------

Yes of course, feel free to look around in the [repository](https://github.com/sqjk/pimpl_ptr).
