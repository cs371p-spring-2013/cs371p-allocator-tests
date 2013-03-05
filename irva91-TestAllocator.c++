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

    //
    // constructor tests
    //
    void test_constructor_1(){
        A x;
        CPPUNIT_ASSERT(sizeof(x.a) == 100);
    }

    void test_constructor_2(){
        A x;
        CPPUNIT_ASSERT(sizeof(x.a) == 100);
        int& sentinel = x.view(x.a[0]);
        CPPUNIT_ASSERT(sentinel == 92);
    }

    void test_constructor_3(){
        A x;
        CPPUNIT_ASSERT(sizeof(x.a) == 100);
        int& sentinel = x.view(x.a[96]);
        CPPUNIT_ASSERT(sentinel == 92);
    }

    //
    // allocate tests
    //

    void test_allocate_1 (){
        A x;
        const difference_type s = 10;
        x.allocate(s);
        CPPUNIT_ASSERT(true);
    }

    void test_allocate_2 (){
        A x;
        const difference_type s = 10;
        x.allocate(s);
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        }
        catch (...){
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_3(){
        A x;
        const difference_type s = 5;
        try{
            x.allocate(s);
            x.allocate(s);
            x.allocate(s);
            const difference_type t = 2;
            x.allocate(t);
            CPPUNIT_ASSERT(sizeof(value_type) == 4);
        }
        catch(...){
            CPPUNIT_ASSERT(sizeof(value_type) == 8);
        }
    }

    void test_allocate_4(){
        A x;
        const difference_type s = 0;
        try{
            x.allocate(s);
            CPPUNIT_ASSERT(true);
        }
        catch(...){
            CPPUNIT_ASSERT(false);
        }
    }

    void test_allocate_5(){
        A x;
        const difference_type s = -1;
        try{
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        }
        catch(...){
            CPPUNIT_ASSERT(true);
        }
    }

    // construct tests
    void test_construct_1(){
        A x;
        const difference_type s = 1;
        const value_type      v = 2;

        const pointer b = x.allocate(s);
        x.construct(b, v);
        CPPUNIT_ASSERT(*b == v);
    }

    void test_construct_2(){
        A x;
        const difference_type s = 1;
        const value_type      v = 2;

        const pointer b = x.allocate(s);
        x.construct(b, v);
        CPPUNIT_ASSERT(*b == v);
        *b = *b + 1;
        CPPUNIT_ASSERT(*b == (v + 1));
    }

    void test_construct_3(){
        A x;
        const difference_type s = 5;
        const value_type      v = 2;

        const pointer p = x.allocate(s);
        const pointer e = p + s;
              pointer b = p;

        value_type num = v;
        while (b != e){
            x.construct(b, num);
            ++b;
            ++num;
        }

        b = p;
        num = v;
        while (b != e){
            CPPUNIT_ASSERT(*b == num);
            ++b;
            ++num;
        }
    }

    // test destroy
    void test_destroy_1(){
        A x;
        const difference_type s = 5;
        const value_type      v = 2;
        const pointer p = x.allocate(s);

        pointer b = p;
        pointer e = p + s;

        while (b != e){
            x.construct(b, v);
            ++b;
        }
        b = p;
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        b = p;
        while (b != e){
            x.destroy(b);
            ++b;
        }
    }

    void test_destroy_2(){
        A x;
        const difference_type s = 1;
        const value_type      v = 5;
        const pointer p = x.allocate(s);

        x.construct(p, v);
        x.destroy(p);
        CPPUNIT_ASSERT(true);
    }

    void test_destroy_3(){
        A x;
        const difference_type s = 1;
        const value_type      v = 5;
        const pointer p = x.allocate(s);

        x.construct(p, v);
        x.destroy(p);
        CPPUNIT_ASSERT(true);
        try{
            x.destroy(p + 1);
            CPPUNIT_ASSERT(true);
        }
        catch(...){
            CPPUNIT_ASSERT(false);
        }
    }

    void test_deallocate_1(){
        A x;
        const difference_type s = 1;
        const value_type      v1 = 5;
        const pointer p1 = x.allocate(s);

        x.construct(p1, v1);
        CPPUNIT_ASSERT(*p1 == v1);

        const value_type v2 = 8;
        const pointer p2 = x.allocate(s);
        x.construct(p2, v2);
        CPPUNIT_ASSERT(*p2 == v2);
        x.destroy(p2);
        x.deallocate(p2, s);
    }

    void test_deallocate_2(){
        A x;
        const difference_type s = 1;
        const value_type      v1 = 5;
        const pointer p1 = x.allocate(s);

        x.construct(p1, v1);
        CPPUNIT_ASSERT(*p1 == v1);

        const value_type v2 = 8;
        const pointer p2 = x.allocate(s);
        x.construct(p2, v2);
        CPPUNIT_ASSERT(*p2 == v2);

        x.destroy(p1);
        x.deallocate(p1, s);
    }

    void test_deallocate_3(){
        A x;
        const difference_type s = 1;
        const value_type      v = 2;

        const pointer p1 = x.allocate(s);
        x.construct(p1, v);
        const pointer p2 = x.allocate(s);
        x.construct(p2, v);
        const pointer p3 = x.allocate(s);
        x.construct(p3, v);

        x.destroy(p1);
        x.deallocate(p1, s);

        x.destroy(p3);
        x.deallocate(p3, s);

        x.destroy(p2);
        x.deallocate(p2, s);
    }

    void test_valid_1 () {
        A x;
        x.a[0] = 1;
        CPPUNIT_ASSERT (!x.valid ());
    }

    void test_valid_2 (){
        A x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_3 (){
        A x;
        const difference_type s = 10;
        x.allocate(s);
        CPPUNIT_ASSERT(x.valid());
        x.a[0] = -1;
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_view_1(){
        A x;
        char c[] = {1, 0, 0, 0};
        int& sentinel = x.view(*c);
        CPPUNIT_ASSERT(sentinel == 1);
    }

    void test_view_2(){
        A x;
        char c[] = {1, 2, 0, 0};
        int& sentinel = x.view(*c);
        CPPUNIT_ASSERT(sentinel == 513);
    }

    void test_view_3(){
        A x;
        char c[] = {0, 2, 0, 0};
        int& sentinel = x.view(*c);
        CPPUNIT_ASSERT(sentinel == 512);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);

    // Downing tests
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);

    // test constructors
    // only run for our implementation of Allocator
    // CPPUNIT_TEST(test_constructor_1);
    // CPPUNIT_TEST(test_constructor_2);
    // CPPUNIT_TEST(test_constructor_3);

    // test allocate
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);

    //test construct
    CPPUNIT_TEST(test_construct_1);
    CPPUNIT_TEST(test_construct_2);
    CPPUNIT_TEST(test_construct_3);

    //test destroy
    CPPUNIT_TEST(test_destroy_1);
    CPPUNIT_TEST(test_destroy_2);
    CPPUNIT_TEST(test_destroy_3);

    // test deallocate
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);

    // test valid
    // only run for our implementation of Allocator
    // CPPUNIT_TEST(test_valid_1);
    // CPPUNIT_TEST(test_valid_2);
    // CPPUNIT_TEST(test_valid_3);

    // test view
    // only run for our implementation of Allocator
    // CPPUNIT_TEST(test_view_1);
    // CPPUNIT_TEST(test_view_2);
    // CPPUNIT_TEST(test_view_3);

    // test constructor

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