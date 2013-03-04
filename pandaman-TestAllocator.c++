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

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestSpecialFunctions : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_eleven
    // --------

    void test_eleven() {
        if(DEBUG) {std::cout << "_starting test 11" << std::endl;}
        A x;
        CPPUNIT_ASSERT(x.valid());
        
        int& sent_val = x.view(x.a[0]);
        sent_val = -92;
        CPPUNIT_ASSERT(!x.valid());
        sent_val = 92;
        CPPUNIT_ASSERT(x.valid());

        if(DEBUG) {std::cout << "_ending test 11" << std::endl;}
    }

    // --------
    // test_twelve
    // --------

    void test_twelve() {
        if(DEBUG) {std::cout << "_starting test 12" << std::endl;}
        A x;
        CPPUNIT_ASSERT(x.valid());

        // simple allocate should still be valid.
        const pointer p = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p);
        CPPUNIT_ASSERT(x.valid());

        if(DEBUG) {std::cout << "_ending test 12" << std::endl;}
    }

    // --------
    // test_thirteen
    // --------

    void test_thirteen() {
        if(DEBUG) {std::cout << "_starting test 13" << std::endl;}
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

        if(DEBUG) {std::cout << "_ending test 13" << std::endl;}
    }

    // --------
    // test_fourteen
    // --------

    void test_fourteen() {
        if(DEBUG) {std::cout << "_starting test 14" << std::endl;}

        A x;
        CPPUNIT_ASSERT(x.valid());
        
        // set sentinels to correct values.
        x.set_sentinels(-4, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(x.valid());

        if(DEBUG) {std::cout << "_ending test 14" << std::endl;}
    }

    // --------
    // test_fifteen
    // --------

    void test_fifteen() {
        if(DEBUG) {std::cout << "_starting test 15" << std::endl;}

        A x;
        CPPUNIT_ASSERT(x.valid());

        // set sentinels to incorrect values.
        x.set_sentinels(-8, 0);
        x.set_sentinels(80, 12);
        CPPUNIT_ASSERT(!x.valid());

        if(DEBUG) {std::cout << "_ending test 15" << std::endl;}
    }

    // --------
    // test_sixteen
    // --------

    void test_sixteen() {
        if(DEBUG) {std::cout << "_starting test 16" << std::endl;}

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

        if(DEBUG) {std::cout << "_ending test 16" << std::endl;}
    }

    // --------
    // test_seventeen
    // --------

    void test_seventeen() {
        if(DEBUG) {std::cout << "_starting test 17" << std::endl;}

        A x;
        const value_type      v = 2;
        const pointer         p = x.allocate(1);
        x.construct(p, v);
        //std::cout << (int)x.view(x.a[0]) << std::endl;
        CPPUNIT_ASSERT(x.view(x.a[0]) == (int)(-1*sizeof(value_type)));
        x.destroy(p);
        x.deallocate(p, 1);

        if(DEBUG) {std::cout << "_ending test 17" << std::endl;}
    }

    // --------
    // test_eighteen
    // --------

    void test_eighteen() {
        if(DEBUG) {std::cout << "_starting test 18" << std::endl;}

        A x;

        x.set_sentinels(-4, 0);
        x.set_sentinels(80, 12);

        CPPUNIT_ASSERT(x.view(x.a[0]) == -4);

        if(DEBUG) {std::cout << "_ending test 18" << std::endl;}
    }

    // --------
    // test_nineteen
    // --------

    void test_nineteen() {
        if(DEBUG) {std::cout << "_starting test 19" << std::endl;}

        A x;

        x.a[37] = 8;
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);

        if(DEBUG) {std::cout << "_ending test 19" << std::endl;}
    }

    CPPUNIT_TEST_SUITE(TestSpecialFunctions);

    // valid() tests.  Must comment std::allocator lines to run.
    CPPUNIT_TEST(test_eleven);
    CPPUNIT_TEST(test_twelve);
    CPPUNIT_TEST(test_thirteen);

    // // set_sentinals() tests.  Must comment std::allocator lines to run.
    CPPUNIT_TEST(test_fourteen);
    CPPUNIT_TEST(test_fifteen);
    CPPUNIT_TEST(test_sixteen);

    // // view() tests.  Must comment std::allocator lines to run.
    CPPUNIT_TEST(test_seventeen);
    CPPUNIT_TEST(test_eighteen);
    CPPUNIT_TEST(test_nineteen);
    
    CPPUNIT_TEST_SUITE_END();

};

template <typename A>
struct TestStdAllocator : CppUnit::TestFixture {
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

    CPPUNIT_TEST_SUITE(TestStdAllocator);

    // Downing test.
    CPPUNIT_TEST(test_one);

    // Downing test.
    CPPUNIT_TEST(test_ten);
    
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

        // Testing proper allocate bad_alloc exception for too small input
        A x;
        const difference_type s = 0;
        const difference_type s1 = 1;
        const value_type      v = 2;
        pointer         p;

        try {
            p = x.allocate(s);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch(std::bad_alloc& E) {
            // always goes into the catch block.  Good!
            
            p = x.allocate(s1);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s1);
        }
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
    // test_nine
    // --------
    
    void test_nine() {
        // Test a bunch of allocs and deallocs
        A x;
        const difference_type s = 4;
        pointer a, b;
        int i = 0;
        while (i != 10) {
            a = x.allocate(s);
            try {
                b = x.allocate(s);
                x.deallocate(a);
            }
            catch (std::bad_alloc& E) {
                x.deallocate(a);
                b = x.allocate(s);
            }
            x.deallocate(b);
            ++i;
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
    CPPUNIT_TEST(test_nine);

    // Downing test.
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

    // Test special functions (valid, view, set_sentinels, etc)
    // Our tests expect heap size of 100
    tr.addTest(TestSpecialFunctions< Allocator<char, 100> >::suite());
    tr.addTest(TestSpecialFunctions< Allocator<short, 100> >::suite());
    tr.addTest(TestSpecialFunctions< Allocator<int, 100> >::suite());
    tr.addTest(TestSpecialFunctions< Allocator<double, 100> >::suite());

    // Test everything else
    tr.addTest(TestStdAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 80> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<short> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 80> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<long> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<long long> >::suite());
    tr.addTest(TestAllocator< Allocator<long long, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<long long, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<char> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 80> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 1000> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
