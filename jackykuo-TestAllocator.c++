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
#include <cstring>
#include <cassert> // assert

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
  
  // --------
  // test_one 
  // --------
  
void test_one () {
A x;
const difference_type s = 1;
const value_type v = 2;
const pointer p = x.allocate(s); // allocate 1 block of the type_size

x.construct(p, v); // construct a new type T with value v at location p
CPPUNIT_ASSERT(*p == v); // assert p point to the address with value v
x.destroy(p); // deconstruct object p
x.deallocate(p, s); // deallocate the block, turn sentinal from negative to positive, merge into 1 chunk
}
  
  // --------
  // test_two
  // --------
  
void test_two () {
A x;
const difference_type s = 2;
// allocate 2 pointers, with type_size 2
const pointer p1 = x.allocate(s);
const pointer p2 = x.allocate(s);
// deallocate second pointer, merge to right
x.deallocate(p2, s);
// deallocate first pointer
x.deallocate(p1, s);
  }


  // ----------
  // test_three //
  // ---------- //
  
void test_three () {
A x;
const difference_type s = 2;
const pointer p1 = x.allocate(s);
const pointer p2 = x.allocate(s);
const pointer p3 = x.allocate(s);

x.deallocate(p3, s);
x.deallocate(p1, s);
x.deallocate(p2, s); // merge p2 block with left and right chunk

  }


  // ---------
  // test_four //
  // --------- //
  
void test_four () {
A x;
const difference_type s = 11;
const pointer p1 = x.allocate(s); // test allocate 11*double+8 = 96, 4 byte left, make sure valid() won't go out of bound

x.deallocate(p1, s);
  }

  // ---------
  // test_five //
  // --------- //
  
  void test_five () {
A x;
const difference_type s = 3;
const pointer p1 = x.allocate(s);
const pointer p2 = x.allocate(s);
const pointer p3 = x.allocate(s);

x.deallocate(p1, s); // no merge, make sure don't merge left
x.deallocate(p2, s); // merge left

x.deallocate(p3, s); // merge left and right
  }

  
  // -------- //
  // test_six //
  // -------- //
  
void test_six () {
A x;

value_type v = 2;
difference_type s = 2;

const pointer p1 = x.allocate(s);
x.construct(p1, v);

v = 3;
const pointer p2 = x.allocate(s);
x.construct(p2, v);

v = 4;
const pointer p3 = x.allocate(s);
x.construct(p3, v);
// p1 has valus 2, p2 has value 3, p3 has value 4
CPPUNIT_ASSERT(*p1 ==(value_type)2);
CPPUNIT_ASSERT(*p2 == (value_type)3);
CPPUNIT_ASSERT(*p3 == (value_type)4);
x.destroy(p1);
x.destroy(p2);
x.destroy(p3);
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
pointer p = x.allocate(-10);
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
const difference_type s = 4;
value_type v = 5;
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

  
// turn off tr.addTest(TestAllocator< allocator<int> >::suite()) and tr.addTest(TestAllocator< allocator<int> >::suite()) in main()
// before test the following method
  
  void test_eleven() {
A x;
const difference_type s = 1;
const value_type v = 2;
const pointer p = x.allocate(s);

x.construct(p, v);
assert(*p == v);
assert(x.valid());
x.deallocate(p);
assert(x.valid());
  }


  void test_twelve() {
A x;
const difference_type s = 10;
value_type v = 2;
const pointer b = x.allocate(s);
pointer e = b + s;
pointer p = b;
try {
while (p != e) {
x.construct(p, v++);
assert(x.valid()); // assert x.valid() work
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
assert(x.valid()); //assert x.valid() work
  }


  void test_thirteen() {
A x;
const difference_type s = 6;

try{
pointer p1 = x.allocate(s);
pointer p2 = x.allocate(s);
pointer p3 = x.allocate(s);
assert(x.valid()); // assert x.valid() work
x.deallocate(p1);
x.deallocate(p2);
x.deallocate(p3);
assert(x.valid()); // assert x.valid() work
} catch (std::exception& e) {
CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
}

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

  // turn off tr.addTest(TestAllocator< allocator<int> >::suite()) and tr.addTest(TestAllocator< allocator<int> >::suite()) in main()
  // before test the following method 
  
   CPPUNIT_TEST(test_eleven); 
   CPPUNIT_TEST(test_twelve);
   CPPUNIT_TEST(test_thirteen);

    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
  using namespace std;
  //ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
  cout << "TestAllocator.c++" << endl;
  
  CppUnit::TextTestRunner tr;
  
  //tr.addTest(TestAllocator< allocator<int> >::suite());
  tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 
  //tr.addTest(TestAllocator< Allocator<int, 20> >::suite()); // expected to fail because all the test cases are designed to test the heap with a size of 100. 
  //tr.addTest(TestAllocator< Allocator<int, 12> >::suite());
  //tr.addTest(TestAllocator< Allocator<int, 100000> >::suite());
  //tr.addTest(TestAllocator< Allocator<int, 3> >::suite());
  
  //tr.addTest(TestAllocator< allocator<double> >::suite());
  tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); 

  tr.addTest(TestAllocator< Allocator<float, 100> >::suite()); 

  tr.addTest(TestAllocator< Allocator<char, 100> >::suite()); 


  tr.run();
  
  cout << "Done." << endl;
  return 0;
}
