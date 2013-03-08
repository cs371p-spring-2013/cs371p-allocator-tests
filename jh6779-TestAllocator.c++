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
    CPPUNIT_TEST_SUITE_END();};

// --------------
// TestAllocator2
// --------------

template <typename A>
struct TestAllocator2 : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    
    // --------
    // test_con
    // --------

    void test_con () {
        A x;
        int size = sizeof(x.a)/sizeof(x.a[0]);
        size -= 2*sizeof(int);
        char* ptr = x.a;
        int& f = x.view(*ptr);
        CPPUNIT_ASSERT(f > 0);
        ptr += f + sizeof(int);
        int& b = x.view(*ptr);
        CPPUNIT_ASSERT(b > 0);
        CPPUNIT_ASSERT(f == b);
        CPPUNIT_ASSERT(b == size);}
    
    // ------------
    // test_valid_1
    // ------------

    void test_valid_1 () {
        A x;
        CPPUNIT_ASSERT(x.valid());}
    
    // ------------
    // test_valid_2
    // ------------

    void test_valid_2 () {
        A x;
        char* ptr = x.a;
        int& f = x.view(*ptr);
        f = 10;
        CPPUNIT_ASSERT(!x.valid());}
    
    // ------------
    // test_valid_3
    // ------------

    void test_valid_3 () {
        A x;
        int size = sizeof(x.a); // x.a is array of char which is 1 byte no need for sizeof(x.a[0])
        size -= 2*sizeof(int);
        char* ptr = x.a;
        int& f = x.view(*ptr);
        ptr += f + sizeof(int);
        int& b = x.view(*ptr);
        f = -size;
        b = f;
        CPPUNIT_ASSERT(x.valid());}

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
        
    // ----------------
    // test_allocate_1
    // ----------------
    
    void test_allocate_1 () {
        A x;
        try{
            x.allocate(101);
            CPPUNIT_ASSERT(false);
        }catch(std::bad_alloc&){
            CPPUNIT_ASSERT(true);
        }}
        
    // ----------------
    // test_allocate_2
    // ----------------
    
    void test_allocate_2 () {
        A x;
        difference_type s = sizeof(x.a); // x.a is array of char which is 1 byte no need for sizeof(x.a[0])
        s -= 2*sizeof(int);
        s /= sizeof(value_type);
        x.allocate(s);
        try{
            x.allocate(1);
            CPPUNIT_ASSERT(false);
        }catch(std::bad_alloc&){
            CPPUNIT_ASSERT(true);
        }}
        
    // ----------------
    // test_allocate_3
    // ----------------
    
    void test_allocate_3 () {
        A x;
        x.allocate(2);
        char* p = x.a;
        int& f = x.view(*p);
        p += -f + sizeof(int);
        int& b = x.view(*p);
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(f == b);
        CPPUNIT_ASSERT(f == -(signed)(2*sizeof(value_type)));}
        
    // ----------------
    // test_deallocate_1
    // ----------------
    
    void test_deallocate_1 () {
        A x;
        const pointer t = x.allocate(10);
        x.deallocate(t+102);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(t);}
        
    // ----------------
    // test_deallocate_2
    // ----------------
    
    void test_deallocate_2 () {
        A x;
        const pointer t1 = x.allocate(5);
        const pointer t2 = x.allocate(5);
        x.deallocate(t1+102);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(t1);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(t2);
        CPPUNIT_ASSERT(x.valid());}
        
    // ----------------
    // test_deallocate_3
    // ----------------
    
    void test_deallocate_3 () {
        A x;
        const pointer t1 = x.allocate(2);
        const pointer t2 = x.allocate(1);
        const pointer t3 = x.allocate(2);
        const pointer t4 = x.allocate(2);
        x.deallocate(t1+102);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(t3);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(t2);
        CPPUNIT_ASSERT(x.valid());
        char* ptr = x.a;
        int& f = x.view(*ptr);
        int& f2 = x.view(*(ptr - f + 2*sizeof(int)));
        CPPUNIT_ASSERT(f2 == 3*sizeof(value_type) + 2*sizeof(int));
        x.deallocate(t1);
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(f == 5*sizeof(value_type) + 4*sizeof(int));
        x.deallocate(t4);
        CPPUNIT_ASSERT(x.valid());}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator2);
    CPPUNIT_TEST(test_con);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
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
  tr.addTest(TestAllocator2< Allocator<int, 100> >::suite()); // uncomment!

//    tr.addTest(TestAllocator< std::allocator<double> >::suite());
  tr.addTest(TestAllocator2< Allocator<double, 100> >::suite()); // uncomment!
  tr.addTest(TestAllocator2< Allocator<float, 100> >::suite());
  tr.addTest(TestAllocator2< Allocator<char, 100> >::suite());
  tr.addTest(TestAllocator2< Allocator<bool, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}