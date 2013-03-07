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
        x.deallocate(b, s);
    }

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();
};

// ---------------
// myTestAllocator
// ---------------

template <typename A>
struct myTestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // ----------
    // test_valid
    // ----------
    
    void test_valid_one(){
	Allocator<int,100> x;
	CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_two(){
    	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 1;
	const pointer p = x.allocate(s);
	CPPUNIT_ASSERT(x.view(x.a[0]) == -1 * (int)(signed int)(sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[(int)(signed int)(sizeof(value_type)) + 4]) == -1 * (int)sizeof(value_type));
	CPPUNIT_ASSERT(x.view(x.a[(int)(signed int)(sizeof(value_type)) + 8]) == N - (16 + (int)sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (16 + (int)(signed int)(sizeof(value_type))));
	x.deallocate(p, s);
    }

    void test_valid_three(){
    	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 3;
	const pointer p1 = x.allocate(s);
	const pointer p2 = x.allocate(s);
	CPPUNIT_ASSERT(x.view(x.a[0]) == -1 * 3 * (int)(signed int)(sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[3 * (int)((signed int)(sizeof(value_type))) + 4]) == -1 * 3 * (int)(sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[3 * (int)((signed int)(sizeof(value_type))) + 8]) == -1 * 3 * (int)(sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[6 * (int)((signed int)(sizeof(value_type))) + 12]) == -1 * 3 * (int)(sizeof(value_type)));
	CPPUNIT_ASSERT(x.view(x.a[6 * (int)((signed int)(sizeof(value_type))) + 16]) == N - (6 * (int)(sizeof(value_type)) + 24));
	CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (6 * (int)((signed int)(sizeof(value_type))) + 24));
	x.deallocate(p1, s);	
	x.deallocate(p2, s);	
    }

    // ---------
    // test_view
    // ---------

    void test_view_one() {
        Allocator<int,100> x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
    }

    void test_view_two() {
	A x;
	const difference_type s = 1;
	const pointer p = x.allocate(s);
	CPPUNIT_ASSERT(x.view(x.a[0]) == -1 * s * (int)((signed int)(sizeof(value_type))));
	x.deallocate(p, s);
    }

    void test_view_three() {
	A x;
	const difference_type s = 4;
	const pointer p = x.allocate(s);
	CPPUNIT_ASSERT(x.view(x.a[0]) == -1 * s * (int)((signed int)(sizeof(value_type))));
	x.deallocate(p, s);
    }

    // --------
    // test_set
    // --------

    void test_set_one() {
	char a[100];
	A x;
	x.set(a[3], 7);
	CPPUNIT_ASSERT(a[3] == 7);
    }

    void test_set_two() {
	char a[100];
	A x;
	x.set(a[96], 123);
	CPPUNIT_ASSERT(a[96] == 123);
    }
    
    void test_set_three() {
	char a[100];
	A x;
	x.set(a[0], 0);
	CPPUNIT_ASSERT(a[0] == 0);
    }

    // ----------------
    // test_constructor
    // ----------------

    void test_constructor_one() {
	Allocator<int, 20> x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 12);
	CPPUNIT_ASSERT(x.view(x.a[16]) == 12);
    }

    void test_constructor_two(){
	Allocator<double, 1000> x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 992);
	CPPUNIT_ASSERT(x.view(x.a[996]) == 992);
    }

    void test_constructor_three() {
	Allocator<char, 500> x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 492);
	CPPUNIT_ASSERT(x.view(x.a[496]) == 492);
    }

    // -------------
    // test_allocate
    // -------------

    void test_allocate_one() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 400;
        try {
	    const pointer p = x.allocate(s);
	    if(N == 500){
		CPPUNIT_ASSERT(true);
	    }
	    else{
	    	CPPUNIT_ASSERT(false);
	    	x.deallocate(p, s);
	    }
	} catch(bad_alloc& e) {
	    CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_two() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s1 = 5;
	const difference_type s2 = 3;
	try {
            const pointer p1 = x.allocate(s1);
	    const pointer p2 = x.allocate(s2);
	    int index = 96;
	    if(N == 500){
	        index += 400;
	    }
	    CPPUNIT_ASSERT(x.view(x.a[0]) == -1 * s1 * (signed int)(sizeof(value_type)));
	    CPPUNIT_ASSERT(x.view(x.a[index]) == N - ((s1 + s2) * (signed int)(sizeof(value_type)) + 6 * (signed int)(sizeof(int))));
	    x.deallocate(p1, s1);
	    x.deallocate(p2, s2);
	} catch(bad_alloc& e) {
	    CPPUNIT_ASSERT(false);
	}
    }

    void test_allocate_three() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 10;
	try {
            const pointer p = x.allocate(s);
	    int result = -1 * s * (signed int)(sizeof(value_type));
	    int result2 = N - s * (signed int)(sizeof(value_type)) - 4 * (signed int)(sizeof(int));
	    int index = 96;
	    if(result < (-76 - (N - 100))) {
		result = -92;
		result2 = -92;
	    }
	    if(N == 500){
	        index += 400;
	    }
	    CPPUNIT_ASSERT(x.view(x.a[0]) == result);
	    CPPUNIT_ASSERT(x.view(x.a[index]) == result2); 
	    x.deallocate(p, s);
	} catch(bad_alloc& e) {
	    CPPUNIT_ASSERT(false);
	}
    }

    // ---------------
    // test_deallocate
    // ---------------

    void test_deallocate_one() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s1 = 2;
	const difference_type s2 = 3;
	const pointer p1 = x.allocate(s1);
	const pointer p2 = x.allocate(s2);
	x.deallocate(p1, s1);
	x.deallocate(p2, s2);
	int sentinel = 92;
	int index = 96;
	if(N == 500){
	    sentinel += 400;
	    index += 400;
	}
	CPPUNIT_ASSERT(x.view(x.a[0]) == sentinel);
	CPPUNIT_ASSERT(x.view(x.a[index]) == sentinel);
    }

    void test_deallocate_two() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 10;
	const pointer p = x.allocate(s);
	x.deallocate(p, s);
	int sentinel = 92;
	int index = 96;
	if(N == 500){
	    sentinel += 400;
	    index += 400;
	}
	CPPUNIT_ASSERT(x.view(x.a[0]) == sentinel);
	CPPUNIT_ASSERT(x.view(x.a[index]) == sentinel);
    }

    void test_deallocate_three() {
	A x;
	int N = (signed int)(sizeof(x.a));
	const difference_type s = 1;
	const pointer p = x.allocate(s);
	x.deallocate(p, s);
	int sentinel = 92;
	int index = 96;
	if(N == 500){
	    sentinel += 400;
	    index += 400;
	}
	CPPUNIT_ASSERT(x.view(x.a[0]) == sentinel);
	CPPUNIT_ASSERT(x.view(x.a[index]) == sentinel);
    }
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(myTestAllocator);
    CPPUNIT_TEST(test_valid_one);
    CPPUNIT_TEST(test_valid_two);
    CPPUNIT_TEST(test_valid_three);
    CPPUNIT_TEST(test_view_one);
    CPPUNIT_TEST(test_view_two);
    CPPUNIT_TEST(test_view_three);
    CPPUNIT_TEST(test_set_one);
    CPPUNIT_TEST(test_set_two);
    CPPUNIT_TEST(test_set_three);
    CPPUNIT_TEST(test_constructor_one);
    CPPUNIT_TEST(test_constructor_two);
    CPPUNIT_TEST(test_constructor_three);
    CPPUNIT_TEST(test_allocate_one);
    CPPUNIT_TEST(test_allocate_two);
    CPPUNIT_TEST(test_allocate_three);
    CPPUNIT_TEST(test_deallocate_one);
    CPPUNIT_TEST(test_deallocate_two);
    CPPUNIT_TEST(test_deallocate_three);
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

    // myTestAllocator
    tr.addTest(myTestAllocator< Allocator<int, 100> >::suite());
    tr.addTest(myTestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(myTestAllocator< Allocator<char, 100> >::suite());
    tr.addTest(myTestAllocator< Allocator<char, 500> >::suite());
    tr.addTest(myTestAllocator< Allocator<bool, 500> >::suite());
    tr.addTest(myTestAllocator< Allocator<long, 500> >::suite());
    tr.addTest(myTestAllocator< Allocator<short, 100> >::suite());
    tr.addTest(myTestAllocator< Allocator<double, 500> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
