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

// -------
// defines
// -------
#define private public

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator

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

// ---------------
// TestMyAllocator
// ---------------

struct MyAlloc : CppUnit::TestFixture {
    // -----------
    // test_valid1
    // -----------

    void test_valid1() {
        Allocator<char, 100> x;
        CPPUNIT_ASSERT(x.valid());
    }

    // -----------
    // test_valid2
    // -----------

    void test_valid2() {
        Allocator<double, 100> x;
        CPPUNIT_ASSERT(x.valid());
    }


    // -----------
    // test_valid3
    // -----------

    void test_valid3() {
        Allocator<double, 100> x;
        x.allocate(5);
        CPPUNIT_ASSERT(x.valid());
    }


    // -----------
    // test_valid4
    // -----------

    void test_valid4() {
        Allocator<double, 100> x;
        x.allocate(5);
        x.allocate(1);
        CPPUNIT_ASSERT(x.valid());
    }


    // ---------------
    // test_construct1
    // ---------------

    void test_construct1() {
       Allocator<char, 100> x;
       CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
       CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
       CPPUNIT_ASSERT(x.view(x.a[4]) != 92);
    }
 
    // ---------------
    // test_construct2
    // ---------------

    void test_construct2() {
       Allocator<double, 100> x;
       CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
       CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
    }
  
    // ---------------
    // test_construct3
    // ---------------

    void test_construct3() {
       try {
          Allocator<double, 4> x;
          CPPUNIT_ASSERT(false);
       } catch(...) {
       }
    }
 
    // ---------------
    // test_construct4
    // ---------------

    void test_construct4() {
       try {
          Allocator<int, 0> x;
          CPPUNIT_ASSERT(false);
       } catch(...) {
       }
    }
 
    // ---------------
    // test_allocate1
    // ---------------

    void test_allocate1() {
       Allocator<double, 100> x;

       try {
          x.allocate(9001);
          CPPUNIT_ASSERT(false);
       } catch(...) {
       }
    }

    // ---------------
    // test_allocate2
    // ---------------

    void test_allocate2() {
       Allocator<int, 100> x;
       x.allocate(1);
       CPPUNIT_ASSERT(x.valid());
    }


    // ---------------
    // test_allocate3
    // ---------------

    void test_allocate3() {
       Allocator<char, 100> x;
       x.allocate(92);
       CPPUNIT_ASSERT(x.valid());
       try {
          x.allocate(1);
          CPPUNIT_ASSERT(false);
       } catch(...) {
       }
    }


    // ---------------
    // test_allocate4
    // ---------------

    void test_allocate4() {
       Allocator<int, 100> x;
       try {
          x.allocate(0);
       } catch(...) {
       }
    }

    // ----------------
    // test_deallocate1
    // ----------------

    void test_deallocate1() {
       Allocator<int, 100> x;
       int* p = x.allocate(5);
       x.deallocate(p);
       CPPUNIT_ASSERT(x.valid());
    }
 
    // ----------------
    // test_deallocate2
    // ----------------

    void test_deallocate2() {
       Allocator<double, 100> x;
       int* p1 = x.allocate(2);
       int* p2 = x.allocate(3);
       x.deallocate(p1);
       CPPUNIT_ASSERT(x.valid());
    }
  
    // ----------------
    // test_deallocate3
    // ----------------

    void test_deallocate3() {
       Allocator<double, 100> x;
       int* p1 = x.allocate(2);
       int* p2 = x.allocate(2);
       x.deallocate(p2);
       CPPUNIT_ASSERT(x.valid());
       x.deallocate(p1);
       CPPUNIT_ASSERT(x.valid());
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(MyAlloc);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
//    CPPUNIT_TEST(test_valid4);
    CPPUNIT_TEST(test_construct1);
    CPPUNIT_TEST(test_construct2);
    CPPUNIT_TEST(test_construct3);
    CPPUNIT_TEST(test_construct4);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
//    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_allocate4);
//    CPPUNIT_TEST(test_deallocate1);
//    CPPUNIT_TEST(test_deallocate2);
//    CPPUNIT_TEST(test_allocate3);
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
//  tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
//  tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    // testing my allocator class
    tr.addTest(MyAlloc::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
