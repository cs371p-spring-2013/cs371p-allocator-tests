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
    CPPUNIT_TEST(test_one);
   CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct myTest : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_constructor
    // --------

    void test_constructor(){
      A x;
      int size = sizeof(x.a);
      CPPUNIT_ASSERT(x.view(x.a[0]) == size-8);
      CPPUNIT_ASSERT(x.view(x.a[size-4]) == size-8);
    }
    
    // --------
    // test_allocate1
    // --------
    void test_allocate1(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      x.allocate(3);
      
      int allocSize = 3 * sizeof(value_type);
      CPPUNIT_ASSERT(x.view(x.a[0]) == -allocSize);
      CPPUNIT_ASSERT(x.valid());
      
      
      x.allocate(4);
      
      int allocSize2 = 4 * sizeof(value_type);
      CPPUNIT_ASSERT(x.view(x.a[allocSize+8]) == -allocSize2);
      CPPUNIT_ASSERT(x.valid());
   
    }
    
    // --------
    // test_allocate2
    // --------
    
    void test_allocate2(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      int size = sizeof(x.a) * sizeof(x.a);
      
      CPPUNIT_ASSERT(x.allocate(size) == 0);
      
    }
    
    // --------
    // test_allocate3
    // --------
    
    void test_allocate3(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      int size = sizeof(x.a);
      
      int allocSize = ((size/2)-8)/sizeof(value_type);
      x.allocate(allocSize);
      
      CPPUNIT_ASSERT(x.view(x.a[0]) == -(allocSize*sizeof(value_type)));
      CPPUNIT_ASSERT(x.valid());
      
      int allocSize2 = ((size/2) - ((sizeof(value_type) + (2 * sizeof(int)))/2))/sizeof(value_type);
      
      x.allocate(allocSize2);
      
      int index = allocSize*sizeof(value_type) + 8;
      CPPUNIT_ASSERT(x.view(x.a[size - 4]) == x.view(x.a[index]));
      CPPUNIT_ASSERT(x.valid());
      
    }
    
    // --------
    // test_deallocate1
    // --------
    
    void test_deallocate1(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      int size = sizeof(x.a);
      value_type* ptr = x.allocate(1);
      
      x.deallocate(ptr);
      
      CPPUNIT_ASSERT(x.view(x.a[0]) == (size - 8));
      CPPUNIT_ASSERT(x.view(x.a[size - 4]) == (size - 8));
      CPPUNIT_ASSERT(x.valid());
      
    }
    
    // --------
    // test_deallocate2
    // --------
    
    void test_deallocate2(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      int size = sizeof(x.a);
      value_type* ptr = x.allocate(3);
      
      value_type* ptr2 = x.allocate(4);
      
      x.deallocate(ptr);
      x.deallocate(ptr2);
      
      CPPUNIT_ASSERT(x.view(x.a[0]) == (size - 8));
      CPPUNIT_ASSERT(x.view(x.a[size - 4]) == (size - 8));
      CPPUNIT_ASSERT(x.valid());
      
    }
    
    // --------
    // test_deallocate3
    // --------
    
    void test_deallocate3(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      
      int size = sizeof(x.a);
      value_type* ptr = x.allocate(2);
      value_type* ptr2 = x.allocate(2);
      value_type* ptr3 = x.allocate(2);
      
      x.deallocate(ptr);
      x.deallocate(ptr3);
      x.deallocate(ptr2);
      
      CPPUNIT_ASSERT(x.view(x.a[0]) == (size - 8));
      CPPUNIT_ASSERT(x.view(x.a[size - 4]) == (size - 8));
      CPPUNIT_ASSERT(x.valid());
      
    }
    
    // --------
    // test_valid1
    // --------
    
    void test_valid1(){
      A x;
      CPPUNIT_ASSERT(x.valid());
    }
    
    // --------
    // test_valid2
    // --------
    
    void test_valid2(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      int size = sizeof(x.a);
      int* ptr = reinterpret_cast<int*>(&x.a[0]);
    
      *ptr = size;
      CPPUNIT_ASSERT(!x.valid());
    }
    
    // --------
    // test_valid3
    // --------
    
    void test_valid3(){
      A x;
      CPPUNIT_ASSERT(x.valid());
      x.allocate(2);
      x.allocate(2);
      x.allocate(2);
      
      CPPUNIT_ASSERT(x.valid());
    }
    
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(myTest);
    CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
    tr.addTest(myTest< Allocator<int, 100> >::suite());
      
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(myTest< Allocator<double, 100> >::suite());
    
    tr.addTest(TestAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(myTest< Allocator<long, 100> >::suite());
    
    tr.run();

    cout << "Done." << endl;
    return 0;}
