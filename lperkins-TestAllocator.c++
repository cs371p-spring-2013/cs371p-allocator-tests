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
#include <cstring>

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
    // testing view - viewing integer through a char reference
    // --------

    void test_view_1() {
        Allocator<int, 20> x;
        char c[4] = {'0' - '0', '0' - '0', '0' - '0', '0' - '0'};
        CPPUNIT_ASSERT(x.view(c[0]) == 0);
    }
    
    void test_view_2() {
        Allocator<int, 20> x;
        char c[4] = {'0' - '0', '0' - '0', '0' - '0', '2' - '0'};
        CPPUNIT_ASSERT(!(x.view(c[0]) == 1));
    }

    void test_view_3() {
        Allocator<int, 20> x;
        char c[4] = {'0' - '0', '0' - '0', '1' - '0', '0' - '0'};
        CPPUNIT_ASSERT(x.view(c[0]) == (1<<16));
    }

    void test_view_4() {
        Allocator<int, 20> x;
        char c[4] = {'0' - '0', '0' - '0', '1' - '0', '0' - '0'};
        CPPUNIT_ASSERT(!(x.view(c[0]) == 1));
    }

    // --------
    // testing validity
    // --------

    // wrong block_begin_sentinel
    void test_valid_1(){
        Allocator<int, 100> x;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0])  = -16;
        x.view(x.a[20]) = -16;
        x.view(x.a[24]) =  68;
        x.view(x.a[96]) =  68;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0])  =  16;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[0])  = -16;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[24]) =  97;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[24]) =  68;
        CPPUNIT_ASSERT(x.valid());
    }

    //wrong block_end_sentinel
    void test_valid_2(){
        Allocator<int, 100> x;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0])  = -16;
        x.view(x.a[20]) = -16;
        x.view(x.a[24]) =  68;
        x.view(x.a[96]) =  68;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[20]) =  80;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[20]) = -16;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[96]) =  52;
        CPPUNIT_ASSERT(!x.valid()); 
        x.view(x.a[96]) =  68;
        CPPUNIT_ASSERT(x.valid())  ;
    }

    // wrong block_begin_sentinel
    void test_valid_3(){
        Allocator<double, 100> x;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0]) = -32;
        x.view(x.a[36]) = -32;
        x.view(x.a[40]) = 52;
        x.view(x.a[96]) = 52;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0]) = 33;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[0]) = -32;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[40]) = -52;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[40]) = 52;
        CPPUNIT_ASSERT(x.valid());
    }

    // wrong block_end_sentinel
    void test_valid_4(){
        Allocator<double, 100> x;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[0]) = -32;
        x.view(x.a[36]) = -32;
        x.view(x.a[40]) = 52;
        x.view(x.a[96]) = 52;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[36]) = 33;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[36]) = -32;
        CPPUNIT_ASSERT(x.valid());
        x.view(x.a[96]) = -5;
        CPPUNIT_ASSERT(!x.valid());
        x.view(x.a[96]) = 52;
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // testing constructor
    // --------

    // not asking for enough bytes for size type
    void test_constructor_1 (){
        try {
	    Allocator<double, 14> x;
	} catch (std::exception& e){
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
	}
    }

    // test sentinel values
    void test_constructor_2 (){
        Allocator<int, 100> x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
	CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
	Allocator<double, 64> y;
	CPPUNIT_ASSERT(y.view(y.a[0]) == 56);
	CPPUNIT_ASSERT(y.view(y.a[60]) == 56);
	Allocator<double, 16> z;
        CPPUNIT_ASSERT(z.view(z.a[0]) == 8);
	CPPUNIT_ASSERT(z.view(z.a[12]) == 8);
    }

    // test more sentinel values
    void test_constructor_3 (){
        Allocator<char, 12> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 4);
        CPPUNIT_ASSERT(x.view(x.a[8]) == 4);
        Allocator<int, 12> y;
        CPPUNIT_ASSERT(y.view(y.a[0]) == 4);
        CPPUNIT_ASSERT(y.view(y.a[8]) == 4);
        Allocator<long, 100> z;
        CPPUNIT_ASSERT(z.view(z.a[0]) == 92);
        CPPUNIT_ASSERT(z.view(z.a[96]) == 92);
    }

    // --------
    // testing allocation
    // --------
    
    // testing proper allocation
    void test_allocate_1 () {
        A x;
        const difference_type s = 2;
        const value_type v = 2;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }

    // testing invalid allocation request
    void test_allocate_2 () {
        A x;
        const difference_type s = -2;
        try{
            x.allocate(s);
        }
        catch(std::exception& e){
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

    // testing not enough memory
    void test_allocate_3 () {
        A x;
        const difference_type s1 = 5;
        const difference_type s2 = 9;
        const difference_type s3 = 10;
        x.allocate(s1);
        try {
            x.allocate(s2);
            x.allocate(s3);
        }
        catch (std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

    // testing allocate of 88 bytes but expect a block of 92 bytes
    void test_allocate_4() {
        Allocator<double,100> x;
        const difference_type s = 11;
        x.allocate(s);
        CPPUNIT_ASSERT(x.view(x.a[0])  == -92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == -92);
    }

    // --------
    // testing deallocation
    // --------

    // deallocate a free block
    void test_deallocate_1 (){
        Allocator<double, 100> x;
        int s = 1;
        double v = 2;
        double* p = x.allocate(s);
	x.deallocate(p, v);
        x.deallocate(p, v);
	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
	CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
    }

    // testing deallocate remove center then edge blocks 
    void test_deallocate_2 (){
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
	const pointer p2 = x.allocate(s);
	const pointer p3 = x.allocate(s);
        x.construct(p1, v);
	x.construct(p2, v);
	x.construct(p3, v);
	x.deallocate(p2, v);
	x.deallocate(p1, v);
	x.deallocate(p3, v);
    }

    // fill memory for double type, then deallocate edge pointers, then center
    void test_deallocate_3 (){
        Allocator<double, 100> x;
        int s = 6;
        double v = 2;
        double* p1 = x.allocate(s);
	double* p2 = x.allocate(1);
	double* p3 = x.allocate(1);
	x.deallocate(p3, v);
	x.deallocate(p1, v);
	x.deallocate(p2, v);
	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
    }

    // test deallocate with char type, fill then deallocate in reverse order of allocation
    void test_deallocate_4 (){
	Allocator<char, 100> x;
        int s = 74;
        char v = 'a';
        char* p1 = x.allocate(1);
        char* p2 = x.allocate(1);
        char* p3 = x.allocate(s);
        x.deallocate(p3, v);
        x.deallocate(p2, v);
        x.deallocate(p1, v);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);

    }

    // --------
    // testing overall functionality
    // --------

    // testing overall functionaliy - Downing test #10
    void test_overall_1 () {
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

    // test overall functionality, allocate, construct, destroy, deallocate
    void test_overall_2 () {
        A x;
	const difference_type s = 1;
        const value_type v = 2;
	const pointer p1 = x.allocate(s);
	const pointer p2 = x.allocate(s);
        x.construct(p1, v);
	x.construct(p2, v);
	CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
	x.destroy(p2);
	x.destroy(p1);
	x.deallocate(p1, s);
	x.deallocate(p2, s);
    }
    // test overall functionality allocate, deallocate, destroy then repeat
    void test_overall_3 () {
        A x;
	const difference_type s1 = 5;
	const difference_type s2 = 11;
	const value_type v1 = 2;
	const value_type v2 = 3;
	
	const pointer p1 = x.allocate(s2);
	x.construct(p1,v2);
	CPPUNIT_ASSERT(*p1 == v2);
	x.destroy(p1);
	x.deallocate(p1,s2);
        
	const pointer p2 = x.allocate(s1);
	x.construct(p2,v1);
	CPPUNIT_ASSERT(*p2 == v1);
	
	const pointer p3 = x.allocate(s1);
	x.construct(p3,v2);
	CPPUNIT_ASSERT(*p3 == v2);

	x.destroy(p2);
	x.destroy(p3);
	x.deallocate(p2, s1);
        x.deallocate(p3, s1);

	const pointer p4 = x.allocate(s2);
	x.construct(p4, v2);
	CPPUNIT_ASSERT(*p4 == v2);
	x.destroy(p4);
        x.deallocate(p4,s2);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_view_4);

    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);

    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    
    CPPUNIT_TEST(test_overall_1);
    CPPUNIT_TEST(test_overall_2);
    CPPUNIT_TEST(test_overall_3);
    
    CPPUNIT_TEST_SUITE_END();};

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

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
