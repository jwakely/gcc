// { dg-options "-std=gnu++20" }
// { dg-do run { target c++20 } }
// { dg-require-effective-target gthreads }
// { dg-require-gthreads "" }

#include <memory>

#ifndef __cpp_lib_atomic_shared_ptr
# error "Feature-test macro for atomic<shared_ptr<T>> missing in <memory>"
#elif __cpp_lib_atomic_shared_ptr != 201711L
# error "Feature-test macro for atomic<shared_ptr<T>> has wrong value in <memory>"
#endif

#include <thread>

#include <testsuite_hooks.h>

template<typename Tp>
  void
  test_is_lock_free()
  {
    using test_type = std::atomic<Tp>;
    VERIFY( test_type::is_always_lock_free == false );

    test_type p;
    VERIFY( p.is_lock_free() == false );
  }

struct A { int a; int b; };

template<typename Tp, typename Ta>
  void
  test_wait_notify(Tp& p, const Ta& a, const Ta& b)
  {
    p.store(a);
    p.wait(b);
    std::thread t([&]
      {
        p.store(b);
        p.notify_one();
      });
    p.wait(a);
    t.join();
  }

void
test_atomic_shared_ptr()
{
  test_is_lock_free<std::shared_ptr<int>>();
  auto a = std::make_shared<A>( 0, 42 );
  using ptr_t = std::shared_ptr<A>;
  {
    std::atomic<ptr_t> p{ };
    VERIFY( p.load().get() == nullptr );
  }

  std::atomic<ptr_t> p{ a };
  VERIFY( p.load().get() == a.get() );
  auto b = std::make_shared<A>( 42, 0 );
  p.store(b);
  VERIFY( p.load().get() != a.get() );
  VERIFY( p.load().get() == b.get() );
  p.exchange(a);
  VERIFY( p.load().get() != b.get() );
  VERIFY( p.load().get() == a.get() );

  {
    ptr_t aa{ a };
    VERIFY( p.compare_exchange_strong(aa, b,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == true );
    ptr_t bb{ a };
    VERIFY( p.compare_exchange_strong(bb, b,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == false );
    VERIFY( bb.get() == b.get() );
  }

  {
    ptr_t bb{ b };
    VERIFY( p.compare_exchange_weak(bb, a,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == true );
    ptr_t aa{ b };
    VERIFY( p.compare_exchange_weak(aa, a,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == false );
    VERIFY( aa.get() == a.get() );
  }
  test_wait_notify(p, a, b);
}

void
test_atomic_weak_ptr()
{
  test_is_lock_free<std::weak_ptr<int>>();
  auto a = std::make_shared<A>( 0, 42 );
  using ptr_t = std::weak_ptr<A>;
  ptr_t wa{ a };
  {
    std::atomic<ptr_t> p{ };
    VERIFY( p.load().lock().get() == nullptr );
  }

  std::atomic<ptr_t> p{ wa };
  VERIFY( p.load().lock().get() == a.get() );

  auto b = std::make_shared<A>( 42, 0 );
  ptr_t wb{ b };
  p.store(wb);
  VERIFY( p.load().lock().get() != a.get() );
  VERIFY( p.load().lock().get() == b.get() );
  p.exchange(wa);
  VERIFY( p.load().lock().get() != b.get() );
  VERIFY( p.load().lock().get() == a.get() );

  {
    ptr_t aa{ a };
    VERIFY( p.compare_exchange_strong(aa, b,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == true );
    ptr_t bb{ a };
    VERIFY( p.compare_exchange_strong(bb, b,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == false );
    VERIFY( bb.lock().get() == b.get() );
  }

  {
    ptr_t bb{ b };
    VERIFY( p.compare_exchange_weak(bb, a,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == true );
    ptr_t aa{ b };
    VERIFY( p.compare_exchange_weak(aa, a,
                std::memory_order_seq_cst,
                std::memory_order_seq_cst) == false );
    VERIFY( aa.lock().get() == a.get() );
  }
  test_wait_notify(p, wa, wb);
}

int
main()
{
  test_atomic_shared_ptr();
  test_atomic_weak_ptr();
  return 0;
}
