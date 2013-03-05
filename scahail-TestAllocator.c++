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
#include <cstring>   // strcmp

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define protected public
#define private public
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
        x.deallocate(p, s);
	}
	
	//test to make sure that releasing a block and combining it with it's following block works
	void test_next_combine(){
		Allocator<int, 100> x;
        int* b = x.allocate(2);
        int* b1 = x.allocate(2);
		
        x.deallocate(b1, 2);
        CPPUNIT_ASSERT(*((int*)(b+2)+1) == 76);
        
        x.deallocate(b,2);
	}
	
	//test to make sure that releasing a block and combining it with it's preceding block works
	void test_prev_combine(){
		Allocator<int, 100> x;
        int* b = x.allocate(2);
        
        int* b1 = x.allocate(2);
        
        int* b2 = x.allocate(2);
		
        x.deallocate(b1, 2);
        CPPUNIT_ASSERT(*((b+2)+1) == 8);
        
        x.deallocate(b,2);
        CPPUNIT_ASSERT(*(b-1) == 24);
        
        x.deallocate(b2, 2);
        CPPUNIT_ASSERT(*(b-1) == 92);
	}
	
	void test_no_room () {
        A x;
        const difference_type s = 40;
        try{
        	x.allocate(s);
        }
        catch(bad_alloc& e){
        	CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
	}
	
	void test_exact_amount () {
		  Allocator<double, 100> x;
		  double* p = x.allocate(11);
		  CPPUNIT_ASSERT(*(((int *)p)-1) == -92);
	}
	
	
	void test_negative_allocate () {
		A x;
		const difference_type s = -1;
		try{
			x.allocate(s);
		}
		catch(bad_alloc& e){
			CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
		}
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
        
        
    void test_valid1()
    {
		  Allocator<int, 1000> x;
		  CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_valid2
    // --------

    void test_valid2()
    {
		  Allocator<double, 100> x;
		  double* p = x.allocate(10);
		  *(((int *)p)-1) = 4;
		  CPPUNIT_ASSERT(!x.valid());
    }
    void test_valid22()
    {
		  Allocator<double, 100> x;
		  double* p = x.allocate(5);
		  *(int*)(((char *)p)-4) = 0;
		  CPPUNIT_ASSERT(!x.valid());
    }
    
    // --------
    // test_valid3
    // --------

    void test_valid3()
    {
		  Allocator<int, 100> x;
		  int* p = x.allocate(23);
		  *(p+23) = 92;
		  CPPUNIT_ASSERT(!x.valid());
    }
    

    // ------
    // test_allocate3
    // -----

    void test_allocate3()
    {
		  
		  try
		  {
		  		Allocator<double,4> x;
		    	x.allocate(1);
		  }
		  catch (bad_alloc& e)
		  {
				CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
		  }
     }
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_no_room);
    CPPUNIT_TEST(test_next_combine);
    CPPUNIT_TEST(test_prev_combine);
    CPPUNIT_TEST(test_negative_allocate);
    CPPUNIT_TEST(test_exact_amount);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid22);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_allocate3);
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
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< std::allocator<float> >::suite());
    
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 
  	tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); 
	tr.addTest(TestAllocator< Allocator<long, 100> >::suite()); 
  	tr.addTest(TestAllocator< Allocator<float, 100> >::suite());
  	
  	tr.addTest(TestAllocator< Allocator<int, 10000> >::suite()); 
  	tr.addTest(TestAllocator< Allocator<double, 1000> >::suite()); 
	tr.addTest(TestAllocator< Allocator<long, 1000> >::suite()); 
  	tr.addTest(TestAllocator< Allocator<float, 1000> >::suite());
  	
    tr.run();

    cout << "Done." << endl;
    return 0;}
