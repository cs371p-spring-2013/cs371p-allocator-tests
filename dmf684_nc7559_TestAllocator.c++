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
    // test_equal_one
    // --------

    void test_equal_one () {
        A x;
        A y;
        CPPUNIT_ASSERT(x == y);}

    // --------
    // test_equal_two 
    // --------

    void test_equal_two () {
        A x;
        A y;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        x.destroy(p);
        x.deallocate(p, s);
        const difference_type t = 1;
        const value_type      u = 2;
        const pointer         q = y.allocate(t);
        y.construct(q, u);
        y.destroy(q);
        y.deallocate(q, t);
        CPPUNIT_ASSERT(x == y);}

    // --------
    // test_equal_three 
    // --------

    void test_equal_three () {
        A x;
        A y;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        x.destroy(p);
        x.deallocate(p, s);
        const difference_type t = 1;
        const value_type      u = 2;
        const pointer         q = y.allocate(t);
        y.construct(q, u);
        CPPUNIT_ASSERT(x == y);
        y.destroy(q);
        y.deallocate(q, t);}


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
    CPPUNIT_TEST(test_equal_one);
    CPPUNIT_TEST(test_equal_two);
    CPPUNIT_TEST(test_equal_three);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};


template <typename A>
struct TestAllocator2 : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

   void test_one() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }

   void test_two() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }

   void test_three() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }

   void test_four() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }

   void test_five() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator2);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_four);
    CPPUNIT_TEST(test_five);
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

    tr.addTest(TestAllocator2< Allocator<int, 100> >::suite());
    tr.addTest(TestAllocator2< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}