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


#define protected public
#define private public
#define class struct

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

    // ----------
    // test_multiplecases
    // ---------
    
    void test_multiplecases()
    {
      A x;
      pointer p = x.allocate(5);
      x.deallocate(p,0);
      x.allocate(1);
      p = x.allocate(1);
      int testValue = 19;
      *((int *)p) = 19;
      pointer p2 = x.allocate(1);
      x.deallocate(p2,0);
      CPPUNIT_ASSERT(*((int *)p) == testValue);
      x.deallocate(p,0);
    }

    // --------
    // test_valid1
    // --------

    void test_valid1()
    {
      Allocator<int, 100> x;
      CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_valid2
    // --------

    void test_valid2()
    {
      Allocator<double, 100> x;
      double* p = x.allocate(5);
      *(int*)(((char *)p)-4) = 0;
      CPPUNIT_ASSERT(!x.valid());
    }
    
    // --------
    // test_valid3
    // --------

    void test_valid3()
    {
      Allocator<int, 100> x;
      int* p = x.allocate(23);
      *(p+23) = 92;
      CPPUNIT_ASSERT(!x.valid());
    }

    // -------
    // test_allocate1
    // -------

    void test_allocate1()
    {
      Allocator<double,100> x;
      x.allocate(5);
      x.allocate(2);
      CPPUNIT_ASSERT(x.valid());
    }

    // -------
    // test_allocate2
    // -------

    void test_allocate2()
    {
      Allocator<int,100> x;
      x.allocate(10);
      x.allocate(8);
      x.allocate(1);
      try
      {
        x.allocate(1);
      }
      catch (std::bad_alloc e)
      {

      }
      CPPUNIT_ASSERT(x.valid());
    }

    // ------
    // test_allocate3
    // -----

    void test_allocate3()
    {
      Allocator<double,4> x;
      try
      {
        x.allocate(1);
      }
      catch (std::bad_alloc e)
      {

      }

      Allocator<int, 12> y;
      CPPUNIT_ASSERT(y.allocate(0) == 0);
    }

    // ------
    // test_deallocate
    // ------

    void test_deallocate1()
    {
      Allocator<double,100> x;
      double* p = x.allocate(5);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());

    }

    // -----
    // test_deallocate2
    // -----

    void test_deallocate2()
    {
      Allocator<int, 100> x;
      x.allocate(5);
      int* p = x.allocate(5);
      x.allocate(5);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());
    }

    // ------
    // test_deallocate3
    // -----
    
    void test_deallocate3()
    {
      Allocator<int, 20> x;
      int* p = x.allocate(3);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());
    }
    

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_multiplecases);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
