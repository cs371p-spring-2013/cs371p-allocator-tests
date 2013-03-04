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
#define private public
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
    // test_constructor_one
    // --------
    void test_constructor_one()
    {
        Allocator<double, 100> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);

    }
    // --------
    // test_constructor_two
    // --------
    void test_constructor_two()
    {
        Allocator<int, 50> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 42);
        CPPUNIT_ASSERT(x.view(x.a[46]) == 42);

    }
    // --------
    // test_constructor_three
    // --------
    void test_constructor_three()
    {
        Allocator<char, 10> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 2);
        CPPUNIT_ASSERT(x.view(x.a[6]) == 2);

    }

    // --------
    // test_valid_one
    // --------
    void test_valid_one()
    {
        Allocator<int, 50> x;
        x.view(x.a[0]) = 50;
        CPPUNIT_ASSERT(!x.valid());

    }
    // --------
    // test_valid_two
    // --------
    void test_valid_two()
    {
        Allocator<double, 100> x;
        x.view(x.a[96]) = 21;
        CPPUNIT_ASSERT(!x.valid());

    } 
    // --------
    // test_valid_three
    // --------
    void test_valid_three()
    {
        Allocator<char, 500> x;
        CPPUNIT_ASSERT(x.valid());

    }     

    // --------
    // test_allocate_one
    // --------
    void test_allocate_one()
    {
        Allocator<double, 100> x;
        int size = sizeof(double);
        x.allocate(5);
        CPPUNIT_ASSERT(x.view(x.a[0]) == size * -5);
        CPPUNIT_ASSERT(x.view(x.a[4 + size * 5 + 4]) == 100 - (size * 5 + 8 + 8));


    }

    // --------
    // test_allocate_two
    // --------
    void test_allocate_two()
    {
        Allocator<double, 100> x;
        int size = sizeof(double);
        x.allocate(5);
        x.allocate(3);
        CPPUNIT_ASSERT(x.view(x.a[0]) == size * -5);
        CPPUNIT_ASSERT(x.view(x.a[4 + size*5 + 4]) == size * -3);

    }

    // --------
    // test_allocate_three
    // --------
    void test_allocate_three()
    {
        Allocator<int, 50> x;
        x.allocate(10);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -42);
        CPPUNIT_ASSERT(x.view(x.a[46]) == -42);

    }
    
    // --------
    // test_deallocate_one
    // --------
    void test_deallocate_one()
    {
        Allocator<double, 100> x;
        x.deallocate(x.allocate(5));
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
        //change
    }

    // --------
    // test_deallocate_two
    // --------
    void test_deallocate_two()
    {
        Allocator<double, 100> x;
        int size = sizeof(double);
        x.allocate(2);
        x.deallocate(x.allocate(5));
        CPPUNIT_ASSERT(x.view(x.a[0]) == size * -2);
        CPPUNIT_ASSERT(x.view(x.a[4 + size * 2]) == size * -2);
        CPPUNIT_ASSERT(x.view(x.a[4 + size * 2 + 4]) == 100 - size * 2 - 8 - 8);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 100 - size * 2 - 8 - 8);
    }
    
        // --------
    // test_deallocate_three
    // --------
    void test_deallocate_three()
    {
        Allocator<int, 100> x;
        int size = sizeof(int);
        int* y = x.allocate(1);
        x.allocate(1);
        x.allocate(5);
        x.deallocate(y);
        CPPUNIT_ASSERT(x.view(x.a[0]) == size);
        CPPUNIT_ASSERT(x.view(x.a[4 + size]) == size);
        CPPUNIT_ASSERT(x.view(x.a[4 + size + 4]) == size * -1);
        CPPUNIT_ASSERT(x.view(x.a[4 + size + 4 + 4 + size]) == size * -1);
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

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_constructor_one);
    CPPUNIT_TEST(test_constructor_two);
    CPPUNIT_TEST(test_constructor_three);
    CPPUNIT_TEST(test_valid_one);
    CPPUNIT_TEST(test_valid_two);
    CPPUNIT_TEST(test_valid_three);
    CPPUNIT_TEST(test_allocate_one);
    CPPUNIT_TEST(test_allocate_two);
    CPPUNIT_TEST(test_allocate_three);
    CPPUNIT_TEST(test_deallocate_one);
    CPPUNIT_TEST(test_deallocate_two);
    CPPUNIT_TEST(test_deallocate_three);
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

    tr.run();

    cout << "Done." << endl;
    return 0;}
