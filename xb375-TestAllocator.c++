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

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

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

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};


struct TestCustomAllocator : CppUnit::TestFixture{
//-----
//valid
//-----

  void test_valid1(){
    Allocator<double, 80> x;
    CPPUNIT_ASSERT(x.isValid());
  }
  void test_valid2(){
    Allocator<int, 100> x;
    x.allocate(8);
    int* p2=x.allocate(2);
    x.deallocate(p2);
    CPPUNIT_ASSERT(x.isValid());
  }
  void test_valid3(){
    Allocator<char,100> x;
    x.allocate(92);
    CPPUNIT_ASSERT(x.isValid());
  }

//--------
//allocate
//--------

  void test_alloc1(){
    Allocator<char, 100> x;
    try{
      x.allocate(-5);
      CPPUNIT_ASSERT(false);
    }
    catch(std::bad_alloc&){
      CPPUNIT_ASSERT(true);
    }
  }
  void test_alloc2(){
    Allocator<int, 100> x;
    try{
      x.allocate(93);
      CPPUNIT_ASSERT(false);
    }
    catch(std::bad_alloc&){
     CPPUNIT_ASSERT(true);
    }
  }
  void test_alloc3(){
    Allocator<double, 100> x;
    try{
      x.allocate(12);
      CPPUNIT_ASSERT(false);
    }
    catch(std::bad_alloc&){
     CPPUNIT_ASSERT(true);
    }
  }

//----------
//deallocate
//----------

  void test_dealloc1(){
    Allocator<int,100> x;
    int* p1=x.allocate(23);
    x.deallocate(p1);
    int* p2=x.allocate(11);
    
    CPPUNIT_ASSERT(p1==p2);
  }
  void test_dealloc2(){
    Allocator<int,100>x;
    int* p1=x.allocate(23);
    x.deallocate(p1);
    CPPUNIT_ASSERT(*reinterpret_cast<int*>(--p1)==92);
  }
  void test_dealloc3(){
   Allocator<int,100>x;
    int* p1=x.allocate(4);
    x.deallocate(p1);
    CPPUNIT_ASSERT(x.isValid());
  }

  //-----
  //suite
  //-----

  CPPUNIT_TEST_SUITE(TestCustomAllocator);
  CPPUNIT_TEST(test_valid1);
  CPPUNIT_TEST(test_valid2);
  CPPUNIT_TEST(test_valid3);
  CPPUNIT_TEST(test_alloc1);
  CPPUNIT_TEST(test_alloc2);
  CPPUNIT_TEST(test_alloc3);
  CPPUNIT_TEST(test_dealloc1);
  CPPUNIT_TEST(test_dealloc2);
  CPPUNIT_TEST(test_dealloc3);
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
    tr.addTest(TestCustomAllocator::suite());
    tr.run();

    cout << "Done." << endl;
    return 0;}
