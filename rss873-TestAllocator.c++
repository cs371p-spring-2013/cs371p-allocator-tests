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
% g++ -pedantic -std=c++0x -Wall Allocator.h TestAllocator.c++ -o TestAllocator -lcppunit -ldl
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
    void test_allocate_one () {
        A x;
        const pointer p = x.allocate(5);
        CPPUNIT_ASSERT(reinterpret_cast<char*>(p) == (x.a + 4));

    }
    void test_allocate_two () {
      A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.deallocate(p, s);

    }
    void test_allocate_three () {
   		A x;
			CPPUNIT_ASSERT(x.valid());
			x.a[34] = 0;
			CPPUNIT_ASSERT(x.valid());
			x.a[67] = 92;
			CPPUNIT_ASSERT(x.valid());
    }
    void test_deallocate_one () {
        A x;
 		//std::cout<<"This Far"<<std::endl;
        const pointer p = x.allocate(20);
   
        x.deallocate(p);
		const pointer q = x.allocate(20);
        x.deallocate(q);
		const pointer r = x.allocate(1);
        
        CPPUNIT_ASSERT(reinterpret_cast<char*>(r) == (x.a + 4));
    }
    void test_deallocate_two () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.deallocate(p, s);
    }
    void test_deallocate_three () {
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
    
  // ----------------
    // validity tests
    // ----------------

    //test: newly constructed object should be valid
    void test_valid_new(){
      A x;
      CPPUNIT_ASSERT(x.valid());
    }

    //test: just write over first sentinel
    void test_valid_missing_start(){
      A x;
      int* start = reinterpret_cast<int*> (x.a);
      *start = 0;
      ++start;
      *start = 1;
      bool valid = x.valid();
      CPPUNIT_ASSERT(!x.valid());
    }

    //test: just write over last sentinel
    void test_valid_missing_end(){
      A x;
      int n = sizeof(x.a);
      x.a[n-1] = 0;
      x.a[n-2] = 0;
      x.a[n-3] = 0;
      x.a[n-4] = 0;
      CPPUNIT_ASSERT(!x.valid());

    }
    // --------
    // test_ten
    // --------

    void test_ten () {
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
        x.deallocate(b, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_allocate_one);
    CPPUNIT_TEST(test_allocate_two);
    CPPUNIT_TEST(test_allocate_three);
    CPPUNIT_TEST(test_deallocate_one);
    CPPUNIT_TEST(test_deallocate_two);
    CPPUNIT_TEST(test_deallocate_three);
    CPPUNIT_TEST(test_valid_new);
    CPPUNIT_TEST(test_valid_missing_start);
    CPPUNIT_TEST(test_valid_missing_end);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    //tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

		//tr.addTest(TestAllocator< std::allocator<double> >::suite());
		tr.addTest(TestAllocator< Allocator<double, 200> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
