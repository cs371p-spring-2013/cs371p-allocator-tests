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
    void test_construct_1 () {
    //not enough room for anything to fit so throw error
        try{
          Allocator<int, 2> x;
          CPPUNIT_ASSERT(false); // shouldnt ever reach this line
        }
        catch (std::bad_alloc&){}
    }
    
    void test_construct_2 () {
        try{
          Allocator<int, 100> x; // no error should be thrown
        }
        catch (std::bad_alloc&){}
    }
    
    void test_construct_3 () {
        try{
          Allocator<value_type, 12> x; //error for anything of size greater than 4 bytes
          CPPUNIT_ASSERT(sizeof(value_type) <= 4); 
        }
        catch (std::bad_alloc&){
          CPPUNIT_ASSERT(sizeof(value_type) > 4);
        }
    }
    
    void test_allocate_1 () {
       Allocator<int,100> x;
        try {
           x.allocate(10000); // bad allocate not enought room for this request
        }
        catch (std::bad_alloc&){}
    }
    
    void test_allocate_2 () {
       Allocator<int,100> x;
       int num = 92/sizeof(int);
        try {
           x.allocate(num); // allocate all the space that is allowed
        }
        catch (std::bad_alloc&){ 
            CPPUNIT_ASSERT(false);
        }
        
        try {
           x.allocate(1); // allocate one more and it will fail
           CPPUNIT_ASSERT(false);
        }
        catch (std::bad_alloc&){        
        }
    }
    
    void test_allocate_3 () {
        Allocator<int,100> x;
         x.allocate(1); // simple test case to check if it can allocate alot of stuff one after another
         x.allocate(2);
         x.allocate(3);
         x.allocate(4);
    }
    
    
    void test_deallocate_1 () { // simple allocate and deallocate
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }
    
    void test_deallocate_2 () {
        A x;
        const pointer a = x.allocate(1);
        const pointer b = x.allocate(1);
        const pointer c = x.allocate(1);
        
        // combining them back together doing a merge forward and back at the end
        x.deallocate(a, 1);
        x.deallocate(c, 1);
        x.deallocate(b, 1);
        
        // now seeing if we can fill up the entire thing to check if it merged correctly
        const difference_type s = 92/sizeof(value_type);
        try {
            const pointer d = x.allocate(s);
            x.deallocate(d, s);
        }
         catch (std::bad_alloc&){ 
            CPPUNIT_ASSERT(false);
        } 
    }
    
    
    void test_deallocate_3 () {
         Allocator<value_type,100> x;
        const pointer a = x.allocate(1);
        const pointer b = x.allocate(1);
        const pointer c = x.allocate(1);
        
        x.deallocate(b, 1); // this does not combine them together since its the middle one
        
        const difference_type s = (92/sizeof(value_type)); // this would work if and only if we had combined them but we havent 
        try {
            const pointer d = x.allocate(s);
            CPPUNIT_ASSERT(false);
            x.deallocate(d, s);
        }
         catch (std::bad_alloc&){
            x.deallocate(a,1);
            x.deallocate(c,1);
           const pointer d = x.allocate(s);
           x.deallocate(d, s);
            
        }
        

        
    }

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
    CPPUNIT_TEST(test_construct_1);
    CPPUNIT_TEST(test_construct_2);
    CPPUNIT_TEST(test_construct_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
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

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}