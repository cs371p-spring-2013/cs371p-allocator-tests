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

    /// ----------------
    /// test_constructor
    /// ----------------

    void test_constructor_1 () {
        A x;}

    void test_constructor_2 () {
        A x;
        A y(x);}

    void test_constructor_3 () {
        A x, y;
        y = x;}

    /// -------------
    /// test_allocate
    /// -------------

    void test_allocate_1 () {
        A x;
        const difference_type s = 1;
        const pointer p = x.allocate(s);
        CPPUNIT_ASSERT(p != 0);
        x.deallocate(p, s);}

    void test_allocate_2 () {
        A x;
        const difference_type s = 1;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        CPPUNIT_ASSERT(p1 != p2);
        x.deallocate(p1, s);
        x.deallocate(p2, s);}

    void test_allocate_3 () {
        A x;
        const difference_type s = x.max_size() + 1;
        try {
            const pointer p1 = x.allocate(s);
            x.deallocate(p1, s);
            CPPUNIT_ASSERT(false);
        }
        catch (std::bad_alloc& e) {} }

    void test_allocate_4 () {
        A x;
        const difference_type s = 5;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        x.deallocate(p1, s);
        x.deallocate(p2, s);}

    // ---------------
    // test_deallocate
    // ---------------

    void test_deallocate_1 () {
        A x;
        const difference_type s = 1;

        for (int i = 0; i < 1000; i++) {
            const pointer p = x.allocate(s);
            x.deallocate(p, s);}}

    void test_deallocate_2 () {
        A x;
        const difference_type s = 5;

        for (int i = 0; i < 1000; i++) {
            const pointer p1 = x.allocate(s);
            const pointer p2 = x.allocate(s);
            x.deallocate(p1, s);
            x.deallocate(p2, s);}}

    void test_deallocate_3 () {
        A x;
        difference_type s = x.max_size() / 10000000000;
        if (s <= 0)
            s = 1;

        for (int i = 0; i < 1000; i++) {
            const pointer p = x.allocate(s);
            x.deallocate(p, s);}}

    // --------------
    // test_construct
    // --------------

    void test_construct_1 () {
        A x;
        const difference_type s = 1;
        const value_type v = 1;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        assert(*p == v);
        x.deallocate(p, s);}

    void test_construct_2 () {
        A x;
        const difference_type s = 10;
        const pointer p = x.allocate(s);
        for (int i = 0; i < s; i++) {
            const value_type v = i;
            x.construct(&p[i], v);
        }
        for (int i = 0; i < s; i++) {
            const value_type v = i;
            assert(p[i] == v);
        }
        x.deallocate(p, s);}

    void test_construct_3 () {
        A x, y;
        const difference_type s = 1;
        const value_type v = 1;
        const pointer p = x.allocate(s);
        y.construct(p, v);
        assert(*p == v);
        x.deallocate(p, s);}

    // ------------
    // test_destroy
    // ------------

    void test_destroy_1 () {
        A x;
        const difference_type s = 1;
        const value_type v = 1;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        x.destroy(p);
        x.deallocate(p, s);}

    void test_destroy_2 () {
        A x;
        const difference_type s = 10;
        const pointer p = x.allocate(s);
        for (int a = 0; a < 100; a++) {
            for (int i = 0; i < s; i++) {
                const value_type v = i;
                x.construct(&p[i], v);
            }
            for (int i = 0; i < s; i++) {
                x.destroy(&p[i]);
            }
        }
        x.deallocate(p, s);}

    void test_destroy_3 () {
        A x, y;
        const difference_type s = 1;
        const value_type v = 1;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        y.destroy(p);
        x.deallocate(p, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_construct_1);
    CPPUNIT_TEST(test_construct_2);
    CPPUNIT_TEST(test_construct_3);
    CPPUNIT_TEST(test_destroy_1);
    CPPUNIT_TEST(test_destroy_2);
    CPPUNIT_TEST(test_destroy_3);
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

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
