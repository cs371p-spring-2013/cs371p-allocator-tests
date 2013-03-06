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

#define private public
#include "Allocator.h"
#undef private

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
        x.deallocate(p, s);
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
        x.deallocate(b, s);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();
};

struct TestMyAllocator : CppUnit::TestFixture {

    // --------
    // getSentinel
    // --------

    void test_getSentinel_1 () {
        Allocator<int, 12> x;
        CPPUNIT_ASSERT(x.getSentinel(0) == 4);
        CPPUNIT_ASSERT(x.getSentinel(8) == 4);
    }

    void test_getSentinel_2 () {
        Allocator<int, 8> x;
        CPPUNIT_ASSERT(x.getSentinel(0) == 0);
        CPPUNIT_ASSERT(x.getSentinel(4) == 0);
    }

    void test_getSentinel_3 () {
        Allocator<int, 24> x;
        int *p1;
        int *p2;
        p1 = x.allocate(1);

        CPPUNIT_ASSERT(x.getSentinel(0) == -4);
        CPPUNIT_ASSERT(x.getSentinel(8) == -4);
        CPPUNIT_ASSERT(x.getSentinel(12) == 4);
        CPPUNIT_ASSERT(x.getSentinel(20) == 4);

        p2 = x.allocate(1);

        CPPUNIT_ASSERT(x.getSentinel(0) == -4);
        CPPUNIT_ASSERT(x.getSentinel(8) == -4);
        CPPUNIT_ASSERT(x.getSentinel(12) == -4);
        CPPUNIT_ASSERT(x.getSentinel(20) == -4);

        x.deallocate(p1);
        x.deallocate(p2);

        CPPUNIT_ASSERT(x.getSentinel(0) == 16);
        CPPUNIT_ASSERT(x.getSentinel(20) == 16);
    }
    
    // --------
    // setSentinel
    // --------

    void test_setSentinel_1 () {
        Allocator<int, 12> x;
        x.setSentinel(-4, 0);
        x.setSentinel(-4, 8);
        try {
            x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_setSentinel_2 () {
        Allocator<int, 12> x;
        int *p;
        p = x.allocate(1);
        x.setSentinel(4, 0);
        x.setSentinel(4, 8);
        try {
            x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_setSentinel_3 () {
        Allocator<int, 12> x;
        x.setSentinel(-4, 0);
        x.setSentinel(-4, 8);
        CPPUNIT_ASSERT(x.getSentinel(0) == -4);
        CPPUNIT_ASSERT(x.getSentinel(8) == -4);
    }

    // --------
    // valid
    // --------

    void test_valid_1 () {
        Allocator<int, 12> x;
        x.setSentinel(8, 8);
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_2 () {
        Allocator<int, 12> x;
        x.setSentinel(-4, 0);
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_3 () {
        Allocator<int, 12> x;
        x.setSentinel(-4, 0);
        x.setSentinel(-4, 8);
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // constructor
    // --------

    void test_constructor_1 () {
        try {
            Allocator<int, 8> x;
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_constructor_2 () {
        try {
            Allocator<int, 1> x;
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_constructor_3 () {
        try {
            Allocator<int, 7> x;
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // --------
    // allocate
    // --------

    void test_allocate_1 () {
        Allocator<int, 12> x;
        try {
            int *p = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_allocate_2 () {
        Allocator<int, 12> x;
        try {
            int *p = x.allocate(2);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_3 () {
        Allocator<int, 8> x;
        try {
            int *p = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_4 () {
        Allocator<int, 12> x;
        int *p;
        try {
            p = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        try {
            p = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // --------
    // deallocate
    // --------

    void test_deallocate_1 () {
        Allocator<int, 36> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(1);
        p3 = x.allocate(1);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p2);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p4);
    }

    void test_deallocate_2 () {
        Allocator<int, 36> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(1);
        p3 = x.allocate(1);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p1);
        x.deallocate(p2);
        try {
            p4 = x.allocate(4);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p4);
    }

    void test_deallocate_3 () {
        Allocator<int, 36> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(1);
        p3 = x.allocate(1);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p3);
        x.deallocate(p2);
        try {
            p4 = x.allocate(4);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p4);
    }

    void test_deallocate_4 () {
        Allocator<int, 36> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(1);
        p3 = x.allocate(1);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p1);
        x.deallocate(p3);
        x.deallocate(p2);
        try {
            p4 = x.allocate(7);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p4);
    }

    void test_deallocate_5 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p2);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_6 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p1);
        x.deallocate(p2);
        try {
            p4 = x.allocate(5);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_7 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p2);
        x.deallocate(p3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 != p4);
    }

    void test_deallocate_8 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p1);
        x.deallocate(p2);
        x.deallocate(p3);
        try {
            p4 = x.allocate(10);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_9 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p3 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_10 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p3);
        x.deallocate(p2);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_deallocate_11 () {
        Allocator<int, 48> x;
        int *p1;
        int *p2;
        int *p3;
        int *p4;
        p1 = x.allocate(1);
        p2 = x.allocate(2);
        p3 = x.allocate(3);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p3);
        x.deallocate(p2);
        x.deallocate(p1);
        try {
            p4 = x.allocate(10);
            CPPUNIT_ASSERT(true);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p4);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_getSentinel_1);
    CPPUNIT_TEST(test_getSentinel_2);
    CPPUNIT_TEST(test_getSentinel_3);
    CPPUNIT_TEST(test_setSentinel_1);
    CPPUNIT_TEST(test_setSentinel_2);
    CPPUNIT_TEST(test_setSentinel_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
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
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST(test_deallocate_6);
    CPPUNIT_TEST(test_deallocate_7);
    CPPUNIT_TEST(test_deallocate_8);
    CPPUNIT_TEST(test_deallocate_9);
    CPPUNIT_TEST(test_deallocate_10);
    CPPUNIT_TEST(test_deallocate_11);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.addTest(TestMyAllocator::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
