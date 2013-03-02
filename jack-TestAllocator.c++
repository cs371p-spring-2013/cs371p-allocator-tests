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
  
  // ---------
  // test_zero // allocate many pointers
  // ---------
  
  void test_zero () {
	A x;
	const difference_type s = 1;
	const value_type      v = 2;
	const pointer         p = x.allocate(s);
	const pointer        p2 = x.allocate(2);
	const pointer        p3 = x.allocate(3);
	const pointer        p4 = x.allocate(1);
  }

  // --------
  // test_one // allocate 1 pointer, deallocate (should merge into 1 chunk)
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
  // test_two // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // --------
  
  void test_two () {
	A x;
	const difference_type s = 1;
	const pointer        p1 = x.allocate(s);
	const pointer        p2 = x.allocate(s);
	
	x.deallocate(p2, s);

	// aftermath
	x.deallocate(p1, s);
  }


  // ----------
  // test_three // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // ---------- // then                 deallocate first one  (should merge into 1 chunk)
  
  void test_three () {
	A x;
	const difference_type s = 1;
	const pointer        p1 = x.allocate(s);
	const pointer        p2 = x.allocate(s);
	
	x.deallocate(p2, s);
	x.deallocate(p1, s);
  }


  // ---------
  // test_four // allocate 3 pointers, deallocate second one (no merge)
  // --------- // then                 deallocate first one  (should merge into FREE-USED-FREE)
  
  void test_four () {
	A x;
	const difference_type s = 1;
	const pointer        p1 = x.allocate(s);
	const pointer        p2 = x.allocate(s);
	const pointer        p3 = x.allocate(s);
	
	x.deallocate(p2, s);
	x.deallocate(p1, s);

	// aftermath
	x.deallocate(p3, s);
  }

  // ---------
  // test_five // allocate 3 pointers, deallocate first one   (no merge)
  // --------- // then                 deallocate second one  (should merge into FREE-USED-FREE)
  
  void test_five () {
	A x;
	const difference_type s = 1;
	const pointer        p1 = x.allocate(s);
	const pointer        p2 = x.allocate(s);
	const pointer        p3 = x.allocate(s);
	
	x.deallocate(p1, s);
	x.deallocate(p2, s);

	// aftermath
	x.deallocate(p3, s);
  }

  
  // -------- // allocate 3 pointers, deallocate first one   (no merge)
  // test_six // then                 deallocate second one  (should merge into FREE-USED-FREE)
  // -------- // then                 deallocate third one   (should merge into FREE)
  
  void test_six () {
	A x;
	const difference_type s = 1;
	const pointer        p1 = x.allocate(s);
	const pointer        p2 = x.allocate(s);
	const pointer        p3 = x.allocate(s);
	
	x.deallocate(p1, s);
	x.deallocate(p2, s);
	x.deallocate(p3, s);
  }
  
  // ----------
  // test_seven  // checks if give a negative number
  // ----------
  
  void test_seven () {
	A x;
	
	try {
	  pointer p = x.allocate(-5);
	  std::cout << "p = " << p << std::endl;
	} catch (std::exception& e) {
	  CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
	}
  }

  // ----------
  // test_eight  // checks if you want more than they have
  // ----------
  
  void test_eight () {
	A x;
	
	try {
	  pointer p = x.allocate(1000);
	  x.deallocate(p, 1000);
	} catch (std::exception& e) {
	  CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
	}
  }
  
  // ---------
  // test_nine
  // ---------
  
  void test_nine () {
	A x;
	const difference_type s = 10;
	value_type      v = 2;
	const pointer   b = x.allocate(s);
	pointer         e = b + s;
	pointer         p = b;
	try {
	  while (p != e) {
		x.construct(p, v++);
		++p;
	  }
	}
	catch (...) {
	  while (b != p) {
		--p;
		x.destroy(p);
	  }
	  x.deallocate(b, s);
	  throw;
	}
	
	while (b != e) {
	  --e;
	  CPPUNIT_ASSERT(*e == --v);
	  
	  x.destroy(e);
	}
	x.deallocate(b, s);
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
		++p;
	  }
	}
	catch (...) {
	  while (b != p) {
		--p;
		x.destroy(p);
	  }
	  x.deallocate(b, s);
	  throw;
	}
	CPPUNIT_ASSERT(std::count(b, e, v) == s);
	while (b != e) {
	  --e;
	  x.destroy(e);
	}
	x.deallocate(b, s);
  }

  void test_eleven() {
	A x;
	
	CPPUNIT_ASSERT(x.valid());
	x.a[0] = 0;
	CPPUNIT_ASSERT(!x.valid());
	x.a[0] = 92;
	CPPUNIT_ASSERT(x.valid());
  }


  void test_twelve() {
	A x;
	
	CPPUNIT_ASSERT(x.valid());
	x.a[34] = 0;
	CPPUNIT_ASSERT(x.valid());
	x.a[67] = 92;
	CPPUNIT_ASSERT(x.valid());
  }


  void test_thirteen() {
	A x;
	
	CPPUNIT_ASSERT(x.valid());
	x.a[96] = 0;
	CPPUNIT_ASSERT(!x.valid());
	x.a[67] = 92;
	CPPUNIT_ASSERT(!x.valid());
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


  // CPPUNIT_TEST(test_eleven);   // uncomment to test valid()
  // CPPUNIT_TEST(test_twelve);
  // CPPUNIT_TEST(test_thirteen);

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
  return 0;
}
