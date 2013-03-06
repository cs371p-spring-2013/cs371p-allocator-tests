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

   // --------
    // constructor
    // --------

    void test_constructor_1 () {
        try {
            Allocator<int, 10> x;
            CPPUNIT_ASSERT(true);
	    CPPUNIT_ASSERT(x.valid());
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_constructor_2 () {
        try {
            Allocator<int, 16> x;
            CPPUNIT_ASSERT(true);
	    int* p1 = x.allocate(2);
	    x.deallocate(p1);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_constructor_3 () {
         try {
            Allocator<int, 16> x;
            CPPUNIT_ASSERT(true);
	    int* p1 = x.allocate(3);
            CPPUNIT_ASSERT(false);
	    x.deallocate(p1);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // --------
    // allocate
    // --------

    void test_allocate_1 () {
        Allocator<int, 12> x;
	int *p;
        try {
            p = x.allocate(1);
            CPPUNIT_ASSERT(true);
	    x.deallocate(p);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
    }

    void test_allocate_2 () {
        Allocator<int, 10> x;
	int *p;
        try {
            p = x.allocate(1);
            CPPUNIT_ASSERT(false);
	    x.deallocate(p);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_3 () {
        Allocator<int, 48> x;
	int *p;
        try {
            p = x.allocate(3);
            CPPUNIT_ASSERT(true);
            x.deallocate(p);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        try {
            p = x.allocate(10);
            CPPUNIT_ASSERT(true);
            x.deallocate(p);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        try {
            p = x.allocate(11);
            CPPUNIT_ASSERT(false);
            x.deallocate(p);
        }
        catch (std::bad_alloc& e) {
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
            CPPUNIT_ASSERT(true);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        x.deallocate(p2);
        try {
            p4 = x.allocate(1);
            CPPUNIT_ASSERT(true);

        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        CPPUNIT_ASSERT(p2 == p4);
	x.deallocate(p4); 
	x.deallocate(p1);
	x.deallocate(p3);
    }

    void test_deallocate_2 () {
        Allocator<value_type, 100> x;
       	value_type *p1;
        value_type *p2;
        value_type *p3;
        try {
	        p1 = x.allocate(1);
	        p2 = x.allocate(1);
		CPPUNIT_ASSERT(true);
	}
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        x.deallocate(p2);
        try {
            p3 = x.allocate(1);
            CPPUNIT_ASSERT(true);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p2 == p3);
	x.deallocate(p1); 
	x.deallocate(p3); 
    }

     void test_deallocate_3 () {
        Allocator<value_type, 100> x;
       	value_type *p1;
        value_type *p2;
        value_type *p3;
        try {
	        p1 = x.allocate(1);
	        p2 = x.allocate(1);
		CPPUNIT_ASSERT(true);
	}
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        x.deallocate(p2);
	x.deallocate(p1);
        try {
            p3 = x.allocate(3);
            CPPUNIT_ASSERT(true);
        }
        catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(false);
        }
        CPPUNIT_ASSERT(p1 == p3);
	x.deallocate(p3); 
    }


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
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST_SUITE_END();};


// -------------
// TestAllocator
// -------------

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // valid
    // --------

    void test_valid_1 () {
        Allocator<int, 12> x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_2 () {
	double v = 400;
        Allocator<double, 20> x;
	double *p = x.allocate(1); 
	new (p) double(v); 
	p++;
	new (p) double(v); 
	p--;
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_3 () {
	const int v = 400;
        Allocator<int, 12> x;
	int *p = x.allocate(1); 
	new (p) int(v); 
	p++;
	new (p) int(v); 
	p--;
        CPPUNIT_ASSERT(!x.valid());
    }



    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
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

    tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite()); // uncomment!
    tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
