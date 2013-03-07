// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

// ________________________________________
// WARNING: implementation-specific tests
//          are in the TestAllocaterInDepth
//          struct.
// ________________________________________

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

#define private public   // dangerous, but allows for testing of private parts
#define protected public // of allocator
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
        x.deallocate(b, s);
    }

    // -----------
    // test_double
    // -----------

    void test_double(){
      A x;
      const difference_type s = 2;
      const pointer       b = x.allocate(s);
      const pointer       c = x.allocate(s);
      CPPUNIT_ASSERT(b!=c);
      x.deallocate(b, s);
      x.deallocate(c, s);
    }

    // -----------------------
    // test_reallocate_perfect
    // -----------------------

    void test_reallocate_perfect(){
      A x;
      const difference_type s = 2;
      const value_type      v = 2;
      const pointer         b = x.allocate(s);
            pointer         e = b + s;
            pointer         p = b;
      const pointer         b2 = x.allocate(s);
            pointer         e2 = b2+s;
            pointer         p2 = b2;
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
      try{
        while (p2 != e2) {
          x.construct(p2, v);
          ++p2;}}
      catch (...) {
        while (b2 != p2) {
          --p2;
          x.destroy(p2);}
          x.deallocate(b2, s);
          throw;}
      CPPUNIT_ASSERT(std::count(b2, e2, v) == s);
      CPPUNIT_ASSERT(b != b2);  
      while (b != e) {
          --e;
          x.destroy(e);}
      x.deallocate(b, s);
     
      const pointer b3 = x.allocate(s);
      CPPUNIT_ASSERT(b3 == b);

      while (b2 != e2) {
          --e2;
          x.destroy(e2);}
      x.deallocate(b2, s);
      x.deallocate(b3, s);
      
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_double);
    CPPUNIT_TEST(test_reallocate_perfect);
    CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct TestAllocatorInDepth : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;


    // ----------------
    // test_constructor
    // ----------------

    void test_constructor(){
      A x;
      CPPUNIT_ASSERT(view(x.a[0]) == 92);
      CPPUNIT_ASSERT(view(x.a[96]) == 92);
    }

    // -----------
    // test_valid1
    // -----------

    void test_valid1(){
      A x;
      CPPUNIT_ASSERT(x.valid());
    }

    // -----------
    // test_valid2
    // -----------

    void test_valid2(){
      A x;
      x.a[0] = 0;
      x.a[sizeof(int)] = 0;
      x.a[2*sizeof(int)] = x.n-4*sizeof(int);
      x.a[x.n-sizeof(int)] = x.n-4*sizeof(int);
      CPPUNIT_ASSERT(!x.valid());
    }

    // -----------
    // test_valid3
    // -----------

    void test_valid3(){
      A x;
      // simulate a block being too small.
      int invalidSize = sizeof(value_type)-1;
      int leftover = x.n - invalidSize;
      x.a[0] = invalidSize;
      x.a[sizeof(int) + invalidSize] = invalidSize;
      // simulate rest being full.
      x.a[2*sizeof(int) + invalidSize] = -leftover;
      x.a[x.n - sizeof(int)] = -leftover;
      CPPUNIT_ASSERT(!x.valid()); // first block is too small to store in
    }

    // -----------
    // test_valid4
    // -----------

    void test_valid4(){
      A x;
      view(x.a[0]) = 44;
      view(x.a[sizeof(int) + 44]) = 44;
      view(x.a[2*sizeof(int) + 44]) = -24;
      view(x.a[44 + 24 + 3*sizeof(int)]) = -24;
      view(x.a[44 + 24 + 4*sizeof(int)]) = 8;
      view(x.a[x.n - sizeof(int)]) = 8;
      CPPUNIT_ASSERT(x.valid());
    }

    // -----------
    // test_valid5
    // -----------

    void test_valid5(){
      A x;
      view(x.a[0]) = 44;
      view(x.a[sizeof(int) + 44]) = 44;
      view(x.a[2*sizeof(int) + 44]) = 24;
      view(x.a[44 + 24 + 3*sizeof(int)]) = 24;
      view(x.a[44 + 24 + 4*sizeof(int)]) = 8;
      view(x.a[x.n - sizeof(int)]) = 8;
      CPPUNIT_ASSERT(!x.valid());
    }

    // -------------
    // test_coalesce
    // -------------

    void test_coalesce(){
      A x;
      int index = 0;
      int dealloc = 0;
      view(x.a[0]) = 32;
      index+=32 + sizeof(int);
      view(x.a[index]) = 32;
      index += sizeof(int);
      view(x.a[index]) = -12;
      dealloc = index + sizeof(int);
      index+= 12 + sizeof(int);
      view(x.a[index]) = -12;
      index+= sizeof(int);
      view(x.a[index]) = 32;
      index+= 32 + sizeof(int);
      view(x.a[index]) = 32;

      pointer p = reinterpret_cast<pointer>(&x.a[dealloc]);
      x.deallocate(p, 12);
      CPPUNIT_ASSERT(x.a[0] == 92);
      CPPUNIT_ASSERT(x.a[96] = 92);
    }

    // --------------
    // test_bad_alloc
    // --------------

    void test_bad_alloc(){
      A x;
      const difference_type s = 24; // should force more than 92 bytes
      pointer p;
      try{
        p = x.allocate(s);
      }
      catch(std::bad_alloc& e){
        return;
      }
      x.deallocate(p, s);
      CPPUNIT_ASSERT(false);
    }

    // ---------------
    // test_bad_alloc2
    // ---------------

    void test_bad_alloc2(){
      A x;
      const difference_type s = 2;
      const pointer p = x.allocate(s);
      pointer d;
      const difference_type tooLarge = 24 - s;
      try{
        d = x.allocate(tooLarge);
      }
      catch(std::bad_alloc& e){
        x.deallocate(p, s);
        return;
      }
      x.deallocate(p, s);
      x.deallocate(d, tooLarge);
      CPPUNIT_ASSERT(false);
    }

    // --------------------
    // test_double_coalesce
    // --------------------

    void test_double_coalesce(){
      A x;
      const difference_type s = 2;
      const pointer       b = x.allocate(s);
      const pointer       c = x.allocate(s);
      CPPUNIT_ASSERT(b!=c);
      x.deallocate(b, s);
      x.deallocate(c, s);
      CPPUNIT_ASSERT(x.a[0] ==  92);
      CPPUNIT_ASSERT(x.a[96] == 92);
    }

    // ---------
    // test_view
    // ---------

    void test_view(){
      char c[] = {0, 0, 0, 0};
      CPPUNIT_ASSERT(view(c[0]) == 0);
    }

    // ----------
    // test_view2
    // ----------

    void test_view2(){
      char c[5];
      c[0] = 0;
      view(c[1]) = 2;
      CPPUNIT_ASSERT(c[0] == 0);
      CPPUNIT_ASSERT(view(c[1]) == 2);
    }

    // ----------
    // test_view3
    // ----------

    void test_view3(){
      char c[8];
      view(c[0]) = -1;
      view(c[4]) = 4;
      CPPUNIT_ASSERT(c[0] = -1);
      CPPUNIT_ASSERT(c[4] = 4);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocatorInDepth);
    CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_valid4);
    CPPUNIT_TEST(test_valid5);
    CPPUNIT_TEST(test_coalesce);
    CPPUNIT_TEST(test_bad_alloc);
    CPPUNIT_TEST(test_bad_alloc2);
    CPPUNIT_TEST(test_double_coalesce);
    CPPUNIT_TEST(test_view);
    CPPUNIT_TEST(test_view2);
    CPPUNIT_TEST(test_view3);
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
    
    tr.addTest(TestAllocatorInDepth< Allocator<int, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
