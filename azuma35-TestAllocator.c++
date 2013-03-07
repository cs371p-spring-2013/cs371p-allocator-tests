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
// TestSelfAllocator
// ---------------

template <typename A>
struct TestSelfAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    
    // --------
    // test_valid
    // --------

    void test_valid_1 () {
        A x;
		CPPUNIT_ASSERT(x.valid());}
        
    void test_valid_2 () {
        A x;
		x.allocate(10);
		CPPUNIT_ASSERT(x.valid());}
        
    void test_valid_3 () {
        A x;
		try {
			x.allocate(10000); 
		} catch (...) { }
		CPPUNIT_ASSERT(x.valid());}
        
    // --------
    // test_allocator
    // --------

    void test_allocator_1 () {
        A x;
        CPPUNIT_ASSERT(reinterpret_cast<int*>(&x.a[0]) == x.start);}
        
    void test_allocator_2 () {
        A x;
        CPPUNIT_ASSERT(reinterpret_cast<int*>(&x.a[x.M-sizeof(int)]) == x.end);}
        
    void test_allocator_3 () {
        A x;
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&x.a[0]) == (int)(x.M - 2*sizeof(int)));}
        
    // --------
    // test_allocate
    // --------

    void test_allocate_1 () {
        A x;
        const difference_type s = 10;
        const pointer p = x.allocate(s);
        x.deallocate(p, s);}
        
    void test_allocate_2 () {
        A x; 
		try {
			x.allocate(-10);
			CPPUNIT_ASSERT(false);
		} catch (std::bad_alloc& badAlloc) {}}
        
    void test_allocate_3 () {
        A x;
        try {
			x.allocate(0);
            CPPUNIT_ASSERT(false);
		} catch (std::bad_alloc& ba) {}}
        
    // --------
    // test_deallocate
    // --------

    void test_deallocate_1 () {
        A x;
		pointer p = x.allocate(1);
		x.deallocate(p);
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(&x.a[0]) == (int)(x.M - 2*sizeof(int)));}
        
    void test_deallocate_2 () {
        A x;
		pointer p = x.allocate(2);
		pointer q = x.allocate(2);
		x.deallocate(p);
		x.deallocate(q);
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(&x.a[0]) == (int)(x.M - 2*sizeof(int)));}
        
    void test_deallocate_3 () {
        A x;
		pointer p = x.allocate(1);
		pointer q= x.allocate(1);
		pointer r = x.allocate(1);
		x.deallocate(r);
		x.deallocate(p);
		x.deallocate(q);

		CPPUNIT_ASSERT(*reinterpret_cast<int*>(&x.a[0]) == (int)(x.M - 2*sizeof(int)));}
        
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestSelfAllocator);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_allocator_1);
    CPPUNIT_TEST(test_allocator_2);
    CPPUNIT_TEST(test_allocator_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
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
    
    // Self allocator
    tr.addTest(TestSelfAllocator< Allocator<char, 100> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<int, 100> >::suite()); 
	tr.addTest(TestSelfAllocator< Allocator<float, 100> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<double, 100> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<long, 100> >::suite());

	tr.addTest(TestSelfAllocator< Allocator<char, 1000> >::suite()); 
	tr.addTest(TestSelfAllocator< Allocator<int, 1000> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<float, 1000> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<double, 1000> >::suite());
	tr.addTest(TestSelfAllocator< Allocator<long, 1000> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
