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
#include <cassert>
#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator
#include <vector>	// vector
#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

// DOWNING TRICK
#define private public
#define class struct 

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

// DOWNING TRICK
#define private public
#define class struct 

using namespace std;
template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // ------------------
    // "Acceptance" Tests
    // ------------------



    // --------
    // test_one
    // --------
    
    void test_one () {
        // cout << "entering test_one" << endl;
        A x; // A is either std::allocator or our Allocator.
        // Default constructor is invoked here.
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }
    // --------
    // test_ten
    // --------

    void test_ten () {
        // cout << "entering test_ten" << endl;
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

	void test_two_allocate (){
		// cout << "entering test_two_allocate" << endl;
		A x;
		const difference_type s = 2;
		const value_type v = 2;
		const pointer b = x.allocate(s);
		const pointer k = x.allocate(s);
		x.construct(b, v);
		x.construct(k, v);
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(*k == v);
		x.destroy(b);
		x.deallocate(b, s);
		
		x.destroy(k);
		x.deallocate(k, s);
	}
	
	void test_two_allocate_2 (){
		// cout << "entering test_two_allocate_2" << endl;
		A x;
		const difference_type s = 2;
		const value_type v = 2;
		const pointer b = x.allocate(s);
		const pointer k = x.allocate(s);
		x.construct(b, v);
		x.construct(k, v);
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(*k == v);
		x.destroy(k);
		x.deallocate(k, s);
		x.destroy(b);
		x.deallocate(b, s);
	}
	
	void test_three_allocate (){
		// cout << "entering test_three_allocate" << endl;
		A x;
		// const difference_type s = 2;
		const value_type v = 2;
		const pointer b = x.allocate(v);
		const pointer k = x.allocate(v);
		const pointer c = x.allocate(v);
		x.construct(b, v);
		x.construct(k, v);
		x.construct(c, v);
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(*k == v);
		CPPUNIT_ASSERT(*c == v);
		x.destroy(k);
		x.deallocate(k, 1);
		x.destroy(b);
		x.deallocate(b, 1);
		x.destroy(c);
		x.deallocate(c, 1);
	}
	
	void test_three_allocate_2 (){
		// cout << "entering test_three_allocate_2" << endl;
		A x;
		const difference_type s = 2;
		const value_type v = 2;
		const pointer b = x.allocate(s);
		const pointer k = x.allocate(s);
		const pointer c = x.allocate(s);
		x.construct(b, v);
		x.construct(k, v);
		x.construct(c, v);
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(*k == v);
		CPPUNIT_ASSERT(*c == v);
		x.destroy(k);
		x.deallocate(k, 1);
		x.destroy(c);
		x.deallocate(c, 1);
		x.destroy(b);
		x.deallocate(b, 1);
	}



	
    // ----- 
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
	CPPUNIT_TEST(test_two_allocate);
	CPPUNIT_TEST(test_two_allocate_2);
	CPPUNIT_TEST(test_three_allocate);
	CPPUNIT_TEST(test_three_allocate_2);
	CPPUNIT_TEST_SUITE_END();
	
};

template <typename A>
struct OurTestAllocator : CppUnit::TestFixture {

	// --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    // ------------------
    // Unit Tests
    // ------------------


	void test_valid_1 () {
		// Just blank, make sure valid doesn't change anything
		// This is really already tested, but we are just being
		// explicit about it.
		A x;
		vector<char> temp;
		for (int i = 0; i < x.heap_size; ++i)
			temp.push_back(x.heap[i]);
		CPPUNIT_ASSERT(x.valid());
		for (int i = x.heap_size - 1; i >= 0; --i)
			CPPUNIT_ASSERT(x.heap[i] == temp[i]);

	}

	void test_valid_2 () {
		// Similar to above except that we are going to throw
		// some allocs and deallocs at it. Blah Blah Blah.
		A x;
		//const difference_type s = 1;
		//const value_type v = 101;
		const pointer b = x.allocate(1);
		*b = 101;
		CPPUNIT_ASSERT (*b == 101);
		char temp[x.heap_size];
		for (int i = 0; i < x.heap_size; i++) {
			CPPUNIT_ASSERT (*b == 101);
			temp[i] = x.heap[i];
		}
			
		CPPUNIT_ASSERT(x.valid());
		for (int i = x.heap_size - 1; i >= 0; --i) {
			CPPUNIT_ASSERT (*b == 101);
			CPPUNIT_ASSERT(*(reinterpret_cast<char*>(x.heap)+i) == temp[i]);
		}
 			
		x.deallocate(b);
	}

	void test_valid_3 () {
		A x;
		const difference_type s = 3;
		const value_type v = -85;
		vector<pointer> ptrs;
		int num = x.heap_size /(2*sizeof(int) + s*sizeof(value_type));

		for(int i = 0; i < num; ++i){
			pointer t = x.allocate(s);
			x.construct(t,v);
			CPPUNIT_ASSERT(*t == v);
			ptrs.push_back(t);
		}
		vector<char> temp;
		for (int i = 0; i < x.heap_size; i++) {
			temp.push_back(x.heap[i]);
		}
			
		CPPUNIT_ASSERT(x.valid());
		for (int i = x.heap_size - 1; i >= 0; --i) {
			CPPUNIT_ASSERT(x.heap[i] == temp[i]);
		}
		for ( int i = num-1; i >= 0; --i) {
			pointer t = ptrs[i];
			CPPUNIT_ASSERT(*t == v);
			x.destroy(t);
			x.deallocate(t,s);
		}
	}
	
	void test_allocate_1 () {
		// Simple alloc
		A x;
		const difference_type s = 1;
		const value_type v = 101;
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap)) == x.heap_size - 2*sizeof(int));
		pointer b = x.allocate(s);
		*b = v;
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap)) == -((signed)sizeof(value_type)));
		CPPUNIT_ASSERT(*(reinterpret_cast<pointer>(x.heap + 4)) == v);
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap + 4 + sizeof(value_type))) == -((signed)sizeof(value_type)));
		char* temp = x.heap + 4 + sizeof(value_type) + 4;
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(temp)) == x.heap_size - sizeof(value_type) - 4*sizeof(int));
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap + x.heap_size - 4)) == x.heap_size - sizeof(value_type) - 4*sizeof(int));

	}
	void test_allocate_2 () {
		A x;
		const difference_type s = 987654;
		try {
			x.allocate(s);
			CPPUNIT_ASSERT(false);
		} catch (bad_alloc&) {
			// Nothing was allocated, should be in initial state.
			CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap)) == x.heap_size - 2*sizeof(int));
			CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap + x.heap_size - sizeof(int))) == x.heap_size - 2*sizeof(int));
		}
	}
	void test_allocate_3 () {
		A x;
		const difference_type s = 1;
		const value_type v = 7;
		vector<pointer> ptrs;
		int num = x.heap_size /(2*sizeof(int) + s*sizeof(value_type));
		// cout << num << endl;	
		for(int i = 0; i < num; ++i){
			pointer t = x.allocate(s);
			x.construct(t,v);
			CPPUNIT_ASSERT(*t == v);
			ptrs.push_back(t);
		}

		try {
			x.allocate(s);
			CPPUNIT_ASSERT(false);
		} catch(bad_alloc&) {
			CPPUNIT_ASSERT(x.valid());
		}

		for ( int i = 0; i < num; ++i) {
			pointer t = ptrs[i];
			CPPUNIT_ASSERT(*t == v);
			x.destroy(t);
			x.deallocate(t,s);
		}
	}		
	
	void test_deallocate_1 () {
		// Deallocate (coalescing) from left
		A x;
		const difference_type s = 1;
		const value_type v = 2;
		vector<pointer> ptrs;
		int num = x.heap_size /(2*sizeof(int) + s*sizeof(value_type));
		// cout << num << endl;	
		for(int i = 0; i < num; ++i){
			pointer t = x.allocate(s);
			x.construct(t,v);
			CPPUNIT_ASSERT(*t == v);
			ptrs.push_back(t);
		}
		CPPUNIT_ASSERT(x.valid());
		for ( int i = 0; i < num; ++i) {
			pointer t = ptrs[i];
			CPPUNIT_ASSERT(*t == v);
			x.destroy(t);
			x.deallocate(t,s);
		}
		
		
	}
	void test_deallocate_2 () {
		// Deallocate (coalescing) from right.
		A x;
		const difference_type s = 1;
		const value_type v = 2;
		vector<pointer> ptrs;
		int num = x.heap_size /(2*sizeof(int) + s*sizeof(value_type));
		// cout << num << endl;	
		for(int i = 0; i < num; ++i){
			pointer t = x.allocate(s);
			x.construct(t,v);
			CPPUNIT_ASSERT(*t == v);
			ptrs.push_back(t);
		}
		CPPUNIT_ASSERT(x.valid());
		for ( int i = num-1; i >= 0; --i) {
			pointer t = ptrs[i];
			CPPUNIT_ASSERT(*t == v);
			x.destroy(t);
			x.deallocate(t,s);
		}
	}

	void test_deallocate_3 () {
		A x;
		const difference_type s = 4;
		const value_type v = 7;
		pointer b = x.allocate(s);
		*b = v;
		CPPUNIT_ASSERT(*b == v);
		CPPUNIT_ASSERT(x.valid());
		x.deallocate(b);
		CPPUNIT_ASSERT(x.valid());
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap)) == x.heap_size - 2*sizeof(int));
		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(x.heap + x.heap_size - sizeof(int))) == x.heap_size - 2*sizeof(int));
	}

	CPPUNIT_TEST_SUITE(OurTestAllocator);

	CPPUNIT_TEST(test_valid_1);
	CPPUNIT_TEST(test_valid_2);
	CPPUNIT_TEST(test_valid_3);
	CPPUNIT_TEST(test_allocate_1);
	CPPUNIT_TEST(test_allocate_2);
	CPPUNIT_TEST(test_allocate_3);
	CPPUNIT_TEST(test_deallocate_1);
	CPPUNIT_TEST(test_deallocate_2);
	CPPUNIT_TEST(test_deallocate_3);
	
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
    
	tr.addTest(OurTestAllocator< Allocator<long, 100> >::suite());
	tr.addTest(OurTestAllocator< Allocator<long long int, 100> >::suite());
	tr.addTest(OurTestAllocator< Allocator<long long int, 10000> >::suite());
	tr.addTest(OurTestAllocator< Allocator<short, 100> >::suite());
	tr.addTest(OurTestAllocator< Allocator<char, 100> >::suite());
	tr.addTest(OurTestAllocator< Allocator<float, 1000> >::suite());

	
    tr.run();

    cout << "Done." << endl;
    return 0;}
