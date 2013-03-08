// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
//
// Modified by Eli K Miller
// March 2nd, 2013
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
		
	// --------
	// test_set_1
	// --------

	void test_set_1 () {
		A x;
		x.set(x.a[0], 3);
		CPPUNIT_ASSERT(x.view(x.a[0]) == 3); }

	// --------
	// test_set_2
	// --------

	void test_set_2 () {
		A x;
		x.set(x.a[96], -92);
		CPPUNIT_ASSERT(x.view(x.a[96]) == -92); }
	
	// --------
	// test_valid_1
	// --------

	void test_valid_1 () {
		A x;
		x.set(x.a[0], -20);
		x.set(x.a[24], -20);
		x.set(x.a[28], 64);
		x.set(x.a[96], 64);
		CPPUNIT_ASSERT(x.valid()); }
	
	// --------
	// test_valid_2
	// --------

	void test_valid_2 () {
		A x;
		x.set(x.a[0], -20);
		x.set(x.a[24], -20);
		x.set(x.a[28], 64);
		x.set(x.a[96], 63);
		CPPUNIT_ASSERT(!x.valid()); }
	
	// --------
	// test_valid_3
	// --------

	void test_valid_3 () {
		A x;
		x.set(x.a[0], -4);
		x.set(x.a[8], -4);
		x.set(x.a[12], 8);
		x.set(x.a[24], 8);
		x.set(x.a[28], -64);
		x.set(x.a[96], -64);
		CPPUNIT_ASSERT(x.valid()); }
		
	// --------
	// test_default_constructor_1
	// --------

	void test_default_constructor_1 () {
		A x = {};
		CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
		CPPUNIT_ASSERT(x.view(x.a[96]) == 92); }
	
	// --------
	// test_allocate_1
	// --------

	void test_allocate_1 () {
		Allocator<int,100> x = {};
		x.allocate(5);
		CPPUNIT_ASSERT((x.view(x.a[0])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[24])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[28])) == 64);
		CPPUNIT_ASSERT((x.view(x.a[96])) == 64); }
	
	// --------
	// test_allocate_2
	// --------

	void test_allocate_2 () {
		Allocator<int,100> x = {};
		x.allocate(5);
		x.allocate(10);
		CPPUNIT_ASSERT((x.view(x.a[0])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[24])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[28])) == -40);
		CPPUNIT_ASSERT((x.view(x.a[72])) == -40);
		CPPUNIT_ASSERT((x.view(x.a[76])) == 16);
		CPPUNIT_ASSERT((x.view(x.a[96])) == 16); }
	
	// --------
	// test_allocate_3
	// --------

	void test_allocate_3 () {
		Allocator<int,100> x = {};
		x.allocate(5);
		x.allocate(13);
		CPPUNIT_ASSERT((x.view(x.a[0])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[24])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[28])) == -64);
		CPPUNIT_ASSERT((x.view(x.a[96])) == -64); }	
		
	// --------
	// test_allocate_4
	// --------

	void test_allocate_4 () {
		Allocator<int,100> x = {};
		try {
			x.allocate(100); }
		catch(...) {
			CPPUNIT_ASSERT(true); }
		CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
		CPPUNIT_ASSERT(x.view(x.a[96]) == 92); }
		
	// --------
	// test_deallocate_1
	// --------

	void test_deallocate_1 () {
		Allocator<int,100> x = {};
		int* p = x.allocate(5);
		x.deallocate(p);
		CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
		CPPUNIT_ASSERT(x.view(x.a[96]) == 92); }
	
	// --------
	// test_deallocate_2
	// --------

	void test_deallocate_2 () {
		Allocator<int,100> x = {};
		x.allocate(5);
		int* p = x.allocate(10);
		x.deallocate(p);
		CPPUNIT_ASSERT((x.view(x.a[0])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[24])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[28])) == 64);
		CPPUNIT_ASSERT((x.view(x.a[96])) == 64); }
	
	// --------
	// test_deallocate_3
	// --------

	void test_deallocate_3 () {
		Allocator<int,100> x = {};
		int* p = x.allocate(5);
		int* q = x.allocate(5);
		x.allocate(5);
		x.deallocate(p);
		x.deallocate(q);
		CPPUNIT_ASSERT((x.view(x.a[0])) == 48);
		CPPUNIT_ASSERT((x.view(x.a[52])) == 48);
		CPPUNIT_ASSERT((x.view(x.a[56])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[80])) == -20);
		CPPUNIT_ASSERT((x.view(x.a[84])) == 8);
		CPPUNIT_ASSERT((x.view(x.a[96])) == 8); }

	// -----
	// suite
	// -----

	CPPUNIT_TEST_SUITE(TestAllocator);
	CPPUNIT_TEST(test_one);
	CPPUNIT_TEST(test_ten);
	CPPUNIT_TEST(test_set_1);
	CPPUNIT_TEST(test_set_2);
	CPPUNIT_TEST(test_valid_1);
	CPPUNIT_TEST(test_valid_2);
	CPPUNIT_TEST(test_valid_3);
	CPPUNIT_TEST(test_default_constructor_1);
	CPPUNIT_TEST(test_allocate_1);
	CPPUNIT_TEST(test_allocate_2);
	CPPUNIT_TEST(test_allocate_3);
	CPPUNIT_TEST(test_allocate_4);
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

	//tr.addTest(TestAllocator< std::allocator<int> >::suite());
	tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

	//tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

	tr.run();

	cout << "Done." << endl;
	return 0; }