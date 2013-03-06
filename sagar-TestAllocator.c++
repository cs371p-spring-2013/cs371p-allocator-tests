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
#include <cstring>   // strcmp`

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

// ------
// macros
// ------
#define private   public
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
        
    // ---------
    // test_zero
    // ---------

    void test_zero () {
        A x;
        const difference_type s = 0;
        const pointer         p = x.allocate(s);
        x.deallocate(p, s);}
    
    // --------
    // test_two
    // --------

    void test_two () {
        try {
            A x;
            const difference_type s = 10;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        } catch (bad_alloc& ba) {
            CPPUNIT_ASSERT(strcmp(ba.what(), "std::bad_alloc") == 0);
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
        x.deallocate(b, s);}
        
    // -----------
    // constructor
    // -----------
    void test_constructor_1 () {
        Allocator<int, 100> x;
        int start = (int)x.a[0];
        int end = (int)x.a[96];
        CPPUNIT_ASSERT(start == 92);
        CPPUNIT_ASSERT(end == 92);
        CPPUNIT_ASSERT(sizeof(x.a) == 100);
    }
    
    void test_constructor_2 () {
        Allocator<double, 50> x;
        int start = (int)x.a[0];
        int end = (int)x.a[46];
        CPPUNIT_ASSERT(start == 42);
        CPPUNIT_ASSERT(end == 42);
        CPPUNIT_ASSERT(sizeof(x.a) == 50);
    }
    
    void test_constructor_3 () {
        Allocator<float, 33> x;
        int start = (int)x.a[0];
        int end = (int)x.a[29];
        CPPUNIT_ASSERT(start == 25);
        CPPUNIT_ASSERT(end == 25);
        CPPUNIT_ASSERT(sizeof(x.a) == 33);
    }

    // --------
    // allocate
    // --------
    void test_allocate_1 () {
        Allocator<int, 100> x;
        const difference_type s = 10;
        x.allocate(s);
        CPPUNIT_ASSERT((int)x.a[0] == -40);
        CPPUNIT_ASSERT((int)x.a[44] == -40);
        CPPUNIT_ASSERT((int)x.a[48] == 44);
        CPPUNIT_ASSERT((int)x.a[96] == 44);
    }
    
    void test_allocate_2 () {
        Allocator<int, 100> x;
        const difference_type s = 5;
        x.allocate(s);
        x.allocate(s);
        CPPUNIT_ASSERT((int)x.a[0] == -20);
        CPPUNIT_ASSERT((int)x.a[24] == -20);
        CPPUNIT_ASSERT((int)x.a[28] == -20);
        CPPUNIT_ASSERT((int)x.a[52] == -20);
        CPPUNIT_ASSERT((int)x.a[56] == 36);
        CPPUNIT_ASSERT((int)x.a[96] == 36);
    }
    
    void test_allocate_3 () {
        try{
            Allocator<int, 100> x;
            const difference_type s = 100;
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (bad_alloc& ba) {
            CPPUNIT_ASSERT(strcmp(ba.what(), "std::bad_alloc") == 0);
        }
    }
    
    
    void test_allocate_4 () {
        Allocator<int, 100> x;
        const difference_type s = 21;
        x.allocate(s);
        CPPUNIT_ASSERT((int)x.a[0] == -92);
        CPPUNIT_ASSERT((int)x.a[96] == -92);
    }
    
    
    
    void test_allocate_5 () {
        try{
            Allocator<int, 100> x;
            x.allocate(20);
            const difference_type s = 2;
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (bad_alloc& ba) {
            CPPUNIT_ASSERT(strcmp(ba.what(), "std::bad_alloc") == 0);
        }
    }
    
    void test_allocate_6 () {
        try {
            Allocator<int, 4> x;
            const difference_type s = 1;
            x.allocate(s);
        } catch (bad_alloc& ba) {
            CPPUNIT_ASSERT(strcmp(ba.what(), "std::bad_alloc") == 0);
        }
    }
    
    // ----------
    // deallocate
    // ----------
    void test_deallocate_1 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = 64;
        x.view(x.a[96]) = 64;
        x.deallocate((int*) &x.a[4], 0);
        CPPUNIT_ASSERT(x.a[0] = 92);
        CPPUNIT_ASSERT(x.a[96] = 92);
    }
    
    void test_deallocate_2 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = 20;
        x.view(x.a[24]) = 20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = -20;
        x.view(x.a[56]) = 36;
        x.view(x.a[96]) = 36;
        x.deallocate((int*) &x.a[32], 0);
        CPPUNIT_ASSERT(x.a[0] = 92);
        CPPUNIT_ASSERT(x.a[96] = 92);
    }
    
    void test_deallocate_3 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = -20;
        x.view(x.a[56]) = -36;
        x.view(x.a[96]) = -36;
        x.deallocate((int*) &x.a[32], 0);
        CPPUNIT_ASSERT(x.a[0] = -20);
        CPPUNIT_ASSERT(x.a[24] = -20);
        CPPUNIT_ASSERT(x.a[28] = 20);
        CPPUNIT_ASSERT(x.a[52] = 20);
        CPPUNIT_ASSERT(x.a[56] = -36);
        CPPUNIT_ASSERT(x.a[96] = -36);
    }
    
    void test_deallocate_4 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = -20;
        x.view(x.a[56]) = 36;
        x.view(x.a[96]) = 36;
        x.deallocate((int*) &x.a[32], 0);
        CPPUNIT_ASSERT(x.a[0] = -20);
        CPPUNIT_ASSERT(x.a[24] = -20);
        CPPUNIT_ASSERT(x.a[28] = 64);
        CPPUNIT_ASSERT(x.a[96] = 64);
    }
    
    void test_deallocate_5 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = 0;
        x.view(x.a[32]) = 0;
        x.view(x.a[36]) = 56;
        x.view(x.a[96]) = 56;
        x.deallocate((int*) &x.a[32], 0);
        CPPUNIT_ASSERT(x.a[0] = -20);
        CPPUNIT_ASSERT(x.a[24] = -20);
        CPPUNIT_ASSERT(x.a[28] = 64);
        CPPUNIT_ASSERT(x.a[96] = 64);
    }
    
    void test_deallocate_6 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = 20;
        x.view(x.a[24]) = 20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = -20;
        x.view(x.a[56]) = -36;
        x.view(x.a[96]) = -36;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[32], 0);
        CPPUNIT_ASSERT(x.a[0] = 48);
        CPPUNIT_ASSERT(x.a[52] = 48);
        CPPUNIT_ASSERT(x.a[56] = -36);
        CPPUNIT_ASSERT(x.a[96] = -36);
    }
    
    void test_deallocate_7 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -92;
        x.view(x.a[96]) = -92;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[4], 0);
        CPPUNIT_ASSERT(x.a[0] = 92);
        CPPUNIT_ASSERT(x.a[96] = 92);
    }
    
    // -----
    // valid
    // -----
    void test_valid_1 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = -20;
        x.view(x.a[56]) = 36;
        x.view(x.a[96]) = 36;
        CPPUNIT_ASSERT(x.valid());
    }
    
    void test_valid_2 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = -20;
        x.view(x.a[52]) = 10;
        x.view(x.a[56]) = 36;
        x.view(x.a[96]) = 36;
        CPPUNIT_ASSERT(!x.valid());
    }
    
    void test_valid_3 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = 0;
        x.view(x.a[32]) = 0;
        x.view(x.a[36]) = 56;
        x.view(x.a[96]) = 56;
        CPPUNIT_ASSERT(x.valid());
    }
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_zero);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);
    CPPUNIT_TEST(test_allocate_6);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST(test_deallocate_6);
    CPPUNIT_TEST(test_deallocate_7);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
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
