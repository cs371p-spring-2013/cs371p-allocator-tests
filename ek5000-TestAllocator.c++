// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
   To test the program:
   % ls /usr/include/cppunit/
   ...
   TestFixture.h
   ...
   % locate libcppunit.a
   /usr/lib/libcppunit.a
   % g++ -pedantic -std=c++0x -Wall Allocator.c++ TestAllocator.c++ -o TestAllocator -lcppunit -ldl
   % valgrind TestAllocator >& TestAllocator.out
 */

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define class struct
#define private public
#define protected public
#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
  // --------
  // typedefs
  // --------

  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;

  // --------
  // test_one
  // --------

  void test_one () {
    A x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    x.construct(p, v);
    CPPUNIT_ASSERT(*p == v);
    x.destroy(p);
    x.deallocate(p, s);}

  // --------
  // test_ten
  // --------

  void test_ten () {
    A x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    pointer         e = b + s;
    pointer         p = b;
    try {
      while (p != e) {
        x.construct(p, v);
        ++p;}}
    catch (...) {
      while (b != p) {
        --p;
        x.destroy(p);}
      x.deallocate(b, s);
      throw;}
    CPPUNIT_ASSERT(std::count(b, e, v) == s);
    while (b != e) {
      --e;
      x.destroy(e);}
    x.deallocate(b, s);}

  // -----
  // suite
  // -----

  CPPUNIT_TEST_SUITE(TestAllocator);
  CPPUNIT_TEST(test_one);
  CPPUNIT_TEST(test_ten);
  CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture
{
  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;
  
  // --------
  // Allocate
  // --------

  //Test allocating one pointer
  void test_allocate_1 ()
  {
    A x;
    const pointer         p1 = x.allocate(10);
    CPPUNIT_ASSERT(x.check_sentinel(0) == -40);
    CPPUNIT_ASSERT(x.check_sentinel(1) ==  44);
    x.deallocate(p1);
  }
  
  //Test trying to acquire unavailable memory
  void test_allocate_2 ()
  {
    A x;
    const pointer         p1 = x.allocate(23);
    const pointer         p2 = x.allocate(1);
    CPPUNIT_ASSERT(x.check_sentinel(0) == -92);
    CPPUNIT_ASSERT(p1 != 0);
    CPPUNIT_ASSERT(p2 == 0);
    x.deallocate(p1);
  }

  //Test multiple allocations, along with not leaving slivers
  void test_allocate_3 ()
  {
    A x;
    const pointer         p1 = x.allocate(5);
    const pointer         p2 = x.allocate(10);
    const pointer         p3 = x.allocate(3);
    CPPUNIT_ASSERT(x.check_sentinel(0) == -20);
    CPPUNIT_ASSERT(x.check_sentinel(1) == -40);
    CPPUNIT_ASSERT(x.check_sentinel(2) == -16);
    x.deallocate(p1);
    x.deallocate(p2);
    x.deallocate(p3);
  }

  //Tests regular deallocate
  void test_deallocate_1()
  {
    A x;
    const pointer         p1 = x.allocate(23);
    CPPUNIT_ASSERT(x.check_sentinel(0) == -92);
    x.deallocate(p1);
    CPPUNIT_ASSERT(x.check_sentinel(0) == 92);
  }
  
  //Tests deallocating a pointer against the side
  void test_deallocate_2()
  {
    A x;
    const pointer         p1 = x.allocate(10);
    const pointer         p2 = x.allocate(11);
    x.deallocate(p2);
    CPPUNIT_ASSERT(x.check_sentinel(0) == -40);
    CPPUNIT_ASSERT(x.check_sentinel(1) == 44);
    x.deallocate(p1);
    CPPUNIT_ASSERT(x.check_sentinel(0) == 92);
  }

  //Test deallocate a pointer in the middle
  void test_deallocate_3()
  {
    A x;
    const pointer         p1 = x.allocate(5);
    const pointer         p2 = x.allocate(10);
    const pointer         p3 = x.allocate(3);
    x.deallocate(p1);
    CPPUNIT_ASSERT(x.check_sentinel(0) ==  20);
    CPPUNIT_ASSERT(x.check_sentinel(1) == -40);
    CPPUNIT_ASSERT(x.check_sentinel(2) == -16);
    x.deallocate(p3);
    CPPUNIT_ASSERT(x.check_sentinel(0) ==  20);
    CPPUNIT_ASSERT(x.check_sentinel(1) == -40);
    CPPUNIT_ASSERT(x.check_sentinel(2) ==  16);
    x.deallocate(p2);
    CPPUNIT_ASSERT(x.check_sentinel(0) == 92);
  }

  //Tests valid for empty container
  void test_valid_1()
  {
    A x;
    for (int i  =0; i < 100; ++i)
    {
      x.view(x.a[i]) = 0;
    }
    x.view(x.a[0]) = 92;
    x.view(x.a[96]) = 92;
    CPPUNIT_ASSERT(x.valid());
  }

  //Tests valid for container with 2 elements
  void test_valid_2()
  {
    A x;
    for (int i  =0; i < 100; ++i)
    {
      x.view(x.a[i]) = 0;
    }
    x.view(x.a[0]) =  -40;
    x.view(x.a[44]) = -40;
    
    x.view(x.a[48]) =  44;
    x.view(x.a[96]) =  44;
    CPPUNIT_ASSERT(x.valid());
  }
  void test_valid_3()
  {
    A x;
    for (int i  =0; i < 100; ++i)
    {
      x.view(x.a[i]) = 0;
    }
    x.view(x.a[0])  = -20;
    x.view(x.a[24]) = -20;
    
    x.view(x.a[28]) = -40;
    x.view(x.a[72]) = -40;

    x.view(x.a[76]) = -16;
    x.view(x.a[96]) = -16;
    CPPUNIT_ASSERT(x.valid());
  }
  CPPUNIT_TEST_SUITE(TestMyAllocator);
  CPPUNIT_TEST(test_allocate_1);
  CPPUNIT_TEST(test_allocate_2);
  CPPUNIT_TEST(test_allocate_3);
  CPPUNIT_TEST(test_deallocate_1);
  CPPUNIT_TEST(test_deallocate_2);
  CPPUNIT_TEST(test_deallocate_3);
  CPPUNIT_TEST(test_valid_1);
  CPPUNIT_TEST(test_valid_2);
  CPPUNIT_TEST(test_valid_3);
  CPPUNIT_TEST_SUITE_END();
};

// ----
// main
// ----

int main () {
  using namespace std;
  ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
  cout << "TestAllocator.c++" << endl;

  CppUnit::TextTestRunner tr;

  tr.addTest(TestAllocator< std::allocator<int> >::suite());
  tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite()); // uncomment!
  tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!
  tr.addTest(TestAllocator< std::allocator<double> >::suite());
  //  tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

  tr.run();

  cout << "Done." << endl;
  return 0;}
