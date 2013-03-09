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

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};
    
// -------------
// MyAllocator
// -------------

template <typename A>
struct MyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    void valid1 () {
        A a;
        CPPUNIT_ASSERT(a.valid());
	}
	void valid2 () {
        A a;
		a.allocate(1);
        CPPUNIT_ASSERT(a.valid());
	}
	void valid3 () {
        A a;
		pointer p = a.allocate(1);
		CPPUNIT_ASSERT(a.valid());
		a.deallocate(p,1);
        CPPUNIT_ASSERT(a.valid());
	}
	
	void view1() {
        A b;
		CPPUNIT_ASSERT(b.view(b.a[0]) == 92);
    }
	void view2() {
        A b;
		CPPUNIT_ASSERT(b.view(b.a[96]) == 92);
    }
	void view3() {
        A b;
		CPPUNIT_ASSERT(b.view(b.a[b.view(b.a[0])+4]) == 92);
    }
	
	void Allocator1() {
		Allocator<int, 100> x;
		CPPUNIT_ASSERT(x.valid());
    }

    void Allocator2(){
		Allocator<double, 100> x;
		CPPUNIT_ASSERT(x.valid());
    }

    void Allocator3() {
		Allocator<long, 100> x;
		CPPUNIT_ASSERT(x.valid());
    }
	
	void Allocate1() {
		Allocator<int, 100> x;
		x.allocate(5);
		CPPUNIT_ASSERT(x.valid());
    }

    void Allocate2(){
		Allocator<double, 100> x;
		x.allocate(2000);
		CPPUNIT_ASSERT(x.valid());
    }

    void Allocate3() {
		Allocator<long, 100> x;
		x.allocate(2);
		CPPUNIT_ASSERT(x.valid());
    }
	
	void deallocate1() {
		A x;
        pointer p = x.allocate(1);      
        x.deallocate(p, 1);
		CPPUNIT_ASSERT(x.valid());		
    }

    void deallocate2(){
		Allocator<double, 100> x;
		double * p = x.allocate(1);
		x.deallocate(p,1);
		CPPUNIT_ASSERT(x.valid());
    }

    void deallocate3() {
		Allocator<long, 100> x;
		long * p = x.allocate(2);
		x.deallocate(p,2);
		CPPUNIT_ASSERT(x.valid());
    }

    CPPUNIT_TEST_SUITE(MyAllocator);
    CPPUNIT_TEST(valid1);
	CPPUNIT_TEST(valid2);
	CPPUNIT_TEST(valid3);
	CPPUNIT_TEST(view1);
	CPPUNIT_TEST(view2);
	CPPUNIT_TEST(view3);
	CPPUNIT_TEST(Allocator1);
	CPPUNIT_TEST(Allocator2);
	CPPUNIT_TEST(Allocator3);
	CPPUNIT_TEST(Allocate1);
	CPPUNIT_TEST(Allocate2);
	CPPUNIT_TEST(Allocate3);
	CPPUNIT_TEST(deallocate1);
	CPPUNIT_TEST(deallocate2);
	CPPUNIT_TEST(deallocate3);
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
    

    tr.addTest(MyAllocator< Allocator<int, 100> >::suite()); 
    tr.addTest(MyAllocator< Allocator<double, 100> >::suite()); 

    tr.run();

    cout << "Done." << endl;
    return 0;}
