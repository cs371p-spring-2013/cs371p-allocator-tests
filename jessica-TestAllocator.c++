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
  
  typedef typename A::value_type value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer pointer;
  
  // ---------
  // test_zero // allocate many pointers
  // ---------
  
  void test_zero () {
		A x;
		const difference_type s = 1;
		const value_type v = 2;
		const pointer p = x.allocate(s);
		const pointer p2 = x.allocate(2);
		const pointer p3 = x.allocate(3);
		const pointer p4 = x.allocate(1);
  }

  // --------
  // test_one // allocate 1 pointer, deallocate (should merge into 1 chunk)
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
  
  // --------
  // test_two // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // --------
  
  void test_two () {
		A x;
		const difference_type s = 1;
		const pointer p1 = x.allocate(s);
		const pointer p2 = x.allocate(s);
	
		x.deallocate(p2, s);
		// aftermath
		x.deallocate(p1, s);
  }


  // ----------
  // test_three // allocate 2 pointers, deallocate second one (should merge, 2 chunks left)
  // ---------- // then deallocate first one (should merge into 1 chunk)
  
  void test_three () {
		A x;
		const difference_type s = 1;
		const pointer p1 = x.allocate(s);
		const pointer p2 = x.allocate(s);
		x.deallocate(p2, s);
		x.deallocate(p1, s);
  }


  // ---------
  // test_four // allocate 3 pointers, deallocate second one (no merge)
  // --------- // then deallocate first one (should merge into FREE-USED-FREE)
  
  void test_four () {
		A x;
		const difference_type s = 1;
		const pointer p1 = x.allocate(s);
		const pointer p2 = x.allocate(s);
		const pointer p3 = x.allocate(s);

		x.deallocate(p2, s);
		x.deallocate(p1, s);

		// aftermath
		x.deallocate(p3, s);
  }

  // ---------
  // test_five // allocate 3 pointers, deallocate first one (no merge)
  // --------- // then deallocate second one (should merge into FREE-USED-FREE)
  
  void test_five () {
		A x;
		const difference_type s = 1;
		const pointer p1 = x.allocate(s);
		const pointer p2 = x.allocate(s);
		const pointer p3 = x.allocate(s);

		x.deallocate(p1, s);
		x.deallocate(p2, s);

		// aftermath
		x.deallocate(p3, s);
  }

  
  // -------- // allocate 3 pointers, deallocate first one (no merge)
  // test_six // then deallocate second one (should merge into FREE-USED-FREE)
  // -------- // then deallocate third one (should merge into FREE)
  
  void test_six () {
		A x;
		const difference_type s = 1;
		const pointer p1 = x.allocate(s);
		const pointer p2 = x.allocate(s);
		const pointer p3 = x.allocate(s);

		x.deallocate(p1, s);
		x.deallocate(p2, s);
		x.deallocate(p3, s);
  }
  
  // ----------
  // test_seven // checks if give a negative number
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
  // test_eight // checks if you want more than they have
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
		value_type v = 2;
		const pointer b = x.allocate(s);
		pointer e = b + s;
		pointer p = b;
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
		const value_type v = 2;
		const pointer b = x.allocate(s);
		pointer e = b + s;
		pointer p = b;
		
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
  // ------
  // Testing view
  // ------
  void test_fourteen() {
  		A x;
  		x.view(x.a[0]) = 10;
  		CPPUNIT_ASSERT(x.view(x.a[0]) == 10);
  }
  
  void test_fifteen() {
  		A x;
  		for(int i = 0; i < 20; i += 4)
  			x.view(x.a[i]) = i % 2 == 0 ? 10 : -10;
  			
  		for(int i = 0; i < 20; i += 4){
  			const int val = i % 2 == 0 ? 10 : -10;
  			CPPUNIT_ASSERT(x.view(x.a[i]) == val);
  		}
  }
  void test_sixteen() {
  		A x;
  		x.a[0] = 24;
  		CPPUNIT_ASSERT(x.view(x.a[0]) == 24);
  		x.view(x.a[32]) = 32;
  		CPPUNIT_ASSERT(x.a[32] = 32);
  		}
  //--------
  // Move	
  // -------
	
  void test_seventeen(){
  		A x;
  		int* ab = &x.view(x.a[0]);
  		int* ae = &x.view(x.a[96]);
  		int* ptr = x.move(ab, ae, 4);
  		CPPUNIT_ASSERT(*ptr == 92);
 		 
  }
   void test_eightteen(){
  		A x;
  		int* ab = &x.view(x.a[0]);
  		int* ae = &x.view(x.a[96]);
  		x.view(x.a[96]) = 1;
  		int* ptr = x.move(ab, ae, 4);
  		CPPUNIT_ASSERT(*ptr == 1);
 		 
  }
   void test_nineteen(){
  		A x;
  		int* ab = &x.view(x.a[0]);
  		int* ae = &x.view(x.a[96]);
  		int* ptr = x.move(ab, ae, 4);
  		CPPUNIT_ASSERT(*ptr == 92);
 		 
  }
  void test_twentythree(){
  		A x;
  		CPPUNIT_ASSERT(x.isFree(&x.view(x.a[0])) == true);
  }
  void test_twentyfour(){
  		A x;
  		const difference_type s = 10;
  		x.allocate(s);
  		CPPUNIT_ASSERT(x.isFree(&x.view(x.a[0])) == false);
  }
  void test_twentyfive(){
  		A x;
  		const difference_type s = 10;
  		x.allocate(s);
  		CPPUNIT_ASSERT(x.isFree(&x.view(x.a[0])) == false);
  		pointer b = reinterpret_cast<pointer>(&x.view(x.a[4]));
  		x.deallocate(b,s);
  		CPPUNIT_ASSERT(x.isFree(&x.view(x.a[0])) == true);
  }
  void test_twentysix(){
  		A x;
  		CPPUNIT_ASSERT(x.isValid(&x.view(x.a[0]), &x.view(x.a[96])) == true);
  }
  void test_twentyseven(){
  		A x;
  		const difference_type s = 1;
  		x.allocate(s);
  		CPPUNIT_ASSERT(x.isValid(&x.view(x.a[0]), &x.view(x.a[96])) == true);
  }
  void test_twentyeight(){
  		A x;
  		const difference_type s = 1;
  		pointer p = x.allocate(s);
  		char* chr = reinterpret_cast<char*>(p);
  		chr = chr - 4;
  		int* i = reinterpret_cast<int*>(chr);
  		CPPUNIT_ASSERT(x.isValid(i, &x.view(x.a[96])) == true);
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
  CPPUNIT_TEST(test_sixteen);
  CPPUNIT_TEST(test_seventeen);
  CPPUNIT_TEST(test_eightteen);
  CPPUNIT_TEST(test_nineteen);
  CPPUNIT_TEST(test_twentythree);
  CPPUNIT_TEST(test_twentyfour);
  CPPUNIT_TEST(test_twentyfive);
  CPPUNIT_TEST(test_twentysix);
  CPPUNIT_TEST(test_twentyseven);
  CPPUNIT_TEST(test_twentyeight);
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
  //tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
  
  tr.run();
  
  cout << "Done." << endl;
  return 0;
}
