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

#define protected public
#define private public
#define class struct

#include "Allocator.h"

/* NOTE!!!! For tests 11+ you must have the std::allocator tests commented out, as the std::allocator doesn't have a valid() method, etc */

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
    // test_two
    // --------
    
    void test_two () {
        // Testing default constructor, and simple allocate and deallocate
        A x;
        const difference_type s = 1;
        const pointer         p = x.allocate(s);
        x.deallocate(p, s);
    }

    // ----------
    // test_three
    // ----------
    
    void test_three () {
        // Testing proper bad alloc error catching for too large input
        A x;
        const difference_type s = 192;
        const difference_type s2 = 1;
        const value_type      v = 2;
        pointer         p;
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

    // ---------
    // test_four
    // ---------
    
    void test_four () {
        // To be fleshed out with test to make sure deallocate catches
        // when a passed pointer is invalid.
    }

    // ---------
    // test_five
    // ---------
    
    void test_five () {
        // Testing case 1 of deallocating (neither adjacent block is free).
        A x;
        const difference_type  s  = 1;
        const difference_type  s2 = 2;
        const difference_type  s3 = 3;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;


        const pointer          p = x.allocate(s);
        const pointer          p2 = x.allocate(s2);
        const pointer          p3 = x.allocate(s3);
        
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

    // ---------
    // test_six
    // ---------
    
    void test_six () {
        // Testing case 2 of deallocating (previous block is free, combine).
        A x;
        const difference_type  s  = 1;
        const difference_type  s2 = 2;
        const difference_type  s3 = 3;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;
        
        const pointer          p1 = x.allocate(s);
        const pointer          p2 = x.allocate(s2);
        const pointer          p3 = x.allocate(s3);

        x.construct(p1, v1);
        x.construct(p2, v2);
        x.construct(p3, v3);
        CPPUNIT_ASSERT(*p1 == v1);
        CPPUNIT_ASSERT(*p2 == v2);
        CPPUNIT_ASSERT(*p3 == v3);
        x.destroy(p1);

        x.deallocate(p1, s);    // deallocate previous block.

        x.destroy(p2);
        x.deallocate(p2, s2);

        x.destroy(p3);
        x.deallocate(p3, s3);
    }

    // ---------
    // test_seven
    // ---------
    
    void test_seven () {
        // Testing case 3 of deallocating (next block is free, combine).
        A x;
        const difference_type  s  = 1;
        const difference_type  s2 = 2;
        const difference_type  s3 = 3;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;
        
        const pointer          p = x.allocate(s);
        const pointer          p2 = x.allocate(s2);
        const pointer          p3 = x.allocate(s3);

        x.construct(p, v1);
        x.construct(p2, v2);
        x.construct(p3, v3);
        CPPUNIT_ASSERT(*p == v1);
        CPPUNIT_ASSERT(*p2 == v2);
        CPPUNIT_ASSERT(*p3 == v3);

        x.destroy(p3);
        x.deallocate(p3, s3);   // deallocate next block.
        
        x.destroy(p2);
        x.deallocate(p2, s2);

        x.destroy(p);
        x.deallocate(p, s);
    }

    // ---------
    // test_eight
    // ---------
    
    void test_eight () {
        // Testing case 4 of deallocating (both are free, combine all three).
        A x;
        const difference_type  s  = 1;
        const difference_type  s2 = 2;
        const difference_type  s3 = 3;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;
        
        const pointer          p = x.allocate(s);
        const pointer          p2 = x.allocate(s2);
        const pointer          p3 = x.allocate(s3);

        x.construct(p, v1);
        x.construct(p2, v2);
        x.construct(p3, v3);
        CPPUNIT_ASSERT(*p == v1);
        CPPUNIT_ASSERT(*p2 == v2);
        CPPUNIT_ASSERT(*p3 == v3);
        x.destroy(p);
        x.destroy(p3);

        x.deallocate(p, s);    // deallocate both previous
        x.deallocate(p3, s3);   // and next blocks.
        
        x.destroy(p2);
        try{
            x.deallocate(p2, 2);
        }
        catch(std::bad_alloc& E) {
            x.deallocate(p2, s2);
        }
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

    // --------
    // test_eleven
    // --------

    void test_eleven() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // mess up one of the sentinals.
        int& sent_val = x.view(x.a[0]);
        ++sent_val;
        CPPUNIT_ASSERT(!x.valid());
        --sent_val;
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_twelve
    // --------

    void test_twelve() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // manually change the sentinals, but in a valid way.  Ohoho.
        // get it?  Because we're testing valid()?  hahaha.
        const pointer p = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p);
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_thirteen
    // --------

    void test_thirteen() {
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

    // --------
    // test_fourteen
    // --------

    void test_fourteen() {
        A x;
        CPPUNIT_ASSERT(x.valid());
        
        // set sentinels to correct values.
        x.set_sentinels(-4, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_fifteen
    // --------

    void test_fifteen() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // set sentinels to incorrect values.
        x.set_sentinels(-8, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(!x.valid());
    }

    // --------
    // test_sixteen
    // --------

    void test_sixteen() {
        A x;
        CPPUNIT_ASSERT(x.valid());

        // set sentinels to incorrect values.
        x.set_sentinels(-8, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(!x.valid());

        // set sentinels to correct values.
        x.set_sentinels(-4, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_seventeen
    // --------

    void test_seventeen() {
        A x;
        const value_type      v = 2;
        const pointer         p = x.allocate(1);
        x.construct(p, v);
        //std::cout << (int)x.view(x.a[0]) << std::endl;
        CPPUNIT_ASSERT(x.view(x.a[0]) == (int)(-1*sizeof(value_type)));
        x.destroy(p);
        x.deallocate(p, 1);
    }

    // --------
    // test_eighteen
    // --------

    void test_eighteen() {
        A x;

        x.set_sentinels(-4, 0);
        x.set_sentinels(80, 12);

        CPPUNIT_ASSERT(x.view(x.a[0]) == -4);
    }

    // --------
    // test_nineteen
    // --------

    void test_nineteen() {
        A x;

        x.a[37] = 8;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);

    // Downing test.
    CPPUNIT_TEST(test_one);

    // tests basic functionality and bad_alloc cases.
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_four);

    // tests for the 4 cases of deallocating.
    CPPUNIT_TEST(test_five);
    CPPUNIT_TEST(test_six);
    CPPUNIT_TEST(test_seven);
    CPPUNIT_TEST(test_eight);

    // Downing test.
    CPPUNIT_TEST(test_ten);

    // valid() tests.  Must comment std::allocator lines to run.
    // CPPUNIT_TEST(test_eleven);
    // CPPUNIT_TEST(test_twelve);
    // CPPUNIT_TEST(test_thirteen);

    // // set_sentinals() tests.  Must comment std::allocator lines to run.
    // CPPUNIT_TEST(test_fourteen);
    // CPPUNIT_TEST(test_fifteen);
    // CPPUNIT_TEST(test_sixteen);

    // // view() tests.  Must comment std::allocator lines to run.
    // CPPUNIT_TEST(test_seventeen);
    // CPPUNIT_TEST(test_eighteen);
    // CPPUNIT_TEST(test_nineteen);
    
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

    tr.addTest(TestAllocator< std::allocator<short> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
