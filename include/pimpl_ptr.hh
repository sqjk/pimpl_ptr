#ifndef PIMPL_PTR_HH_
#define PIMPL_PTR_HH_

#include <type_traits>
#include <utility>
#include <memory>
#include <iostream>

namespace detail {
  template <std::size_t Size, int Align>
  struct aligned_storage {
    typedef typename std::aligned_storage<Size, Align>::type type;
  };

  template <std::size_t Size>
  struct aligned_storage<Size, -1> {
    typedef typename std::aligned_storage<Size>::type type;
  };
}

template <typename T, std::size_t Size, int Align=-1>
class pimpl_ptr {
  typename detail::aligned_storage<Size, Align>::type data;

public:
#define PIMPL_PTR_CHECK_()                                             \
  static_assert(sizeof(T) == Size, "pimpl_ptr: T has a different size than given"); \
  static_assert(alignof(T) >= std::alignment_of<T>::value,              \
                "pimpl_ptr: T has a more restrict alignment than given")

  template <typename... Args>
  pimpl_ptr(Args&&... args)
  { PIMPL_PTR_CHECK_(); new (&data) T(std::forward<Args>(args)...); }
  pimpl_ptr(pimpl_ptr const& o) { PIMPL_PTR_CHECK_(); new (&data) T(*o); }
  pimpl_ptr(pimpl_ptr&  o)      { PIMPL_PTR_CHECK_(); new (&data) T(*o); }
  pimpl_ptr(pimpl_ptr&& o)      { PIMPL_PTR_CHECK_(); new (&data) T(std::move(*o)); }
  ~pimpl_ptr() { PIMPL_PTR_CHECK_(); reinterpret_cast<T*>(&data)->~T(); }

  pimpl_ptr& operator=(pimpl_ptr const& o) { PIMPL_PTR_CHECK_(); **this = *o; return *this; }
  pimpl_ptr& operator=(pimpl_ptr&& o)      { PIMPL_PTR_CHECK_(); **this = std::move(*o); return *this; }

  T      & operator*()        { PIMPL_PTR_CHECK_(); return *reinterpret_cast<T      *>(&data); }
  T const& operator*()  const { PIMPL_PTR_CHECK_(); return *reinterpret_cast<T const*>(&data); }
  T      * operator->()       { PIMPL_PTR_CHECK_(); return &**this; }
  T const* operator->() const { PIMPL_PTR_CHECK_(); return &**this; }

  friend inline void swap(pimpl_ptr& lhs, pimpl_ptr& rhs)
  { using std::swap; PIMPL_PTR_CHECK_(); swap(*lhs, *rhs); }
#undef PIMPL_PTR_CHECK_
};

#endif /* PIMPL_PTR_HH_ */
