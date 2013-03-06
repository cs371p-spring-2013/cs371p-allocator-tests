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
% g++ -pedantic -std=c++0x -Wall Allocator.h TestAllocator.c++ -o TestAllocator -lcppunit -ldl
% valgrind TestAllocator >& TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream> // ios_base
#include <memory> // allocator
#include <new> // bad_alloc
#include <cstring>
#include <vector>

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h" // TestFixture
#include "cppunit/TextTestRunner.h" // TestRunner

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

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;


    // -------------
    // test_allocate
    // -------------

    void test_allocate_easy () {
      A x;
      try {
        pointer p = x.allocate(3);
        x.deallocate(p, 3);
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

    void test_allocate_general_case () {
      A x;
      pointer a[6];
      try {
        for (int i = 0; i < 6; i++) {
          a[i] = x.allocate(1);
        }
        for (int i = 0; i < 6; i++) {
          x.deallocate(a[i], 1);
        }
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

    void test_allocate_special_case () {
      // case where left over space is too small to use later if allocating doubles
      A x;
      pointer a;
      try {
        a = x.allocate(11);
        x.deallocate(a, 11);
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }
    
    // ---------------
    // test_deallocate
    // ---------------

    void test_deallocate_easy () {
      A x;
      pointer p = x.allocate(1);
      x.deallocate(p, 1);
    }

    void test_deallocate_harder () {
      A x;
      pointer p1 = x.allocate(1);
      pointer p2 = x.allocate(1);
      pointer p3 = x.allocate(1);
      x.deallocate(p1, 1);
      x.deallocate(p2, 1);
      x.deallocate(p3, 1);
    }

    void test_deallocate_general_case () {
      A x;
      try {
        pointer p1 = x.allocate(1);
        pointer p2 = x.allocate(1);
        pointer p3 = x.allocate(1);
        pointer p4 = x.allocate(1);
        pointer p5 = x.allocate(1);
        pointer p6 = x.allocate(1);

        x.deallocate(p1, 1);
        x.deallocate(p2, 1);
        x.deallocate(p3, 1);
        x.deallocate(p4, 1);
        x.deallocate(p5, 1);
        x.deallocate(p6, 1);
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

    // --------
    // test_one
    // --------

    void test_one () {
      A x;
      const difference_type s = 1;
      const value_type v = 2;
      const pointer p = x.allocate(s);
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
      const value_type v = 2;
      const pointer b = x.allocate(s);
            pointer e = b + s;
            pointer p = b;
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
    CPPUNIT_TEST(test_allocate_easy);
    CPPUNIT_TEST(test_allocate_general_case);
    CPPUNIT_TEST(test_allocate_special_case);
    CPPUNIT_TEST(test_deallocate_easy);
    CPPUNIT_TEST(test_deallocate_harder);
    CPPUNIT_TEST(test_deallocate_general_case);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten); 
    CPPUNIT_TEST_SUITE_END();
};


template <typename A>
struct TestOurAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // ----------------
    // test_constructor
    // ----------------

    void test_constructor () {
      // tests different constructors when different template parameters are used
      A x;
      int* arr = (int*)x.a;
      CPPUNIT_ASSERT(*arr == x.size - 2*sizeof(int));
      char* temp = x.a + x.size - sizeof(int);
      arr = (int*)temp;
      CPPUNIT_ASSERT(*arr == x.size - 2*sizeof(int));
    }

    // ----------
    // test_valid
    // ----------

    void test_valid_1 () {
      A x;
      x.allocate(1);
      int* arr = (int*)x.a;
      CPPUNIT_ASSERT(*arr == -x.sizeT);
      char* temp = x.a + x.sizeT + sizeof(int);
      arr = (int*)temp;
      CPPUNIT_ASSERT(*arr == -x.sizeT);
      CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_2 () {
      A x;
      x.allocate(3);
      int* arr = (int*)x.a;
      CPPUNIT_ASSERT(*arr == -3*x.sizeT);
      char* temp = x.a + 3*x.sizeT + sizeof(int);
      arr = (int*)temp;
      CPPUNIT_ASSERT(*arr == -3*x.sizeT);
      CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_3 () {
      A x;
      std::vector<pointer> a;
      try {
        for (unsigned int i = 0; i < A::size/(A::sizeT + 2*sizeof(int)); i++) {
          a.push_back(x.allocate(1));
          CPPUNIT_ASSERT(x.valid());
        }

        CPPUNIT_ASSERT(x.valid());
        
        for (unsigned int i = 0; i < A::size/(A::sizeT + 2*sizeof(int)); i++) {
          x.deallocate(a.back());
          a.pop_back();
          CPPUNIT_ASSERT(x.valid());
        }
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_4 () {
      A x;
      pointer p1 = x.allocate(2);
      CPPUNIT_ASSERT(x.valid());
      pointer p2 = x.allocate(3);
      CPPUNIT_ASSERT(x.valid());

      x.deallocate(p1, 2);
      x.deallocate(p2, 3);
      CPPUNIT_ASSERT(x.valid());
    }

    // -------------
    // test_allocate
    // -------------

    void test_allocate_1 () {
      A x;
      std::vector<pointer> a;
      try {
        for (unsigned int i = 0; i < A::size/(A::sizeT + 2*sizeof(int)); i++) {
          a.push_back(x.allocate(1));
        }
        for (unsigned int i = 0; i < A::size/(A::sizeT + 2*sizeof(int)); i++) {
          x.deallocate(a.back());
          a.pop_back();
        }
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT(x.valid());
    }

    void test_allocate_2 () {
      A x;
      try {
        pointer a = x.allocate(1);
        pointer b = x.allocate(2);
        pointer c = x.allocate(3);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(a, 1);
        x.deallocate(b, 2);
        x.deallocate(c, 3);

      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

    void test_allocate_3 () {
      A x;
      try {
        pointer a = x.allocate(2);
        pointer b = x.allocate(2);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(a, 2);
        x.deallocate(b, 2);
        CPPUNIT_ASSERT(x.valid());

      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
    }

  
    // ---------------
    // test_deallocate
    // ---------------

    void test_deallocate_1 () {
      A x;
      try {
        pointer a = x.allocate(1);
        pointer b = x.allocate(2);
        pointer c = x.allocate(3);

        x.deallocate(a, 1);
        x.deallocate(b, 2);
        x.deallocate(c, 3);

      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT(x.valid());
    }

    void test_deallocate_2 () {
      A x;
      try {
        pointer a = x.allocate(1);
        pointer b = x.allocate(2);
        x.deallocate(b, 2);
        pointer c = x.allocate(3);
        x.deallocate(a, 1);
        x.deallocate(c, 3);
        pointer d = x.allocate(4);
        x.deallocate(d, 4);

      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT(x.valid());
    }

    void test_deallocate_3 () {
      A x;
      try {
        pointer a = x.allocate(1);
        x.deallocate(a);
        pointer b = x.allocate(3);
        x.deallocate(b);
        pointer c = x.allocate(1);
        x.deallocate(c);

      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      CPPUNIT_ASSERT(x.valid());
    }


    CPPUNIT_TEST_SUITE(TestOurAllocator);
    CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST_SUITE_END();
};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
    tr.addTest(TestAllocator< std::allocator<char> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 100> >::suite());
    tr.addTest(TestAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 100> >::suite());

    // size 255
    tr.addTest(TestAllocator< Allocator<int, 255> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 255> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 255> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 255> >::suite());

                  // tests for our Allocator
    // size 100
    tr.addTest(TestOurAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestOurAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestOurAllocator< Allocator<long, 100> >::suite());
    tr.addTest(TestOurAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestOurAllocator< Allocator<char, 100> >::suite());

    // size 1000
    tr.addTest(TestOurAllocator< Allocator<int, 1000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<long, 1000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<double, 1000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<char, 1000> >::suite());

    // size 5000
    tr.addTest(TestOurAllocator< Allocator<int, 5000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<long, 5000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<double, 5000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<char, 5000> >::suite());

    // size 2121
    tr.addTest(TestOurAllocator< Allocator<int, 2121> >::suite());
    tr.addTest(TestOurAllocator< Allocator<long, 2121> >::suite());
    tr.addTest(TestOurAllocator< Allocator<double, 2121> >::suite());
    tr.addTest(TestOurAllocator< Allocator<char, 2121> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
