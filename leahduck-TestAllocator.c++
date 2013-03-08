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
#include <iostream> // ios_base
#include <memory> // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h" // TestFixture
#include "cppunit/TextTestRunner.h" // TestRunner


#define private public
#define protected public
#define class struct

#include "Allocator.h"



// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // --------
    // test_one
    // --------
	
	//allocate, construct, destroy and deallocate 1 value
    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }
	
    // --------
    // test_two
    // --------
	
	// allocate, construct, destroy, and deallocate 10 values
    void test_two () {
        A x;
        const difference_type s = 10;
        const value_type v = 2;
        const pointer b = x.allocate(s);
              pointer e = b + s;
              pointer p = b;
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
	
	// -----
    // test_three
    // -----
	
	//test for bad alloc in allocate
	void test_three(){
		A x;
		const difference_type s = 100;
		try{
			x.allocate(s);
			CPPUNIT_ASSERT(false);
		}
		catch(...){
		}
	}
	
	// -----
    // test_four
    // -----
	
	//test for allocate
	void test_four(){
		A x;
		const difference_type s = 11;
		x.allocate(s);
	}
	
	// -----
    // test_five
    // -----
	
	//test for allocate maximum
	void test_five(){
		A x;
		const value_type v = 2;
		const difference_type s = 92 / sizeof(v);
		const pointer b = x.allocate(s);
				pointer e = b + s;
				pointer p = b;
		while (p != e) {
                x.construct(p, v);
                ++p;
		}
		try{
			x.allocate(1);
			CPPUNIT_ASSERT(false);
		}
		catch(...){
			while (b != p) {
                --p;
                x.destroy(p);
			}
			x.deallocate(b, s);
		}
	}
	
	// -----
    // test_six
    // -----
	
	//test for construct
	void test_six(){
		A x;
		const difference_type s = 10;
		const value_type v = 2;
		const pointer b = x.allocate(s);
				pointer p = b;
		x.construct(p++, v);
		x.construct(p++, v);
		x.construct(p, v);
		CPPUNIT_ASSERT(*p == v);
		x.deallocate(b, s);
	}
	
	// -----
    // test_seven
    // -----
	
	//test for correctness of construct
	void test_seven(){
		A x;
		const difference_type s = 10;
		const value_type v = 4;
		const pointer b = x.allocate(s);
				pointer e = b + s;
				pointer p = b;
		while(p!=e){
			x.construct(p, v);
			++p;
		}
		while(p!=b){
			--p;
			CPPUNIT_ASSERT(*p == v);
			x.destroy(p);
		}
		x.deallocate(b, s);
	}
	
	// -----
    // test_eight
    // -----
	
	//test for construct
	void test_eight() {
		A x;
		const difference_type s = 5;
		const value_type v = 192;
		const pointer b = x.allocate(s);
				pointer p = b;
				pointer e = b + s;
		while(p != e){
			x.construct(p, v);
			p++;
		}
		while(p != b){
			p--;
			CPPUNIT_ASSERT(*p == v);
			x.destroy(p);
		}
		x.deallocate(b, s);
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
    CPPUNIT_TEST_SUITE_END();
};

//tests that are only valid for our allocator
template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
	
	// --------
    // typedefs
    // --------

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;
	
	// --------
	// test_one
	// --------
	
	//valid test
	void test_one(){
		A x;
		CPPUNIT_ASSERT(x.valid());
		x.a[0] = 0;
		x.a[96] = -39;
		CPPUNIT_ASSERT(!x.valid());
	}
	
	// --------
	// test_two
	// --------
	
	//valid test
	void test_two(){
		A x;
		CPPUNIT_ASSERT(x.valid());
		x.a[0] *= -1;
		CPPUNIT_ASSERT(!x.valid());
	}
	
	// --------
	// test_three
	// --------
	
	//valid test
	void test_three(){
		A x;
		CPPUNIT_ASSERT(x.valid());
		++x.a[0];
		CPPUNIT_ASSERT(!x.valid());
	}
	
	// --------
    // test_four
    // --------

	//view test
	void test_four(){
		A x;
		CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
	}
	
	// --------
    // test_five
    // --------

	//view test
	void test_five(){
		A x;
		CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
	}
	
	// --------
    // test_six
    // --------
	
	//view test
	void test_six(){
		A x;
		CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
	}
	
	// -----
    // test_seven
    // -----
	
	//test for deallocate to check combining free blocks after
	void test_seven(){
		A x;
        const difference_type s = 1;
		int sent1 = x.view(x.a[0]);
		int sent2 = x.view(x.a[96]);
        const pointer p = x.allocate(s);
        x.deallocate(p, s);
		CPPUNIT_ASSERT(x.view(x.a[0]) == sent1);
		CPPUNIT_ASSERT(x.view(x.a[96]) == sent2);
	}
	
	// -----
    // test_eight
    // -----
	
	//test for deallocate to check combining blocks before
	void test_eight(){
		try { 
		  A x;
		  const difference_type s = 3;
		  const value_type v = 1;
		  //value of sentinal at the end of the array after the 
		  //blocks have been allocated
		  const difference_type t = 100 - (2 * s * sizeof(v)) - (6 * SENT_SIZE);
		  int sent1 = x.view(x.a[0]);
		  int sent2 = x.view(x.a[96]);
		  const pointer p = x.allocate(s);
		  const pointer q = x.allocate(s);
		  CPPUNIT_ASSERT(x.view(x.a[96]) == t);
		  x.deallocate(p, s);
		  x.deallocate(q, s);
		  CPPUNIT_ASSERT(x.view(x.a[0]) == sent1);
		  CPPUNIT_ASSERT(x.view(x.a[96]) == sent2);
		}
		catch (...) {
			CPPUNIT_ASSERT(false);
		}
	}
	
	// -----
    // test_nine
    // -----
	
	//test for multiple allocs/deallocs in random order
	void test_nine(){
		try {
			A x;
			const difference_type s = 1;
			int sent1 = x.view(x.a[0]);
			int sent2 = x.view(x.a[96]);
			const pointer p = x.allocate(s);
			pointer p2 = x.allocate(s);
			const pointer p3 = x.allocate(s);
			const pointer p4 = x.allocate(s);
			x.deallocate(p3, s);
			x.deallocate(p4, s);
			x.deallocate(p, s);
			x.deallocate(p2, s);
			CPPUNIT_ASSERT(x.view(x.a[0]) == sent1);
			CPPUNIT_ASSERT(x.view(x.a[96]) == sent2);
		}
		catch (...) {
			CPPUNIT_ASSERT(false);
		}
	}
	
	// -----
    // suite
    // -----
	
	CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_one);
	CPPUNIT_TEST(test_two);
	CPPUNIT_TEST(test_three);
	CPPUNIT_TEST(test_four);
	CPPUNIT_TEST(test_five);
	CPPUNIT_TEST(test_six);
	CPPUNIT_TEST(test_seven);
	CPPUNIT_TEST(test_eight);
	CPPUNIT_TEST(test_nine);
    CPPUNIT_TEST_SUITE_END();
};
// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
	tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!
	tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite());
	
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
	tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
