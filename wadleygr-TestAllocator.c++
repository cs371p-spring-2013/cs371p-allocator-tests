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

//---
// definitions for easier testing
//----
#define private public
#define protected public
#define class struct

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


	//----
	// test multiple allocates and constucts
	//----
    void test_two () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        const pointer         z = x.allocate(s);
        x.construct(z, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*z == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
		x.destroy(q);
		x.destroy(z);
        x.deallocate(q, s);
        x.deallocate(p, s);
        x.deallocate(z, s);}

    void test_4 () {
        A x;
        const difference_type s = 12;
        const value_type      v = 4;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        const pointer         z = x.allocate(s);
        x.construct(z, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*z == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
		x.destroy(q);
		x.destroy(z);
        x.deallocate(q, s);
        x.deallocate(p, s);
        x.deallocate(z, s);}


//Allocate, deallocate, allocate again
    void test_5 () {
        A x;
        const difference_type s = 12;
        const value_type      v = 4;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        x.construct(q, v);
        const pointer         z = x.allocate(s);
        x.construct(z, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*z == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
		x.destroy(q);
		x.destroy(z);
        x.deallocate(q, s);
        x.deallocate(p, s);
        x.deallocate(z, s);
		
		const pointer         t = x.allocate(s+5);
		const pointer         r = x.allocate(s+5);
		x.construct(t,v);
		x.construct(r,v);
		CPPUNIT_ASSERT(*t ==v);
		CPPUNIT_ASSERT(*r ==v);
		CPPUNIT_ASSERT(*t == *r);
		CPPUNIT_ASSERT(t != r);
		x.destroy(r);
		x.destroy(t);
		x.deallocate(r,s);
		x.deallocate(t,s);
		}


    // --------
    // test_three
	// test for sizes where remaining isn't enough for a new block
    // --------

    void test_three () {
        A x;
        const difference_type s = 46;
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
	CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_ten);
	CPPUNIT_TEST(test_4);
	CPPUNIT_TEST(test_5);
    CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one
    // --------

	//basic test
    void test_valid_1 () {
        A x;
		const difference_type s = 5;
        const value_type      v = 20;
        const pointer         p = x.allocate(s);
		CPPUNIT_ASSERT(x.valid() == true);
        x.deallocate(p, s);}

	//change a sentinel, show that its no longer valid
    void test_valid_2 () {
        A x;
		int temp;
        const difference_type s = 5;
        const value_type      v = 20;
        const pointer         p = x.allocate(s);
		CPPUNIT_ASSERT(x.valid() == true);

		temp = *reinterpret_cast<int*>(x.a);
		*reinterpret_cast<int*>(x.a) = (temp-250);
		CPPUNIT_ASSERT(x.valid() == false);
		*reinterpret_cast<int*>(x.a) = temp;
        x.deallocate(p, s);}

    void test_valid_3 () {
		A x;
		int temp;
        const difference_type s = 5;
        const value_type      v = 20;
        const pointer         p = x.allocate(s);
		CPPUNIT_ASSERT(x.valid() == true);

		temp = *reinterpret_cast<int*>(x.a);
		*reinterpret_cast<int*>(x.a) = (temp-200);
		CPPUNIT_ASSERT(x.valid() == false);
		*reinterpret_cast<int*>(x.a) = temp;
        x.deallocate(p, s);	
		}


    void test_joinBlocks_1 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);

		//---- Mimick Deallocate without joining blocks for testing
		char* q  = (char*)p;
		q -=4;
		int size = *reinterpret_cast<int*>(q);
		size *=-1; //flip sign
		*reinterpret_cast<int*>(q) = size;
		q+= (size+4);
		*reinterpret_cast<int*>(q) = size;
		
		//set r and s to the sentinels at the ends of the block for block joining
		char* start = (char*)p;
		start-=4;
		char* end = start;
		end+=(size+4);
		//----
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(start) != 192);
        x.joinBlocks(start, end);
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(start) == 192);
	}

//Tests for bad alloc. Currently commented out in suite
    void test_joinBlocks_2 () {
		using namespace std;
        A x;
        const difference_type s = 12;
        const value_type      v = 4;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        const pointer         z = x.allocate(s);
		const pointer         m = x.allocate(s);
        x.construct(p, v);
		x.construct(q, v);
        x.construct(z, v);
        x.construct(m, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*z == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
        x.destroy(m);
		x.destroy(q);
		x.destroy(z);
        x.deallocate(q, s);
        x.deallocate(p, s);
		x.deallocate(m, s);
        x.deallocate(z, s);
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(x.a) == (x.size-8));}

//Tests for bad alloc. Currently commented out in suite
    void test_joinBlocks_3 () {
        A x;
        const difference_type s = 2;
        const value_type      v = 4;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        const pointer         q = x.allocate(s);
        const pointer         m = x.allocate(s);
		const pointer         z = x.allocate(s);
        x.construct(p, v);
		x.construct(q, v);
        x.construct(z, v);
        x.construct(m, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*z == v);
        CPPUNIT_ASSERT(*q == v);
        x.destroy(p);
        x.destroy(m);
		x.destroy(q);
		x.destroy(z);
        x.deallocate(q, s);
        x.deallocate(p, s);
		x.deallocate(z, s);
        x.deallocate(m, s);
		CPPUNIT_ASSERT(*reinterpret_cast<int*>(x.a) == (x.size-8));}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
	CPPUNIT_TEST(test_joinBlocks_1);
//	CPPUNIT_TEST(test_joinBlocks_2);  //Uncomment to test for bad_alloc
	CPPUNIT_TEST(test_joinBlocks_3);
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
    tr.addTest(TestAllocator< Allocator<int, 200> >::suite());
	tr.addTest(TestMyAllocator< Allocator<int, 200> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 400> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}

