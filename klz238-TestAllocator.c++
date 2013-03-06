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
    CPPUNIT_TEST_SUITE_END();
};

template <typename A>
struct MyTestAllocator : CppUnit::TestFixture {

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

#define RC_CP(x) reinterpret_cast<char*>(x)
#define RC_IP(x) reinterpret_cast<int*>(x)

    void test_construct_1 (){
        A x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_allocate_1 (){
        A x;
        pointer y;
        try {
            y = x.allocate(2);
            CPPUNIT_ASSERT(RC_CP(y) == &x.a[sizeof(int)]);
            x.deallocate(y);
        } catch (std::bad_alloc& ba) {

        }
    }

    void test_allocate_2 (){
        A x;
        pointer y = 0;
        try {
            y = x.allocate(9876543L);
            CPPUNIT_ASSERT(false);
        } catch (std::bad_alloc& ba) {
            //CPPUNIT_ASSERT()
        }

        CPPUNIT_ASSERT(y == 0);

        //x.deallocate(y);
    }

    void test_allocate_3 (){
        A x;
        pointer y = 0;
        pointer z = 0;
        try {
            int v = *reinterpret_cast<int*>(&x.a[0]) / sizeof(value_type);
            z = x.allocate((v != 0) ? v : 1);
            CPPUNIT_ASSERT(RC_CP(z) == &x.a[sizeof(int)]);
        } catch (std::bad_alloc& ba) {

        }

        try {
            y = x.allocate(1);
            CPPUNIT_ASSERT(false);
        } catch (std::bad_alloc& ba) {

        }

        CPPUNIT_ASSERT(y == 0);

        if (z != 0) {
            x.deallocate(z);
        }
    }

    void test_valid_1 (){
        A x;
        if (x.max_bytes >= 8) {
            *RC_IP(&x.a[0]) = x.max_bytes - sizeof(int) * 2;
            *RC_IP(&x.a[x.max_bytes- sizeof(int)]) = *RC_IP(&x.a[0]);

            CPPUNIT_ASSERT(x.valid());

            *RC_IP(&x.a[0]) = -*RC_IP(&x.a[0]);
            *RC_IP(&x.a[x.max_bytes- sizeof(int)]) = -*RC_IP(&x.a[0]);

            if (*RC_IP(&x.a[0]) != 0) {
                CPPUNIT_ASSERT(x.valid() == false);
            }
        }
    }

    void test_valid_2 (){
        A x;
        if (x.max_bytes >= 8) {
            *RC_IP(&x.a[0]) = x.max_bytes - sizeof(int) * 2;
            *RC_IP(&x.a[x.max_bytes- sizeof(int)]) = -x.a[0];

            if (*RC_IP(&x.a[0]) != 0) {
                CPPUNIT_ASSERT(x.valid() == false);
            }
        }
    }

    void test_valid_3 (){
        A x;
        pointer y = 0;

        try {
            y = x.allocate(1);

            CPPUNIT_ASSERT(x.valid());
            x.deallocate(y);
        } catch (std::bad_alloc& ba) {

        }

        CPPUNIT_ASSERT(x.valid());
    }

    void test_deallocate_1 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }

        x.deallocate(a);
        x.deallocate(b);
        x.deallocate(c);
    }


    void test_deallocate_2 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }

        x.deallocate(a);
        x.deallocate(c);
        x.deallocate(b);
    }

    void test_deallocate_3 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }
        x.deallocate(b);
        x.deallocate(a);
        x.deallocate(c);
    }

    void test_deallocate_4 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }

        x.deallocate(b);
        x.deallocate(c);
        x.deallocate(a);
    }

    void test_deallocate_5 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }

        x.deallocate(c);
        x.deallocate(a);
        x.deallocate(b);        
    }

    void test_deallocate_6 (){
        A x;
        pointer a = 0;
        pointer b = 0;
        pointer c = 0;

        try{
            a = x.allocate(1);
            b = x.allocate(2);
            c = x.allocate(3);
        } catch(std::bad_alloc &ba){

        }

        x.deallocate(c);
        x.deallocate(b);
        x.deallocate(a); 
    }

    CPPUNIT_TEST_SUITE(MyTestAllocator);
    CPPUNIT_TEST(test_construct_1);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST(test_deallocate_6);
    CPPUNIT_TEST_SUITE_END();

#undef RC_CP
#undef RC_IP
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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    tr.addTest(MyTestAllocator< Allocator<double,       100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<float,        100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<short,        100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<int,          100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<char,         100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long long,    100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long,         100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<bool,         100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long double,  100> >::suite());
    tr.addTest(MyTestAllocator< Allocator<wchar_t,      100> >::suite());


    tr.addTest(MyTestAllocator< Allocator<double,       20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<float,        20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<short,        20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<int,          20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<char,         20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long long,    20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long,         20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<bool,         20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long double,  20> >::suite());
    tr.addTest(MyTestAllocator< Allocator<wchar_t,      20> >::suite());


    tr.addTest(MyTestAllocator< Allocator<double,       8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<float,        8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<short,        8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<int,          8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<char,         8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long long,    8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long,         8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<bool,         8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<long double,  8> >::suite());
    tr.addTest(MyTestAllocator< Allocator<wchar_t,      8> >::suite());


    tr.run();

    cout << "Done." << endl;
    return 0;}
