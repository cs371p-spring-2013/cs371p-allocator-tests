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
#include <new>	     // bad_alloc
#include <limits>    // max_int

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

// -------
// defines
// -------

#define private public
#define protected public
#define class struct

#include "Allocator.h"

using namespace std;

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

    // ----------
    // allocate()
    // ----------

    void test_allocate_negative(){
	A x;
	const difference_type s = -1;
	try {
		x.allocate(s);
	} catch(std::bad_alloc& error) {
		CPPUNIT_ASSERT(true);
	}
	CPPUNIT_ASSERT(true);
    }

    void test_allocate_maxplusone(){
	A x;
	const difference_type s = 10000;
	try{
		x.allocate(s);
	} catch(std::bad_alloc& error) {
		CPPUNIT_ASSERT(true);
	}
	CPPUNIT_ASSERT(true);

    }

    void test_allocate_normal(){
	A x;
	const difference_type s = 2;
	const pointer p = x.allocate(s);
	
	CPPUNIT_ASSERT(p > 0);
	x.deallocate(p, 2);
    }
	
    // ------------
    // deallocate()
    // ------------

    void test_deallocate(){
	A x;
	const difference_type s = 10;
	const pointer p = x.allocate(s);
	const pointer t = x.allocate(s);
	x.deallocate(p, 10);
	x.deallocate(t, 10);

	const pointer u = x.allocate(s);

	CPPUNIT_ASSERT(u > 0);
	
	x.deallocate(u, 10);
    }   

    void test_deallocate_2(){
	A x;
	const difference_type s = 10;
	const difference_type z = 20;

	const pointer p = x.allocate(s);
	const pointer t = x.allocate(s);

	x.deallocate(p, 10);
	x.deallocate(t, 10);
	
	const pointer u = x.allocate(z);

	CPPUNIT_ASSERT(u > 0);

	x.deallocate(u, 20);

    }


    void test_deallocate_3(){
	A x;
	const difference_type s = 10;
	const difference_type z = 20;

	const pointer p = x.allocate(s);
	const pointer t = x.allocate(s);

	x.deallocate(t, 10);
	x.deallocate(p, 10);

	const pointer u = x.allocate(z);

	CPPUNIT_ASSERT(u >0);

	x.deallocate(u, 20);

    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_allocate_negative);
    CPPUNIT_TEST(test_allocate_maxplusone);
    CPPUNIT_TEST(test_allocate_normal);
    CPPUNIT_TEST(test_deallocate);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST_SUITE_END();};

// ----------------
// TestNewAllocator
// ----------------

template <typename A>
struct TestNewAllocator : CppUnit::TestFixture {

   // --------
   // typedefs
  // ---------

   typedef typename A::value_type value_type;
   typedef typename A::difference_type difference_type;
   typedef typename A::pointer pointer;


   // -----
   // valid
   // -----


   void test_valid(){
	A x;
 	*reinterpret_cast<int*>(x.a + 14) = 8;
	*reinterpret_cast<int*>(x.a) = 10;
        CPPUNIT_ASSERT(x.valid() == false);
	

   }

   void test_valid_2(){
	A x;
	int oldValue = *reinterpret_cast<int*>(x.a);
	*reinterpret_cast<int*>(x.a) = 50;
	*reinterpret_cast<int*>(x.a + 54) = 200;
        *reinterpret_cast<int*>(x.a + oldValue) = 50;

        CPPUNIT_ASSERT(x.valid() == false);
   }


   void test_valid_3(){
	A x;
	int oldValue = *reinterpret_cast<int*>(x.a);
	*reinterpret_cast<int*>(x.a) = 50;
	*reinterpret_cast<int*>(x.a + 54) = 50;
	*reinterpret_cast<int*>(x.a + 58) = oldValue - 50 - 8;
	*reinterpret_cast<int*>(x.a + oldValue + 4) = oldValue - 50 - 8;

	CPPUNIT_ASSERT(x.valid() == true);

   } 


   void test_constructor(){
	try{
		Allocator<int, 3> x;
	}catch(bad_alloc& e){
		CPPUNIT_ASSERT(true);

	}

	CPPUNIT_ASSERT(true);

   }

  
   void test_constructor_2(){
	Allocator<int, 12> x;
	CPPUNIT_ASSERT(x.end - x.start == 3);
   }

   void test_constructor_3(){
	A x;
	CPPUNIT_ASSERT(x.valid());
   }
 

   // -----
   // suite
   // -----

   CPPUNIT_TEST_SUITE(TestNewAllocator);
   CPPUNIT_TEST(test_valid);
   CPPUNIT_TEST(test_valid_2);
   CPPUNIT_TEST(test_valid_3);
   CPPUNIT_TEST(test_constructor);
   CPPUNIT_TEST(test_constructor_2);
   CPPUNIT_TEST(test_constructor_3);
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
    tr.addTest(TestAllocator< Allocator<double, 200> >::suite()); // uncomment!

    tr.addTest(TestNewAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestNewAllocator< Allocator<double, 200> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
