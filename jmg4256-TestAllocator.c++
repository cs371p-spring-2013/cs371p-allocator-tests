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

    // --------
    // test_valid1
    // --------

    void test_valid1()
    {
      Allocator<int, 1000> x;
      CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_valid2
    // --------

    void test_valid2()
    {
      Allocator<char, 100> x;
      char* p = x.allocate(20);
      *(int*)(p-4) = 0;
      CPPUNIT_ASSERT(!x.valid());
    }
    
    // --------
    // test_valid3
    // --------

    void test_valid3()
    {
      Allocator<int, 100> x;
      int* p = x.allocate(15);
      *((int*)(p+15)) = 92;
      CPPUNIT_ASSERT(!x.valid());
    }

    // -------
    // test_allocate1
    // -------

    void test_allocate1()
    {
      Allocator<double,100> x;
      x.allocate(3);
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == -24 && x.get_sentinel_val(28) == -24);
      x.allocate(2);
      CPPUNIT_ASSERT(x.get_sentinel_val(32) == -16 && x.get_sentinel_val(52) == -16);
      CPPUNIT_ASSERT(x.valid());
    }

    // -------
    // test_allocate2
    // -------

    void test_allocate2()
    {
      Allocator<int,100> x;
      try
      {
        //allocate more than space, should throw bad_alloc()
        x.allocate(10);
        x.allocate(8);
        x.allocate(1);
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
      Allocator<double,1> x;
      try
      {
        //can't allocate due to size of Allocator
        x.allocate(1);
      }
      catch (std::bad_alloc e)
      {

      }
    }

    // ------
    // test_deallocate
    // ------

    void test_deallocate1()
    {
      Allocator<double,100> x;
      double* p = x.allocate(10);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());

    }

    // -----
    // test_deallocate2
    // -----

    void test_deallocate2()
    {
      Allocator<double, 500> x;
      x.allocate(5);
      double* p = x.allocate(5);
      x.allocate(5);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());
    }

    // ------
    // test_deallocate3
    // -----
    
    void test_deallocate3()
    {
      Allocator<char, 20> x;
      char* p = x.allocate(3);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());
    }
    
    // ------
    // test_get_sentinel_1
    // -----
    
    void test_get_sentinel_1()
    {
      Allocator<int, 20> x;
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == 12 && x.get_sentinel_val(16) == 12);
    }
    
    // ------
    // test_get_sentinel_2
    // -----
    
    void test_get_sentinel_2()
    {
      Allocator<int, 8> x;
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == 0 && x.get_sentinel_val(4) == 0);
    }
    
    // ------
    // test_get_sentinel_3
    // -----
    
    void test_get_sentinel_3()
    {
      Allocator<double, 100> x;
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == 92 && x.get_sentinel_val(96) == 92);
    }

    // ------
    // test_change_sentinel_1
    // -----
    
    void test_change_sentinel_1()
    {
      Allocator<int, 20> x;
      x.change_sentinel(92, 0);
      x.change_sentinel(92, 16);
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == 92 && x.get_sentinel_val(16) == 92);
    }
  
    // ------
    // test_change_sentinel_2
    // -----
    
    void test_change_sentinel_2()
    {
      Allocator<int, 20> x;
      x.change_sentinel(-92, 5);
      x.change_sentinel(-92, 10);
      CPPUNIT_ASSERT(x.get_sentinel_val(5) == -92 && x.get_sentinel_val(10) == -92);
    }
  
    // ------
    // test_change_sentinel_3
    // -----
    
    void test_change_sentinel_3()
    {
      Allocator<int, 8> x;
      x.change_sentinel(5, 0);
      x.change_sentinel(10, 4);
      CPPUNIT_ASSERT(x.get_sentinel_val(0) == 5 && x.get_sentinel_val(4) == 10);
    }
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST(test_get_sentinel_1);
    CPPUNIT_TEST(test_get_sentinel_2);
    CPPUNIT_TEST(test_get_sentinel_3);
    CPPUNIT_TEST(test_change_sentinel_1);
    CPPUNIT_TEST(test_change_sentinel_2);
    CPPUNIT_TEST(test_change_sentinel_3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    //tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    //tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 1000> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
