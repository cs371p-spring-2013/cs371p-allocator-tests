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
	// test_two
	// --------
	
	void test_two() {
		// Testing allocate and deallocate
		A x;
		const difference_type s  = 10;
		const pointer         p1 = x.allocate(s);
		x.deallocate(p1, s);

		const pointer         p2 = x.allocate(s);
		x.deallocate(p2, s);

	}

	// ----------
	// test_three
	// ----------

	void test_three() {
		// Testing allocate and deallocate

		// There won't be enough room to allocate s4
		//   unless the deallocation was successful
		A x;
		const difference_type s1 = 2;
		const difference_type s2 = 2;
		const difference_type s3 = 2;
		const pointer         p1 = x.allocate(s1);
		const pointer         p2 = x.allocate(s2);
		const pointer         p3 = x.allocate(s3);		
		x.deallocate(p1, s1); // Deallocate 1 then 2 then 3
		x.deallocate(p2, s2);
		x.deallocate(p3, s3);

		const difference_type s4 = 10;
		const pointer         p4 = x.allocate(s4);
		x.deallocate(p4, s4);

	}

	// ---------
	// test_four
	// ---------

	void test_four() {
		// Testing allocate and deallocate
		
		// Same as test 3, only deallocations are not in order
		A x;
		const difference_type s1 = 2;
		const difference_type s2 = 2;
		const difference_type s3 = 2;
		const pointer         p1 = x.allocate(s1);
		const pointer         p2 = x.allocate(s2);
		const pointer         p3 = x.allocate(s3);
		x.deallocate(p2, s1); // deallocate 1 then 3 then 2 (out of order)
		x.deallocate(p3, s3);
		x.deallocate(p1, s2);
		const difference_type s4 = 10;
		const pointer         p4 = x.allocate(s4);
		x.deallocate(p4, s4);
	}

	// ---------
	// test_five
	// ---------

	void test_five() {
		// Testing allocate, deallocate, construct, and destroy

		// Try constructing something of the wrong type. Make sure
		//   that it is handled the same for both types of allocators
		A x;
		const difference_type s = 1;
		const pointer         p = x.allocate(s);
		const long long       f = 1.0f;
                x.construct(p, f); // Note: this is diliberately not type value_type
                x.destroy(p);
                x.deallocate(p, s);
	}

	// --------
	// test_six
	// --------

	void test_six() {
		// Testing allocate and deallocate

		// Allocate as much of the 100 bytes as possible
		A x;
		const difference_type s = (92/sizeof(value_type));
		const pointer         p = x.allocate(s);
                x.deallocate(p, s);
	}

	// ----------
	// test_seven
	// ----------

	void test_seven() {
		// Testing allocate, deallocate, construc, and destroy

		// Allocate and construct in as much of the 100 byte
		//   as possible. 
		A x;
                const difference_type s = (92/sizeof(value_type));
                const pointer         p = x.allocate(s);
		const value_type      v = 2;
		for(int i=0; i < s; i++) {
			x.construct((p+i), v);
		}
		for(int i=0; i < s; i++) {
			x.destroy(p+i);
		}
		x.deallocate(p, s);
	}

	// ----------
	// test_eight
	// ----------

	void test_eight() {
		// Testing allocate, deallocate, construct, and destroy

		// construct and destroy at the same location numerous times
		//   to be sure that it does not cause problems.
		A x;
		const difference_type s = 1;
		const pointer         p = x.allocate(s);
                const value_type      v = 2;
		for(int i=0; i < 20; i++) {
                	x.construct(p, v);
			x.destroy(p);
		}
		x.deallocate(p, s);
	}

	// ---------
	// test_nine
	// ---------

	void test_nine() {
		// Testing allocate, deallocate, construct, and destroy

		// Allocate 4 times and construct at each location.
		//   Make sure the constructed values are correct
		A x;
		const difference_type s = 2;
		const pointer         p1 = x.allocate(s);
		const pointer         p2 = x.allocate(s);
		const pointer         p3 = x.allocate(s);
		const pointer         p4 = x.allocate(s);
		const value_type      v1 = 1;
		const value_type      v2 = 2;
		const value_type      v3 = 3;
		const value_type      v4 = 4;

		x.construct(p1, v1);
		x.construct(p2, v2);
		x.construct(p3, v3);
		x.construct(p4, v4);

		CPPUNIT_ASSERT(*p1 == v1);
		CPPUNIT_ASSERT(*p2 == v2);
		CPPUNIT_ASSERT(*p3 == v3);
		CPPUNIT_ASSERT(*p4 == v4);

		x.destroy(p1);
		x.destroy(p2);
		x.destroy(p3);
		x.destroy(p4);

		x.deallocate(p1, s);
		x.deallocate(p2, s);
		x.deallocate(p3, s);
		x.deallocate(p4, s);
		 				
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

	// -----------
	// test_eleven
	// -----------

	void test_eleven() {
		// Testing allocate, deallocate, construct, and destroy

		// Allocate numerous small blocks, construct values in each block,
		// destroy values and deallocate. Then allocate a large block.
		A x;
		const difference_type s1 = 2;
		const difference_type s2 = 2;
		const difference_type s3 = 2;
		const pointer         p1 = x.allocate(s1);
		const pointer         p2 = x.allocate(s2);
		const pointer         p3 = x.allocate(s3);		
		const value_type      v1 = 1;
		const value_type      v2 = 2;
		const value_type      v3 = 3;
	
		x.construct(p1, v1);
		x.construct(p2, v2);
		x.construct(p3, v3);
		CPPUNIT_ASSERT(*p1 == 1);
		CPPUNIT_ASSERT(*p2 == 2);
		CPPUNIT_ASSERT(*p3 == 3);
		x.destroy(p1);
		x.destroy(p2);
		x.destroy(p2);	
	
		x.deallocate(p1, s1);
		x.deallocate(p2, s2);
		x.deallocate(p3, s3);
		const difference_type s4 = 10;
		const pointer         p4 = x.allocate(s4);
		x.deallocate(p4, s4);
	
	}

	// -----------
	// test_twelve
	// -----------

	void test_twelve() {
		// Testing allocate, deallocate, construct, and destroy

		// Allocate as much of the 100 bytes as possible and then
		// construct throughout the entire array 
		A x;
                const difference_type s = (92/sizeof(value_type));
                const pointer         p = x.allocate(s);
		const value_type      v = 2;
		for(int i=0; i < s; i++) {
			x.construct((p+i), (v+i));
			CPPUNIT_ASSERT(*(p+i) == (v+i));
		}
		for(int i=0; i < s; i++) {
			x.destroy(p+i);
		}
		x.deallocate(p, s);
	}

	// -------------
	// test_thirteen
	// -------------
	
	void test_thirteen() {
		// Testing allocate, deallocate, construct, and destroy

		// construct and distroy numerous times at the same location
		// asserting that the value is correct at each new construction
		A x;
		const difference_type s = 1;
		const pointer         p = x.allocate(s);
                const value_type      v = 2;
		for(int i=0; i < 20; i++) {
                	x.construct(p, (v+i));
			CPPUNIT_ASSERT(*p = (v+i));
			x.destroy(p);
		}
		x.deallocate(p, s);
	}

	// -------------
	// test_fourteen
	// -------------

	void test_fourteen() {
		// Testing allcoate, deallocate, construct, and destroy

		// Construct numerous times at the same location without 
		// destroying. Make sure both allocators handle this
		// in a similar fashion
		A x;
		const difference_type s =1;
		const pointer         p = x.allocate(s);
		value_type            v = 1;
		for(int i=0; i < 20; i++) {
			x.construct(p, (v+i));
		}
		x.destroy(p);
		x.deallocate(p, s);

	}

	// ------------
	// test_fifteen
	// ------------

	void test_fifteen() {
		// Teting allocate, deallocate, construct, and destroy

		// Allocate a loarge block and then construct one element 
		// at the beginning of the block and one at the end.
		A x;
		const difference_type s = 10;
		const value_type      v = 2;
		const pointer         p = x.allocate(s);
		x.construct(p, v);
		x.construct((p+9), (v+9));
		CPPUNIT_ASSERT(*p == v);
		CPPUNIT_ASSERT(*(p+9) == (v+9));
		x.destroy(p);
		x.destroy(p+9);
		x.deallocate(p, s);
	}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_four);
    CPPUNIT_TEST(test_five);
    CPPUNIT_TEST(test_six);
    CPPUNIT_TEST(test_seven);
    CPPUNIT_TEST(test_eight);
    CPPUNIT_TEST(test_nine);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_eleven);
    CPPUNIT_TEST(test_twelve);
    CPPUNIT_TEST(test_thirteen);
    CPPUNIT_TEST(test_fourteen);
    CPPUNIT_TEST(test_fifteen);
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
