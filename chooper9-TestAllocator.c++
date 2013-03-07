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

// ----------------------------
// defines to view private info
// ----------------------------
#define private public
#define protected public
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

    int& view (char& c) {
      return *reinterpret_cast<int*>(&c);}

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        //std::cout << "Test 1" << std::endl;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        //std::cout << "Test 2" << std::endl;
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        //std::cout << "Test 3" << std::endl;
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
        //std::cout << "Test 1" << std::endl;
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
        //std::cout << "Test: Finished Constructing" << std::endl;
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


// --------------
// TestAllocator2
// --------------
// tests specific methods within custom allocator
template <typename A>
struct TestAllocator2 : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;


    int& view (char& c) {
      return *reinterpret_cast<int*>(&c);}

    // -----------
    // Constructor
    // -----------

    void test_Allocator_100 () {
      Allocator<int, 100> x;
      int i = view(x.a[0]);
      //std::cout << "i = " << i << std::endl;
      int j = view(x.a[i+sizeof(int)]);
      //std::cout << "j = " << j << std::endl;
      CPPUNIT_ASSERT(i == 100-(2*sizeof(int)));
      CPPUNIT_ASSERT(j == i);
    }

    void test_Allocator_600 () {
      Allocator<int, 600> x;
      int i = view(x.a[0]);
      //std::cout << "i = " << i << std::endl;
      int j = view(x.a[i+sizeof(int)]);
      //std::cout << "j = " << j << std::endl;
      CPPUNIT_ASSERT(i == 600-(2*sizeof(int)));
      CPPUNIT_ASSERT(j == i);
    }

    void test_Allocator_10 () {
      try {
        Allocator<int, 10> x;
        CPPUNIT_ASSERT(false);
      }
      catch (...) {
      }
    }

    // -----
    // valid
    // -----

    // shows that valid will fail when the sentinels do not match
    void test_valid () {
        A x;
        const difference_type  s = 1;
        const value_type       v = 2;
        const pointer          p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        int* ip = &view(x.a[0]);
        ++*ip;
        CPPUNIT_ASSERT(!x.valid());
        --*ip;
        x.destroy(p);
        x.deallocate(p, s);}

    // --------
    // allocate
    // --------
    void test_allocate_1 () {
        A x;
        const difference_type  s = 1;
        const value_type       v = 2;
        const pointer          p = x.allocate(s);
        x.construct(p, v);
        int i = -1*static_cast<int>(sizeof(value_type));
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(view(x.a[0]) == i);
        CPPUNIT_ASSERT(view(x.a[-1*i+sizeof(int)]) == i);
        x.destroy(p);
        x.deallocate(p, s);}

    void test_allocate_2 () {
        A x;
        const difference_type  s = 8;
        const value_type       v = 2;
        const pointer          p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(view(x.a[0]) == -8*static_cast<int>(sizeof(value_type)));
        x.destroy(p);
        x.deallocate(p, s);}

    // when testing <int, 100>, should give extra block space
    // when testing <double, 100>, should throw a bad_alloc exception
    void test_allocate_3 () {
        A x;
        const difference_type s = 22;
        const value_type      v = 2;
        try {
          const pointer         p = x.allocate(s);
          int                 sen = view(x.a[0]);
          x.construct(p, v);
          CPPUNIT_ASSERT(*p == v);
          CPPUNIT_ASSERT(sen == -92);
          x.destroy(p);
          x.deallocate(p, s);}
        catch (std::bad_alloc& e) {
        }}

    void test_allocate_4 () {
      A x;
      const difference_type s = 1;
      try {
        const pointer       p1 = x.allocate(s);
        const pointer       p2 = x.allocate(s);
        const pointer       p3 = x.allocate(s);
        int i = -1*static_cast<int>(sizeof(value_type));
        CPPUNIT_ASSERT(view(x.a[0]) == i);
        CPPUNIT_ASSERT(view(x.a[(-1*i)+(2*sizeof(int))]) == i);
        CPPUNIT_ASSERT(view(x.a[(-2*i)+(4*sizeof(int))]) == i);
        x.deallocate(p1, s);
        x.deallocate(p2, s);
        x.deallocate(p3, s);
      }
      catch (std::bad_alloc& e) {
        CPPUNIT_ASSERT(false);}
    }



    // ----------
    // deallocate
    // ----------
    // shows that deallocate will merge with free block to the right
    void test_deallocate_1 () {
        A x;
        int init = view(x.a[0]);
        const difference_type  s = 1;
        const pointer          p = x.allocate(s);
        CPPUNIT_ASSERT(x.a[0] != init);
        x.deallocate(p, s);
        CPPUNIT_ASSERT(x.a[0] == init);}

    // shows that deallocate will merge with free block to the left
    void test_deallocate_2 () {
        A x;
        const difference_type s = 1;
        try {
          const pointer         p1 = x.allocate(s);
          const pointer         p2 = x.allocate(s);
          const pointer         p3 = x.allocate(s);
          int                   v = static_cast<int>(sizeof(value_type));
          int                   i = static_cast<int>(sizeof(int));
          CPPUNIT_ASSERT(-1*view(x.a[0]) == v);
          CPPUNIT_ASSERT(-1*view(x.a[(2*sizeof(int))+v]) == v);
          x.deallocate(p1, s);
          CPPUNIT_ASSERT(view(x.a[0]) == v);
          CPPUNIT_ASSERT(-1*view(x.a[(2*sizeof(int))+v]) == v);
          x.deallocate(p2, s);
          CPPUNIT_ASSERT(view(x.a[0]) == 2*v+2*i);
          x.deallocate(p3, s);}
        catch (std::bad_alloc& e) {
          CPPUNIT_ASSERT(false);}}

    // shows that deallocate will merge with multiple free blocks
    void test_deallocate_3 () {
        A x;
        const difference_type s = 1;
        int init                = view(x.a[0]);
        try {
          const pointer         p1 = x.allocate(s);
          const pointer         p2 = x.allocate(s);
          int                   v = static_cast<int>(sizeof(value_type));
          int                   i = static_cast<int>(sizeof(int));
          CPPUNIT_ASSERT(-1*view(x.a[0]) == v);
          CPPUNIT_ASSERT(-1*view(x.a[2*i+v]) == v);
          x.deallocate(p1, s);
          CPPUNIT_ASSERT(view(x.a[0]) == v);
          CPPUNIT_ASSERT(-1*view(x.a[2*i+v]) == v);
          x.deallocate(p2, s);
          CPPUNIT_ASSERT(view(x.a[0]) == init);
        }
        catch (std::bad_alloc& e) {
        CPPUNIT_ASSERT(false);}}


  CPPUNIT_TEST_SUITE(TestAllocator2);
  CPPUNIT_TEST(test_Allocator_100);
  CPPUNIT_TEST(test_Allocator_600);
  CPPUNIT_TEST(test_Allocator_10);
  CPPUNIT_TEST(test_valid);
  CPPUNIT_TEST(test_allocate_1);
  CPPUNIT_TEST(test_allocate_2);
  CPPUNIT_TEST(test_allocate_3);
  CPPUNIT_TEST(test_allocate_4);
  CPPUNIT_TEST(test_deallocate_1);
  CPPUNIT_TEST(test_deallocate_2);
  CPPUNIT_TEST(test_deallocate_3);
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

    tr.addTest(TestAllocator2< Allocator<int, 100> >::suite());
    tr.addTest(TestAllocator2< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}

