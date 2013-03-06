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
// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// -----------------------------------
*/
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
#include <new>
#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

#define private public
#define protected public
#define class struc

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
        x.deallocate(b, s);

    }

    
    void test_bad_alloc () {
        // Testing proper bad alloc error catching for too large input
        A x;
        const difference_type s = 192;
        const difference_type s2 = 1;
        const value_type v = 2;
        pointer p;
        try {

            p = x.allocate(s);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
         catch (std::bad_alloc& E){
            p = x.allocate(s2);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s2);
        }
    }
    
    void test_valid_1() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // mess up one of the sentinals.
        int& sent_val = x.view(x.a[0]);
        ++sent_val;
        CPPUNIT_ASSERT(!x.valid());
        --sent_val;
        CPPUNIT_ASSERT(x.valid());
    }
	void test_valid_2() {
	    A x;
        CPPUNIT_ASSERT(x.valid());

        // manually change the sentinals, but in a valid way. Ohoho.
        // get it? Because we're testing valid()? hahaha.
        const pointer p = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p);
        CPPUNIT_ASSERT(x.valid());
    }
    
    void test_valid_3() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // changes some random values in the array, that shouldn't affect valid().
        // x.a[27] = 9;
        int& sent_val = x.view(x.a[27]);
        sent_val = 9;

        // x.a[57] = 88;
        int& sent_val2 = x.view(x.a[57]);
        sent_val2 = 99;
        CPPUNIT_ASSERT(x.valid());
    }
    
    
    void test_view_1() {
        A x;
        int b = -4;
        x.set_sentinel(x.a[0], b);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -4);
    }
     
    void test_view_2() {
        A x;

        x.a[37] = 8;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
    } 
    
    void test_view_3() {
        A x;
        const value_type v = 2;
        const pointer p = x.allocate(1);
        x.construct(p, v);
        CPPUNIT_ASSERT(x.view(x.a[0]) == (int)(-1*sizeof(value_type)));
        x.destroy(p);
        x.deallocate(p, 1);
    }
    
    void test_simple_allocate_deallocate() {
        A x;
        const difference_type s = 1;
        const pointer p = x.allocate(s);
        x.deallocate(p, s);
    }
    
    void test_dealloc_no_free() {
        A x;
        const difference_type s = 1;
        const difference_type s2 = 2;
        const difference_type s3 = 3;
        const value_type v1 = 1;
        const value_type v2 = 2;
        const value_type v3 = 3;


        const pointer p = x.allocate(s);
        const pointer p2 = x.allocate(s2);
        const pointer p3 = x.allocate(s3);
        
        x.construct(p, v1);
        CPPUNIT_ASSERT(*p == v1);
        x.construct(p2, v2);
        CPPUNIT_ASSERT(*p2 == v2);
        x.construct(p3, v3);
        CPPUNIT_ASSERT(*p3 == v3);
        
        x.destroy(p2);
        x.deallocate(p2, 2);


        x.destroy(p);
        x.deallocate(p, s);
        x.destroy(p3);
        x.deallocate(p3, s3);
        
     }
     
     void free_prev_block() { 
        A x;
        const difference_type s = 1;
        const difference_type s2 = 2;
        const difference_type s3 = 3;
        const value_type v1 = 1;
        const value_type v2 = 2;
        const value_type v3 = 3;
        
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s2);
        const pointer p3 = x.allocate(s3);

        x.construct(p1, v1);
        x.construct(p2, v2);
        x.construct(p3, v3);
        CPPUNIT_ASSERT(*p1 == v1);
        CPPUNIT_ASSERT(*p2 == v2);
        CPPUNIT_ASSERT(*p3 == v3);
        x.destroy(p1);

        x.deallocate(p1, s); // deallocate previous block.

        x.destroy(p2);
        x.deallocate(p2, s2);

        x.destroy(p3);
        x.deallocate(p3, s3);
        
    }

    void test_set_sentinel_1() {
        A x;
        x.set_sentinel(x.a[0], 123);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 123);
    }
    void test_set_sentinel_2() {
        A x;
        x.set_sentinel(x.a[96], 123);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 123);
    }
    void test_set_sentinel_3() {
        A x;
        x.set_sentinel(x.a[50], 123);
        CPPUNIT_ASSERT(x.view(x.a[50]) == 123);
    }   
    //set_sentinel
    //find_free_block
    //allocate
    //deallocate
    
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_simple_allocate_deallocate);
    CPPUNIT_TEST(test_dealloc_no_free);
    CPPUNIT_TEST(free_prev_block);
    CPPUNIT_TEST(test_bad_alloc);
    CPPUNIT_TEST(test_set_sentinel_1);
    CPPUNIT_TEST(test_set_sentinel_2);
    CPPUNIT_TEST(test_set_sentinel_3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

//    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

//    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
    tr.addTest(TestAllocator< Allocator<long long, 100> >::suite()); // uncomment!
    tr.addTest(TestAllocator< Allocator<char, 100> >::suite()); // uncomment!
   

    tr.run();

    cout << "Done." << endl;
    return 0;}
