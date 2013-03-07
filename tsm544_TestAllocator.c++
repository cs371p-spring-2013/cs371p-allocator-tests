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
#include <vector>    // vector
#include <typeinfo>

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner
#include "Allocator.h"

//----------------------
// Constants I need
//----------------------
const int ALLOCATOR_SIZE = 100;

// -------------
// TestAllocator
// -------------

template<typename A>
struct TestAllocator: CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // --------
    // test_one
    // --------

    void test_one() {
      A x;
      const difference_type s = 1;
      const value_type v = 2;
      const pointer p = x.allocate(s);
      x.construct(p, v);
      CPPUNIT_ASSERT(*p == v);
      x.destroy(p);
      x.deallocate(p, s);
    }

    // --------
    // test_ten
    // --------

    void test_ten() {
      A x;
      const difference_type s = 10;
      const value_type v = 2;
      const pointer b = x.allocate(s);
      pointer e = b + s;
      pointer p = b;
      try {
        while (p != e) {
          x.construct(p, v);
          ++p;
        }
      }
      catch (...) {
        while (b != p) {
          --p;
          x.destroy(p);
        }
        x.deallocate(b, s);
        throw;
      }

      CPPUNIT_ASSERT(std::count(b, e, v) == s);

      while (b != e) {
        --e;
        x.destroy(e);
      }
      x.deallocate(b, s);
    }

    //--------- Constructor -----------

    void testConstructor01() {
      // Make sure problems aren't happening here
      try {
        A x;
      }
      catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

    //---------------------- Allocate ---------------
    // These tests rely on deallocate to avoid memory leaks,
    // but not assertions

    void testAllocate01() {
      A x;
      const difference_type s = 1;
      const pointer begin = x.allocate(s);

      CPPUNIT_ASSERT(begin != 0);

      x.deallocate(begin, s);  // Avoid memory leak
    }

    void testAllocate02() {
      A x;
      const difference_type s = 1;
      pointer b;

      for (int i = 0; i < 10; ++i) {
        b = x.allocate(s);
        CPPUNIT_ASSERT(b != 0);
        x.deallocate(b, s);
      }
    }

    void testAllocate03() {
      A x;
      // Should be no problem for std::allocator
      // and should fit snuggly within our own capped allocator
      // Assuming a size of 100, of course
      const difference_type s = 92 / x.SizeOfDataType;
      const pointer b = x.allocate(s);

      CPPUNIT_ASSERT(b != 0);

      x.deallocate(b, s);
    }

    // Check on allocate 0
    void testAllocate04() {
      A x;
      const difference_type s = 0;
      const pointer b = x.allocate(s);

      // Can't test b against 0 because malloc(0) and allocate(0) are
      // "implementation defined", so on some compilers it will be
      // null, and on others it will be something else
      // We just need to make sure there are no memory leaks
      if (b != 0) x.deallocate(b, s);
    }

    // Test bad_alloc some more
    // this time with a while loop
    //
    // !! CAUTION !!
    // Will slow down your computer
    void testAllocate06() {
      A x;
      const difference_type s = 10;
      std::vector<pointer> pointers;

      try {
        // Will fail eventually
        while (true) {
          pointer b = x.allocate(s);
          pointers.push_back(b);
        }
        CPPUNIT_ASSERT(false);
      }
      catch (std::bad_alloc& ba) {
        // Good behavior, now deallocate everything
        typedef typename std::vector<pointer>::iterator pIter;

        for (pIter i = pointers.begin(); i != pointers.end(); ++i) {
          x.deallocate(*i, s);
        }
      }
      catch (...) {
        // Bad
        CPPUNIT_ASSERT(false);
      }
    }

    //----------------------- Deallocate ---------------------
    // These won't segfault if allocate isn't working correctly

    void testDeallocate01() {
      A x;
      difference_type s = 5;
      const value_type v = value_type(6);
      const pointer p = x.allocate(s);
      pointer b = p;
      pointer e = p + s;

      while (b != e) {
        x.construct(b, v);
        ++b;
      }

      b = p;
      while (b != e) {
        x.destroy(b);
        ++b;
      }
    }

  CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);

    CPPUNIT_TEST(testConstructor01);

    CPPUNIT_TEST(testAllocate01);
    CPPUNIT_TEST(testAllocate02);
    CPPUNIT_TEST(testAllocate03);
    CPPUNIT_TEST(testAllocate04);

    CPPUNIT_TEST(testDeallocate01);

    // Careful, testAllocate06 is designed to consume the program's
    // entire memory space.  Could cause some unresponsiveness from the
    // system for a while
    CPPUNIT_TEST(testAllocate06);

    CPPUNIT_TEST_SUITE_END()
    ;
};

template<typename A>
struct TestAllocator2: CppUnit::TestFixture {
    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    //--------- Constructor -----------
    void testConstructor02() {
      A x;
      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[ALLOCATOR_SIZE - x.SizeOfInt]);

      CPPUNIT_ASSERT(begin == ALLOCATOR_SIZE - 2 * x.SizeOfInt);
      CPPUNIT_ASSERT(begin == end);
    }

    void testConstructor03() {
      A x;
      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[ALLOCATOR_SIZE - x.SizeOfInt]);

      CPPUNIT_ASSERT(begin == ALLOCATOR_SIZE - 2 * x.SizeOfInt);
      CPPUNIT_ASSERT(begin == end);

      // Make sure data was initialized to 0
      for (int i = x.SizeOfInt; i < x.EndSentinel; ++i) {
        CPPUNIT_ASSERT(x.a[i] == 0);
      }
    }

    //---------------------- Valid ------------------

    void testValid01() {
      A x;
      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[begin + x.SizeOfInt]);

      CPPUNIT_ASSERT(begin == end);
      CPPUNIT_ASSERT(x.valid());
    }

    void testValid02() {
      A x;

      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[begin + x.SizeOfInt]);

      CPPUNIT_ASSERT(begin == end);
      CPPUNIT_ASSERT(x.valid());

      begin = -1 * x.SizeOfInt;

      CPPUNIT_ASSERT(begin != end);
      CPPUNIT_ASSERT(!x.valid());
    }

    void testValid03() {
      A x;
      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[begin + x.SizeOfInt]);

      CPPUNIT_ASSERT(begin == end);
      CPPUNIT_ASSERT(x.valid());

      begin *= -1;
      end *= -1;

      CPPUNIT_ASSERT(begin == end);
      CPPUNIT_ASSERT(x.valid());
    }

    //---------------------- Allocate ---------------
    // These tests rely on deallocate to avoid memory leaks,
    // but not assertions

    void testAllocate07() {
      A x;

      const pointer p = x.allocate(5);

      int begin = x.viewAsInt(x.a[0]);
      int end = x.viewAsInt(x.a[sizeof(int) + 5 * x.SizeOfDataType]);

      CPPUNIT_ASSERT(begin == x.SizeOfDataType * -5);
      CPPUNIT_ASSERT(begin == end);

      x.deallocate(p);
    }

    //----------------------- Deallocate ---------------------

    void testDeallocate02() {
      A x;

      const pointer p = x.allocate(1);

      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[sizeof(int) + x.SizeOfDataType]);
      int& end2 = x.viewAsInt(x.a[x.EndSentinel]);

      CPPUNIT_ASSERT(begin == -1 * x.SizeOfDataType);
      CPPUNIT_ASSERT(begin == end);

      x.deallocate(p);

      CPPUNIT_ASSERT(begin == ALLOCATOR_SIZE - 2 * x.SizeOfInt);
      CPPUNIT_ASSERT(begin == end2);
    }

    void testDeallocate03() {
      A x;

      const pointer p1 = x.allocate(5);
      x.deallocate(p1);
      const pointer p2 = x.allocate(1);

      // Pointer should still be at the beginning of the array

      int& begin = x.viewAsInt(x.a[0]);
      int& end = x.viewAsInt(x.a[sizeof(int) + x.SizeOfDataType]);

      CPPUNIT_ASSERT(begin == -1 * x.SizeOfDataType);
      CPPUNIT_ASSERT(begin == end);

      x.deallocate(p2);
    }

    void testDeallocate04() {
      A x;

      const pointer p1 = x.allocate(1);
      const pointer p2 = x.allocate(1);
      const pointer p3 = x.allocate(1);

      x.deallocate(p1);
      x.deallocate(p3);

      int p1b = x.viewAsInt(x.a[0]);
      int p1e = x.viewAsInt(x.a[sizeof(int) + x.SizeOfDataType]);
//        int p3b = x.viewAsInt(x.a[sizeof(int) * 4 + x.SizeOfDataType * 2]);
//        int p3e = x.viewAsInt(x.a[sizeof(int) * 5 + x.SizeOfDataType * 3]);

      CPPUNIT_ASSERT(p1b == x.SizeOfDataType);
      CPPUNIT_ASSERT(p1e == p1b);

      // On this next delete, the free block in the middle should
      // merge with the blocks all around him
      x.deallocate(p2);

      int begin = x.viewAsInt(x.a[0]);
      int end = x.viewAsInt(x.a[ALLOCATOR_SIZE - sizeof(int)]);

      CPPUNIT_ASSERT(begin == ALLOCATOR_SIZE - 2 * sizeof(int));
      CPPUNIT_ASSERT(begin == end);
    }

    // -----
    // suite
    // -----

  CPPUNIT_TEST_SUITE(TestAllocator2);
    CPPUNIT_TEST(testConstructor02);
    CPPUNIT_TEST(testConstructor03);
    CPPUNIT_TEST(testValid01);
    CPPUNIT_TEST(testValid02);
    CPPUNIT_TEST(testValid03);
    CPPUNIT_TEST(testAllocate07);
    CPPUNIT_TEST(testDeallocate02);
    CPPUNIT_TEST(testDeallocate03);
    CPPUNIT_TEST(testDeallocate04);
    CPPUNIT_TEST_SUITE_END();
};

// ----
// main
// ----

int main() {
  using namespace std;
  ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
  cout << "TestAllocator.c++" << endl;

  CppUnit::TextTestRunner tr;

//  tr.addTest(TestAllocator<std::allocator<int> >::suite());
//  tr.addTest(TestAllocator<std::allocator<double> >::suite());

  tr.addTest(TestAllocator<Allocator<int, ALLOCATOR_SIZE> >::suite());
  tr.addTest(TestAllocator<Allocator<double, ALLOCATOR_SIZE> >::suite());

  tr.addTest(TestAllocator2<Allocator<int, ALLOCATOR_SIZE> >::suite());
  tr.addTest(TestAllocator2<Allocator<double, ALLOCATOR_SIZE> >::suite());

  tr.run();

  cout << "Done." << endl;
  return 0;
}
