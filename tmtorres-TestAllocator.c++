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
    
    // ----------
    // test_valid
    // ----------

    void test_valid_1 () {
        const difference_type s = sizeof(int) * 10;
        Allocator<int, s> x;
        int* b = reinterpret_cast<int*>(x.a);
        b[0] = 0;
        CPPUNIT_ASSERT(!x.valid());
    }
    
    void test_valid_2 () {
        const difference_type s = sizeof(int) * 10;
        Allocator<int, s> x;
        int* b = reinterpret_cast<int*>(x.a);
        b[0] *= -1;
        CPPUNIT_ASSERT(!x.valid());
        b[9] = b[0];
        CPPUNIT_ASSERT(x.valid());
    }
    
    void test_valid_3 () {
        const difference_type s = sizeof(int) * 10;
        Allocator<int, s> x;
        int* b = reinterpret_cast<int*>(x.a);
        b[0] = (int)(-1 * sizeof(int));
        b[2] = 0;
        CPPUNIT_ASSERT(!x.valid());
        b[2] = b[0];
        b[3] = 0;
        CPPUNIT_ASSERT(!x.valid());
        b[3] = (int)(5 * sizeof(int));
        b[9] = b[3];
        CPPUNIT_ASSERT(x.valid());
    }
    
    // ----------------
    // test_constructor
    // ----------------
    
    void test_constructor_1 () {
        const difference_type s = 100;
        Allocator<value_type, s> x;
        const int* b = reinterpret_cast<int*>(x.a);
        const int* e = reinterpret_cast<int*>(x.a + s - sizeof(int));
        CPPUNIT_ASSERT(*b == s - 2 * sizeof(int));
        CPPUNIT_ASSERT(*b == *e);
    }
    
    void test_constructor_2 () {
        const difference_type s = 2;
        Allocator<value_type, s> x;
        try {
            x.allocate(1);
            CPPUNIT_ASSERT(false);
        }
        catch (std::bad_alloc&) {}
    }
    
    void test_constructor_3 () {
        const difference_type s = 100;
        Allocator<value_type, s> x;
        Allocator<value_type, s> y;
        const int* b1 = reinterpret_cast<int*>(x.a);
        const int* b2 = reinterpret_cast<int*>(y.a);
        CPPUNIT_ASSERT(b1 != b2);
        CPPUNIT_ASSERT(*b1 == *b2);
    }
    
    // -------------
    // test_allocate
    // -------------
    
    void test_allocate_1 () {
        A x;
        const difference_type s = 3;
        const pointer         p = x.allocate(s);
        for(int i = 0; i < s; ++i) {
            p[i] = i;
        }
        for(int i = 0; i < s; ++i) {
            CPPUNIT_ASSERT(p[i] == i);
        }
        x.deallocate(p, s);
        
    }
    
    void test_allocate_2 () {
        A x;
        const difference_type s = 21;
        try {
            x.deallocate(x.allocate(s), s);
        }
        catch(std::bad_alloc&){
            pointer q = x.allocate(1);
            CPPUNIT_ASSERT(q);
            x.deallocate(q, 1);
        }
    }
    
    void test_allocate_3 () {
        A x;
        const difference_type s = (100 - 2 * sizeof(int)) / sizeof(value_type);
        const pointer         p = x.allocate(s);
        for(int i = 0; i < s; ++i) {
            p[i] = i * i;
        }
        for(int i = 0; i < s; ++i) {
            CPPUNIT_ASSERT(p[i] == i * i);
        }
        x.deallocate(p, s);
        
    }
    
    // ---------------
    // test_deallocate
    // ---------------
    
    void test_deallocate_1 () {
        A x;
        const difference_type s = (100 - 2 * sizeof(int)) / sizeof(value_type);
        x.deallocate(x.allocate(s), s);
        try {
            x.deallocate(x.allocate(s), s);
        }
        catch(std::bad_alloc&){
            CPPUNIT_ASSERT(false);
        }
    }
    
    void test_deallocate_2 () {
        A x;
        const difference_type s = (100 - 2 * sizeof(int)) / sizeof(value_type);
        for(int i = 1; i < s; ++i) {
            try {
                x.deallocate(x.allocate(i), i);
            }
            catch(std::bad_alloc&){
                CPPUNIT_ASSERT(false);
            }
        }
    }
    
    void test_deallocate_3 () {
        /* back coalesce */
        {
            A x;
            const difference_type s = 3;
            const pointer         p = x.allocate(s);
            const pointer         q = x.allocate(s);
            const pointer         r = x.allocate(s);
            x.deallocate(p, s);
            x.deallocate(q, s);
            x.deallocate(r, s);
            try {
                x.deallocate(x.allocate(3 * s), 3 * s);
            }
            catch (std::bad_alloc&){
                CPPUNIT_ASSERT(false);
            }
        }
        /* front coalesce */
        {
            A x;
            const difference_type s = 3;
            const pointer         p = x.allocate(s);
            const pointer         q = x.allocate(s);
            const pointer         r = x.allocate(s);
            x.deallocate(r, s);
            x.deallocate(q, s);
            x.deallocate(p, s);
            try {
                x.deallocate(x.allocate(3 * s), 3 * s);
            }
            catch (std::bad_alloc&){
                CPPUNIT_ASSERT(false);
            }
        }
        /* middle coalesce */
        {
            A x;
            const difference_type s = 3;
            const pointer         p = x.allocate(s);
            const pointer         q = x.allocate(s);
            const pointer         r = x.allocate(s);
            x.deallocate(p, s);
            x.deallocate(r, s);
            x.deallocate(q, s);
            try {
                x.deallocate(x.allocate(3 * s), 3 * s);
            }
            catch (std::bad_alloc&){
                CPPUNIT_ASSERT(false);
            }
        }
    }
    
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
    
    // -----
    // suite
    // -----
    
    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_one);
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