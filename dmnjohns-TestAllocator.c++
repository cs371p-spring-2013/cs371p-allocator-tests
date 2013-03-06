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

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    
    // --------
    // test_view
    // --------

    void test_view_1 () {
        A x;
		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    void test_view_2 () {
        A x;
		x.allocate(1);
		CPPUNIT_ASSERT(x.view(*(x.a)) == -static_cast<int>(sizeof(value_type)));}
        
    void test_view_3 () {
        A x;
        union T {
            int i;
            char c;
        };
        T t;
        t.i = 10101;
        CPPUNIT_ASSERT(x.view(t.c) == t.i);
        CPPUNIT_ASSERT(&x.view(t.c) == &t.i);}
        
    // --------
    // test_valid
    // --------

    void test_valid_1 () {
        A x;
		CPPUNIT_ASSERT(x.valid());}
        
    void test_valid_2 () {
        A x;
		x.allocate(5);
		CPPUNIT_ASSERT(x.valid());}
        
    void test_valid_3 () {
        A x;
		try {
			x.allocate(x.blocks_allocated / 2);
			x.allocate(x.blocks_allocated / 2); 
		} catch (...) { }


		CPPUNIT_ASSERT(x.valid());}
        
    // --------
    // test_allocator
    // --------

    void test_allocator_1 () {
        A x;
        CPPUNIT_ASSERT(*((int*)&x.a[0]) == x.blocks_allocated - 8);
        CPPUNIT_ASSERT(*((int*)&x.a[x.blocks_allocated - 4]) == x.blocks_allocated - 8);}
        
    // --------
    // test_allocate
    // --------

    void test_allocate_1 () {
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
        x.deallocate(p, s);}
        
    void test_allocate_2 () {
        A x; 
		try {
			x.allocate(-1);
			CPPUNIT_ASSERT(false);
		} catch (std::bad_alloc& ba) {}}
        
    void test_allocate_3 () {
        A x;
        try {
			x.allocate(x.blocks_allocated);
            CPPUNIT_ASSERT(false);
		} catch (std::bad_alloc& ba) {}}
        
    void test_allocate_4 () {
        A x;
        try {
			pointer p = x.allocate(0);
            CPPUNIT_ASSERT(p == 0);
		} catch (std::bad_alloc& ba) {}}
        
    // --------
    // test_deallocate
    // --------

    void test_deallocate_1 () {
        A x;
		pointer b = x.allocate(1);
		x.deallocate(b);
		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    void test_deallocate_2 () {
        A x;
		pointer b = x.allocate(1);
		pointer c = x.allocate(1);
		x.deallocate(b);
		x.deallocate(c);
		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    void test_deallocate_3 () {
        A x;
		pointer b = x.allocate(1);
		pointer c = x.allocate(1);
		pointer d = x.allocate(1);
		pointer e = x.allocate(1);
		x.deallocate(d);
		x.deallocate(b);
		x.deallocate(e);
		x.deallocate(c);

		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    void test_deallocate_4 () {
        A x;
		pointer b;
		pointer c;
		pointer d;
		pointer e;
		try {
			b = x.allocate(2);
			c = x.allocate(1);
			d = x.allocate(2);
			e = x.allocate(1);
		} catch (...) {
			CPPUNIT_ASSERT(false);
		}

		x.deallocate(d);
		x.deallocate(b);
		x.deallocate(e);
		x.deallocate(c);

		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    void test_deallocate_5 () {
        A x;
		pointer b = x.allocate(1);
		x.deallocate(b);
        b = x.allocate(1);
		x.deallocate(b);
		CPPUNIT_ASSERT(x.view(*(x.a)) == x.blocks_allocated - 8);}
        
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_allocator_1);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;
    CppUnit::TextTestRunner tr;

    // Both allocators
    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< std::allocator<float> >::suite());
    tr.addTest(TestAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<float, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    
    // My allocator
	tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite()); 
	tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite());
	tr.addTest(TestMyAllocator< Allocator<float, 100> >::suite());
	tr.addTest(TestMyAllocator< Allocator<char, 100> >::suite());
	tr.addTest(TestMyAllocator< Allocator<long, 100> >::suite());

	tr.addTest(TestMyAllocator< Allocator<int, 1000> >::suite()); 
	tr.addTest(TestMyAllocator< Allocator<double, 1000> >::suite());
	tr.addTest(TestMyAllocator< Allocator<float, 1000> >::suite());
	tr.addTest(TestMyAllocator< Allocator<char, 1000> >::suite()); 
	tr.addTest(TestMyAllocator< Allocator<long, 1000> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
