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
    % g++ -ldl -pedantic -std=c++0x -Wall Allocator.h TestAllocator.c++ -lcppunit -o TestAllocator
    % valgrind ./TestAllocator >& TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator
#include <cstring>   //strcmp

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

// --------------
// TestAllocator2
// --------------

struct TestAllocator2 : CppUnit::TestFixture {

    // ----------------
    // test_constructor
    // ----------------

    void test_constructor1 () { // allocator too small: no room for element
        Allocator<double,12> x;
        CPPUNIT_ASSERT(x.numSentinelPairs == 0);
    }

    void test_constructor2 () { // allocator too small: no room for sentinel + element
        Allocator<short,2> x;
        CPPUNIT_ASSERT(x.numSentinelPairs == 0);
    }

    void test_constructor3 () {
        Allocator<char,10> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 2);
        CPPUNIT_ASSERT(x.view(x.a[6]) == 2);
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    void test_constructor4 () {
        Allocator<int,12> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 4);
        CPPUNIT_ASSERT(x.view(x.a[8]) == 4);
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    void test_constructor5 () {
        Allocator<float,100> x;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }


    // ----------
    // test_valid
    // ----------

    void test_valid1 () { // hanging byte at end of array
        Allocator<char,10> x;
        x.view(x.a[0]) = 1;
        x.view(x.a[5]) = 1;
        x.numSentinelPairs = 1;
        bool b = x.valid();
        CPPUNIT_ASSERT(b == false);
    }

    void test_valid2 () { // non-matching sentinel pair
        Allocator<short,100> x;;
        x.view(x.a[0]) = -40;
        x.view(x.a[44]) = -40;
        x.view(x.a[48]) = 44;
        x.view(x.a[96]) = 32;
        x.numSentinelPairs = 2;
        bool b = x.valid();
        CPPUNIT_ASSERT(b == false);
    }

    void test_valid3 () {
        Allocator<float, 100> x;
        x.view(x.a[0]) = -40;
        x.view(x.a[44]) = -40;
        x.view(x.a[48]) = -20;
        x.view(x.a[72]) = -20;
        x.view(x.a[76]) = 4;
        x.view(x.a[84]) = 4;
        x.view(x.a[88]) = -4;
        x.view(x.a[96]) = -4;
        x.numSentinelPairs = 4;
        bool b = x.valid();
        CPPUNIT_ASSERT(b == true);
    }
    

    // -------------
    // test_allocate
    // -------------

    void test_allocate1 () { // allocator too small
        Allocator<char,10> x;
        try {
            x.allocate(3);
        } catch (std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    void test_allocate2 () { // allocator too small
        Allocator<double,8> x;
        try {
            x.allocate(1);
        } catch (std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
        CPPUNIT_ASSERT(x.numSentinelPairs == 0);
    }

    void test_allocate3 () { // multiple allocs; each alloc splits off smaller free block. no room for last alloc
        Allocator<int,100> x;
        try {
            int* p1 = x.allocate(10);
            int* p2 = x.allocate(5);
            int* p3 = x.allocate(1);
            CPPUNIT_ASSERT(p1 == reinterpret_cast<int*>(x.a + 4));
            CPPUNIT_ASSERT(x.view(x.a[0]) == -40);
            CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
            CPPUNIT_ASSERT(p2 == reinterpret_cast<int*>(x.a + 52));
            CPPUNIT_ASSERT(x.view(x.a[48]) == -20);
            CPPUNIT_ASSERT(x.view(x.a[72]) == -20);
            CPPUNIT_ASSERT(p3 == reinterpret_cast<int*>(x.a + 80));
            CPPUNIT_ASSERT(x.view(x.a[76]) == -4);
            CPPUNIT_ASSERT(x.view(x.a[84]) == -4);
            CPPUNIT_ASSERT(x.view(x.a[88]) == 4);
            CPPUNIT_ASSERT(x.view(x.a[96]) == 4);
            x.allocate(5);
        } catch (std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
        CPPUNIT_ASSERT(x.numSentinelPairs == 4);
    }

    void test_allocate4 () { // requested block is equal to available space
        Allocator<float,100> x;
        float* p = x.allocate(23);
        CPPUNIT_ASSERT(p == reinterpret_cast<float*>(x.a + 4));
        CPPUNIT_ASSERT(x.view(x.a[0]) == -92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == -92); 
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    void test_allocate5 () { // give whole block to p2 since not enough space for new free block
        Allocator<double,100> x;
        double* p1 = x.allocate(5);
        double* p2 = x.allocate(4);
        CPPUNIT_ASSERT(p1 == reinterpret_cast<double*>(x.a + 4));
        CPPUNIT_ASSERT(x.view(x.a[0]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
        CPPUNIT_ASSERT(p2 == reinterpret_cast<double*>(x.a + 52));
        CPPUNIT_ASSERT(x.view(x.a[48]) == -44);
        CPPUNIT_ASSERT(x.view(x.a[96]) == -44);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);    
    }

    void test_allocate6 () { // give whole block to p2 since not enough space for new free block
        Allocator<double,100> x;
        double* p1 = x.allocate(5);
        double* p2 = x.allocate(5);
        CPPUNIT_ASSERT(p1 == reinterpret_cast<double*>(x.a + 4));
        CPPUNIT_ASSERT(x.view(x.a[0]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
        CPPUNIT_ASSERT(p2 == reinterpret_cast<double*>(x.a + 52));
        CPPUNIT_ASSERT(x.view(x.a[48]) == -44);
        CPPUNIT_ASSERT(x.view(x.a[96]) == -44); 
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);    
    }

    // -------------------
    // test_split_up_block
    // -------------------

    void test_split_up_block1() {
        Allocator<int,100> x;
        x.split_up_block(0, 96, 40, 44);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[48]) == 44);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 44);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
    }

    void test_split_up_block2() {
        Allocator<int,100> x;
        x.split_up_block(0, 96, 40, 44);
        x.split_up_block(48, 96, 20, 16);
        CPPUNIT_ASSERT(x.view(x.a[48]) == -20);
        CPPUNIT_ASSERT(x.view(x.a[72]) == -20);
        CPPUNIT_ASSERT(x.view(x.a[76]) == 16);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 16);   
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
    }


    void test_split_up_block3() {
        Allocator<int,100> x;
        x.split_up_block(0, 96, 40, 44);
        x.split_up_block(48, 96, 20, 16);
        x.split_up_block(76, 96, 4, 4);
        CPPUNIT_ASSERT(x.view(x.a[76]) == -4);
        CPPUNIT_ASSERT(x.view(x.a[84]) == -4);
        CPPUNIT_ASSERT(x.view(x.a[88]) == 4);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 4);
        CPPUNIT_ASSERT(x.numSentinelPairs == 4);
    }
    // --------------
    // test_deallocate
    // ---------------

    void test_deallocate1 () { // no merging with adjacent blocks
        Allocator<char,20> x;
        x.allocate(2);
        char* p2 = x.allocate(1);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
        x.deallocate(p2);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -2);
        CPPUNIT_ASSERT(x.view(x.a[6]) == -2);
        CPPUNIT_ASSERT(x.view(x.a[10]) == 2);
        CPPUNIT_ASSERT(x.view(x.a[16]) == 2);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
    }

    void test_deallocate2 () { // right adjacent block is free
        Allocator<int,100> x;
        x.allocate(10);
        int* p2 = x.allocate(8);
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
        x.deallocate(p2);
        CPPUNIT_ASSERT(x.view(x.a[0]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
        CPPUNIT_ASSERT(x.view(x.a[48]) == 44);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 44);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
    }

    void test_deallocate3 () { // deallocate everything, return to original allocator state
        Allocator<double,100> x;
        double* p1 = x.allocate(2);
        double* p2 = x.allocate(8);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
        x.deallocate(p1);
        x.deallocate(p2);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    void test_deallocate4 () { // left adjacent block is free
        Allocator<char,100> x;
        char* p1 = x.allocate(40);
        char* p2 = x.allocate(32);
        x.allocate(4);
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
        x.deallocate(p2);
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
        x.deallocate(p1);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 80);
        CPPUNIT_ASSERT(x.view(x.a[84]) == 80);
        CPPUNIT_ASSERT(x.view(x.a[88]) == -4);
        CPPUNIT_ASSERT(x.view(x.a[96]) == -4);
        CPPUNIT_ASSERT(x.numSentinelPairs == 2);
    }

    void test_deallocate5 () { // adjacent blocks on both sides are free
        Allocator<short,100> x;
        short* p1 = x.allocate(20);
        short* p2 = x.allocate(16);
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
        x.deallocate(p1);
        CPPUNIT_ASSERT(x.numSentinelPairs == 3);
        x.deallocate(p2);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
        CPPUNIT_ASSERT(x.numSentinelPairs == 1);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator2);
    CPPUNIT_TEST(test_constructor1);
    CPPUNIT_TEST(test_constructor2);
    CPPUNIT_TEST(test_constructor3);
    CPPUNIT_TEST(test_constructor4);
    CPPUNIT_TEST(test_constructor5);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_allocate4);
    CPPUNIT_TEST(test_allocate5);
    CPPUNIT_TEST(test_allocate6);
    CPPUNIT_TEST(test_split_up_block1);
    CPPUNIT_TEST(test_split_up_block2);
    CPPUNIT_TEST(test_split_up_block3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST(test_deallocate4);
    CPPUNIT_TEST(test_deallocate5);
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

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); 

    tr.addTest(TestAllocator2::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
