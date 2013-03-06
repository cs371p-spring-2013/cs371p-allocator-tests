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
#include <cstring>

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner



#define private public
#define protected public
#define class struct


#include "Allocator.h"
// -------------
// TestAllocator
// -------------

template <typename A, int type_size>
struct TestOurAllocator : CppUnit::TestFixture
{
  // --------
  // typedefs
  // --------
  
  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;
  
  // --------------
  // Default_Allocator
  // --------------
  //there is only one thing to test, so we only have one method
  void test_default_constructor_1 ()
  {
    A x;
    int N = sizeof(x.a);
    CPPUNIT_ASSERT(x.view(x.a[0]) == N-8);
    CPPUNIT_ASSERT(x.view(x.a[N-4]) == N-8);
    
  }

    // --------
    // Allocate 
    // --------
      
      void test_allocate_1 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 1;
        CPPUNIT_ASSERT(x.view(x.a[0]) == N-8);
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == N-8);
        pointer         p = x.allocate(s);
		CPPUNIT_ASSERT(x.view(x.a[0]) == -1*(int)sizeof(value_type));
        CPPUNIT_ASSERT(x.view(x.a[(int)sizeof(value_type) + 4]) == -1*(int)sizeof(value_type));
        CPPUNIT_ASSERT(x.view(x.a[(int)sizeof(value_type) + 8]) == N - (16 + (int)sizeof(value_type)));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (16 + (int)sizeof(value_type)));
        x.deallocate(p, s);

	}

    void test_allocate_2 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 3;
        pointer         p = x.allocate(s);
        pointer		    q = x.allocate(s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[3*(int)sizeof(value_type) + 4]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[3*(int)sizeof(value_type) + 8]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[6*(int)sizeof(value_type) + 12]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[6*(int)sizeof(value_type) + 16]) == N - (6*(int)sizeof(value_type) + 24));
		CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (6*(int)sizeof(value_type) + 24));
        x.deallocate(p, s);
		x.deallocate(q, s);
	}

    void test_allocate_3 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = ((N-2*sizeof(int))/(sizeof(value_type)));
        pointer         p = x.allocate(s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -1*(N-8));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == -1*(N-8));
        x.deallocate(p, s);

	}

    // --------
    // Valid 
    // --------
    
    void test_valid_1 () {
        A x;
        int N = sizeof(x.a);;
        CPPUNIT_ASSERT(x.view(x.a[0]) == N-8);
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == N-8);
        x.view(x.a[0]) = N;
        CPPUNIT_ASSERT(!x.valid()); // valid should fail here
   	}


    void test_valid_2 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -1*(int)sizeof(value_type));
        CPPUNIT_ASSERT(x.view(x.a[(int)sizeof(value_type) + 4]) == -1*(int)sizeof(value_type));
        CPPUNIT_ASSERT(x.view(x.a[(int)sizeof(value_type) + 8]) == N - (16 + (int)sizeof(value_type)));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (16 + (int)sizeof(value_type)));
        x.deallocate(p, s);}

    void test_valid_3 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 3;
        const pointer         p = x.allocate(s);
		const pointer         q = x.allocate(s);
		CPPUNIT_ASSERT(x.view(x.a[0]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[3*(int)sizeof(value_type) + 4]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[3*(int)sizeof(value_type) + 8]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[6*(int)sizeof(value_type) + 12]) == -1*(int)sizeof(value_type)*3);
		CPPUNIT_ASSERT(x.view(x.a[6*(int)sizeof(value_type) + 16]) == N - (6*(int)sizeof(value_type) + 24));
		CPPUNIT_ASSERT(x.view(x.a[N-4]) == N - (6*(int)sizeof(value_type) + 24));
		x.deallocate(q, s);
        x.deallocate(p, s);}

    // --------
    // Deallocate 
    // --------


    void test_deallocate_1 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 4;
        pointer         p = x.allocate(s);

        x.deallocate(p, s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == (N-8));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == (N-8));

	}

    void test_deallocate_2 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = 3;
		const difference_type r = 7;
        pointer         p = x.allocate(s);
		pointer			q = x.allocate(r);
		x.deallocate(q, s);
		CPPUNIT_ASSERT(x.view(x.a[0]) == (-3 * (int)sizeof(value_type) ));
		CPPUNIT_ASSERT(x.view(x.a[N-4]) == (N - (16 + 3 * (int)sizeof(value_type))));
        x.deallocate(p, s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == (N-8));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == (N-8));
	}

    void test_deallocate_3 () {
        A x;
        int N = sizeof(x.a);
        const difference_type s = ((100-2*sizeof(int))/(sizeof(value_type)));
        pointer         p = x.allocate(s);

        x.deallocate(p, s);
        CPPUNIT_ASSERT(x.view(x.a[0]) == (N-8));
        CPPUNIT_ASSERT(x.view(x.a[N-4]) == (N-8));

	}

  
  CPPUNIT_TEST_SUITE(TestOurAllocator);
  CPPUNIT_TEST(test_default_constructor_1);
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
      //  CPPUNIT_TEST(test_allocate_4);
    //CPPUNIT_TEST(test_allocate_5);
    //CPPUNIT_TEST(test_allocate_6);
       // CPPUNIT_TEST(test_allocate_7);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!
  tr.addTest(TestOurAllocator< Allocator<int, 200>, 4 >::suite());
  tr.addTest(TestOurAllocator< Allocator<double, 200>, 8 >::suite());

    //tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}

