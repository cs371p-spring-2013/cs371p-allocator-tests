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


// --------
// My tests
// --------
template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    void testView(){
        A x;
        x.allocate(8);
        CPPUNIT_ASSERT(x.view(*(x.a)) == -8 * sizeof(value_type));
    }
    void testView2(){
        A x;
        pointer a = x.allocate(3);
        x.deallocate(a,3);
        CPPUNIT_ASSERT(x.view(*(x.a)) == (sizeof(x.a)) - 8);
    }
    void testView3(){
        A x;
        x.allocate(1);
        CPPUNIT_ASSERT(x.view(*(x.a)) == -1 * sizeof(value_type));
    }
    void testValid(){
        A x;
        CPPUNIT_ASSERT(x.valid());
    }
    void testValid2(){
        A x;
        x.allocate(11);
        CPPUNIT_ASSERT(x.valid());
    }
    void testValid3(){
        A x;
        pointer a = x.allocate(1);
        x.allocate(3);
        x.deallocate(a);
        CPPUNIT_ASSERT(x.valid());
    }
    void testAlloc(){
        A x;
        x.allocate(5);
        CPPUNIT_ASSERT(x.valid());
    }
    void testBadAlloc(){
        A x;
        try {
            x.allocate(2000);
            CPPUNIT_ASSERT(false);
        } catch(...){}
    }
    void testBadAlloc2(){
        A x;
        try {
            while(true)
                x.allocate(1);
            CPPUNIT_ASSERT(false);
        } catch(...){}
    }
    void testDeal(){
        A x;
        x.deallocate(x.allocate(1),2);
        x.deallocate(x.allocate(2),2);
        x.deallocate(x.allocate(3),3);
        x.deallocate(x.allocate(4),4);
        x.deallocate(x.allocate(5),5);
        x.deallocate(x.allocate(6),6);
        x.deallocate(x.allocate(7),7);
        x.deallocate(x.allocate(8),8);
        CPPUNIT_ASSERT(x.valid());
    }
    void testDeal2(){
        A x;
        pointer a = x.allocate(6);
        pointer b = x.allocate(1);
        pointer c = x.allocate(1);
        x.deallocate(a, 6);
        x.deallocate(c, 1);
        x.deallocate(b, 1);
        CPPUNIT_ASSERT(x.valid());
    }
    void testDeal3(){
        A x;
        pointer b = x.allocate(1);
        x.deallocate(b, 1);
        CPPUNIT_ASSERT(x.valid());
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(testView);
    CPPUNIT_TEST(testView2);
    CPPUNIT_TEST(testView3);
    CPPUNIT_TEST(testValid);
    CPPUNIT_TEST(testValid2);
    CPPUNIT_TEST(testValid3);
    CPPUNIT_TEST(testAlloc);
    CPPUNIT_TEST(testBadAlloc);
    CPPUNIT_TEST(testBadAlloc2);
    CPPUNIT_TEST(testDeal);
    CPPUNIT_TEST(testDeal2);
    CPPUNIT_TEST(testDeal3);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
	
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestMyAllocator< Allocator<float, 100> >::suite());
    tr.addTest(TestMyAllocator< Allocator<char, 100> >::suite());
    tr.addTest(TestMyAllocator< Allocator<bool, 100> >::suite());

    tr.addTest(TestMyAllocator< Allocator<int, 1000> >::suite());
    tr.addTest(TestMyAllocator< Allocator<double, 1000> >::suite());
    tr.addTest(TestMyAllocator< Allocator<float, 1000> >::suite());
    tr.addTest(TestMyAllocator< Allocator<char, 1000> >::suite());
    tr.addTest(TestMyAllocator< Allocator<bool, 1000> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}