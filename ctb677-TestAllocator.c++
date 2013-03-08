// Cameron Bielstein, ctb677

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


// Defined to reveal private and protected elements for testing
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
struct CustomTestAllocator : CppUnit::TestFixture {
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


    // Series of tests for constructor
    // ------------------
    // test_constructor_1
    // ------------------

    // should work perfectly
    void test_constructor_1 () {
        try {
            A x;
        }
        catch (...) {
            CPPUNIT_ASSERT(false);
        }
    }

    // too small
    void test_constructor_2 () {
        try {
            Allocator<int, 1> x;
            CPPUNIT_ASSERT(false);
        } 
        catch(std::bad_alloc& a) {
            CPPUNIT_ASSERT(true);
        }
    }

    // exactly the right size
    void test_constructor_3 () {
        try {
            Allocator<int, 12> x;
        }
        catch (...) {
            CPPUNIT_ASSERT(false);
        }
    }

    // Series of tests for allocate
    // ---------------
    // test_allocate_1
    // ---------------
    void test_allocate_1 () {
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
        CPPUNIT_ASSERT(p != NULL);
    }

    // ---------------
    // test_allocate_2
    // ---------------

    void test_allocate_2 () {
        A x;
        const difference_type s = 10000000;
        try {
            x.allocate(s);    
            CPPUNIT_ASSERT(false);
        }
        catch (...) {
            CPPUNIT_ASSERT(true);
        }
    }

    // ---------------
    // test_allocate_3
    // ---------------

    void test_allocate_3 () {
        try {
            A x;
            const difference_type s = 1;
            const difference_type t = 2;
            const pointer         p = x.allocate(s);
            const pointer         q = x.allocate(t);
            CPPUNIT_ASSERT(p != NULL);
            CPPUNIT_ASSERT(q != NULL);
            CPPUNIT_ASSERT(p < q);
        }
        catch (...) {
            CPPUNIT_ASSERT(false);
        }
    }

    // Series of tests for deallocate
    // -----------------
    // test_deallocate_1
    // -----------------

    void test_deallocate_1 () {
        try {
            A x;
            const difference_type s = 1;
            const pointer         p = x.allocate(s);
                                      x.deallocate(p, s);
        }
        catch (...) {
            CPPUNIT_ASSERT(false);
        }

    }

    // -----------------
    // test_deallocate_3
    // -----------------

    void test_deallocate_2 () {
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
                                  x.deallocate(p, s);
        const pointer         q = x.allocate(s);
        CPPUNIT_ASSERT(p == q);
    }

    // -----------------
    // test_deallocate_3
    // -----------------

    void test_deallocate_3 () {
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
        const pointer         q = x.allocate(s);
                                  x.deallocate(p, s);
        const pointer         r = x.allocate(s);
        CPPUNIT_ASSERT( p != NULL );
        CPPUNIT_ASSERT( q != NULL );
        CPPUNIT_ASSERT( r != NULL );
        CPPUNIT_ASSERT( q > p );
        CPPUNIT_ASSERT( r == p );
    }


    // Series of tests for valid after different scenarios
    // ------------
    // test_valid_1
    // ------------

    void test_valid_1 () {
        Allocator<int, 100> x;
        *reinterpret_cast<int*>(&x.a[0]) = -4;
        *reinterpret_cast<int*>(&x.a[8]) = -4;
        *reinterpret_cast<int*>(&x.a[12]) = 80;
        *reinterpret_cast<int*>(&x.a[96]) = 80;

        CPPUNIT_ASSERT(x.valid());
    }

    // ------------
    // test_valid_2
    // ------------

    void test_valid_2 () {
        Allocator<int, 30> x;
        *reinterpret_cast<int*>(&x.a[0]) = -8;
        *reinterpret_cast<int*>(&x.a[12]) = -8;
        *reinterpret_cast<int*>(&x.a[16]) = -6;
        *reinterpret_cast<int*>(&x.a[26]) = -6;

        CPPUNIT_ASSERT(x.valid());
    }

    // ------------
    // test_valid_3
    // ------------

    // Valid after allocation uses extra space
    void test_valid_3 () {
        Allocator<int, 25> x;
        *reinterpret_cast<int*>(&x.a[0]) = -4;
        *reinterpret_cast<int*>(&x.a[8]) = -4;
        *reinterpret_cast<int*>(&x.a[12]) = -6;
        *reinterpret_cast<int*>(&x.a[22]) = -6;

        CPPUNIT_ASSERT(x.valid());
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

    CPPUNIT_TEST_SUITE(CustomTestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.addTest(CustomTestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(CustomTestAllocator< Allocator<int, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}