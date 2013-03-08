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
#include <cstring> // strcmp

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

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


    //During testing, I had assert(valid()) at the end of every one of my methods, 
    //once I made sure Valid was working. 




    // --------
    // test_alpha
    // --------

    void test_alpha () {
        A x;
    }

    // --------
    // test_beta
    // --------

    void test_bravo () {
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
    }

    // --------
    // test_charlie
    // --------

    void test_charlie () {
        A x;
        const difference_type s = 5;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
    }

    // --------
    // test_delta
    // --------

    void test_delta () {
        A x;
        const difference_type s = 5;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        const pointer         q = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
    }

    void test_echo () {
        A x;
        const difference_type s = 3;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        const pointer         q = x.allocate(s);
        const pointer         r = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
    }

    void test_foxtrot () {
        A x;
        const difference_type s = 1;
        const value_type      v = 6;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }

    void test_golf () {
        A x;
        const difference_type s = 3;
        const value_type      v = 6;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(q);
        x.deallocate(q, s);
    }
    void test_hotel () {
        A x;
        const difference_type s = 3;
        const value_type      v = 6;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        const pointer         r = x.allocate(s);
        x.construct(r, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
        x.deallocate(p, s);
        x.destroy(q);
        x.deallocate(q, s);
    }

    void test_india () {
        A x;
        const difference_type s = 3;
        const value_type      v = 6;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        const pointer         r = x.allocate(s);
        x.construct(r, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
        x.deallocate(p, s);
        x.destroy(q);
        x.deallocate(q, s);
        x.destroy(r);
        x.deallocate(r, s);
    }

    // --------
    // test_one
    // --------

    void test_juliet () {
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

    void test_kilo () {
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

   // ---------
  // test_zero // allocate many pointers
  // ---------
  
  void test_zero () {
    A x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    const pointer        p2 = x.allocate(2);
    const pointer        p3 = x.allocate(3);
    const pointer        p4 = x.allocate(1);
  }

  // --------
  // test_one // allocate 1 pointer, deallocate (should merge into 1 chunk)
  // --------
  
  void test_one () {
    A x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);

    x.construct(p, v);
    CPPUNIT_ASSERT(*p == v);
    x.destroy(p);
    x.deallocate(p, s);
  }
  
  // -------- 
  // test_two // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // --------
  
  void test_two () {
    A x;
    const difference_type s = 1;
    const pointer        p1 = x.allocate(s);
    const pointer        p2 = x.allocate(s);
    
    x.deallocate(p2, s);

    // aftermath
    x.deallocate(p1, s);
  }


  // ----------
  // test_three // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // ---------- // then                 deallocate first one  (should merge into 1 chunk)
  
  void test_three () {
    A x;
    const difference_type s = 1;
    const pointer        p1 = x.allocate(s);
    const pointer        p2 = x.allocate(s);
    
    x.deallocate(p2, s);
    x.deallocate(p1, s);
  }


  // ---------
  // test_four // allocate 3 pointers, deallocate second one (no merge)
  // --------- // then                 deallocate first one  (should merge into FREE-USED-FREE)
  
  void test_four () {
    A x;
    const difference_type s = 1;
    const pointer        p1 = x.allocate(s);
    const pointer        p2 = x.allocate(s);
    const pointer        p3 = x.allocate(s);
    
    x.deallocate(p2, s);
    x.deallocate(p1, s);

    // aftermath
    x.deallocate(p3, s);
  }

  // ---------
  // test_five // allocate 3 pointers, deallocate first one   (no merge)
  // --------- // then                 deallocate second one  (should merge into FREE-USED-FREE)
  
  void test_five () {
    A x;
    const difference_type s = 1;
    const pointer        p1 = x.allocate(s);
    const pointer        p2 = x.allocate(s);
    const pointer        p3 = x.allocate(s);
    
    x.deallocate(p1, s);
    x.deallocate(p2, s);

    // aftermath
    x.deallocate(p3, s);
  }

  
  // -------- // allocate 3 pointers, deallocate first one   (no merge)
  // test_six // then                 deallocate second one  (should merge into FREE-USED-FREE)
  // -------- // then                 deallocate third one   (should merge into FREE)
  
  void test_six () {
    A x;
    const difference_type s = 1;
    const pointer        p1 = x.allocate(s);
    const pointer        p2 = x.allocate(s);
    const pointer        p3 = x.allocate(s);
    
    x.deallocate(p1, s);
    x.deallocate(p2, s);
    x.deallocate(p3, s);
  }
  
  // ----------
  // test_seven  // checks if give a negative number
  // ----------
  
  void test_seven () {
    A x;
    
    try {
      pointer p = x.allocate(-5);
      std::cout << "p = " << p << std::endl;
    } catch (std::exception& e) {
      CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
    }
  }

  // ----------
  // test_eight  // checks if you want more than they have
  // ----------
  
  void test_eight () {
    A x;
    
    try {
      pointer p = x.allocate(1000);
      x.deallocate(p, 1000);
    } catch (std::exception& e) {
      CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
    }
  }
  
  // ---------
  // test_nine
  // ---------
  
  void test_nine () {
    A x;
    const difference_type s = 10;
    value_type      v = 2;
    const pointer   b = x.allocate(s);
    pointer         e = b + s;
    pointer         p = b;
    try {
      while (p != e) {
        x.construct(p, v++);
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
    
    while (b != e) {
      --e;
      CPPUNIT_ASSERT(*e == --v);
      
      x.destroy(e);
    }
    x.deallocate(b, s);
  }

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
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_alpha);
    CPPUNIT_TEST(test_bravo);
    CPPUNIT_TEST(test_charlie);
    CPPUNIT_TEST(test_delta);
    CPPUNIT_TEST(test_echo);
    CPPUNIT_TEST(test_foxtrot);
    CPPUNIT_TEST(test_golf);
    CPPUNIT_TEST(test_hotel);
    CPPUNIT_TEST(test_india);
    CPPUNIT_TEST(test_juliet);
    CPPUNIT_TEST(test_kilo);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_four);
    CPPUNIT_TEST(test_five);
    CPPUNIT_TEST(test_six);
    CPPUNIT_TEST(test_seven);
    CPPUNIT_TEST(test_eight);
    CPPUNIT_TEST(test_nine);
    CPPUNIT_TEST(test_ten);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 1100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 1244> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 10000> >::suite()); 


    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 1100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<double, 1244> >::suite()); 
    tr.addTest(TestAllocator< Allocator<double, 10000> >::suite());  

    tr.addTest(TestAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 1100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<long, 1244> >::suite()); 
    tr.addTest(TestAllocator< Allocator<long, 10000> >::suite()); 

    tr.addTest(TestAllocator< std::allocator<float> >::suite());
    tr.addTest(TestAllocator< Allocator<float, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<float, 1100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<float, 1244> >::suite()); 
    tr.addTest(TestAllocator< Allocator<float, 10000> >::suite());   

    tr.run();

    cout << "Done." << endl;
    return 0;}
