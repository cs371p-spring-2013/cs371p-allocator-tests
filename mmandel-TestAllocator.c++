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

// ---------
// define
// ---------

#define private public
#define protected public
#define class struct

// --------
// includes
// --------

#include "Allocator.h"

// ----------------
// OurTestAllocator
// ----------------

template <typename A>
struct OurTestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::size_type       size_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // -----------
    // test_valid1
    // -----------

    void test_valid1 () {
        try {
            A x;
            int size = x.SIZE;
            CPPUNIT_ASSERT(x.valid());
            x.view(x.a[size - sizeof(int)]) = -92;
            CPPUNIT_ASSERT(!x.valid());}
        catch (...) {}}


    // -----------
    // test_valid2
    // -----------

    void test_valid2 () {
        try {
            A x;
            CPPUNIT_ASSERT(x.valid());
            x.view(x.a[4])  = -25;
            CPPUNIT_ASSERT(x.valid());
            x.view(x.a[5])  = 10;
            CPPUNIT_ASSERT(x.valid());
            x.view(x.a[0]) = 1;
            CPPUNIT_ASSERT(!x.valid());
            x.view(x.a[96]) = 4;
            CPPUNIT_ASSERT(!x.valid());}
        catch (...) {}}


    // -----------
    // test_valid3
    // -----------

    void test_valid3 () {
        try {
            A x;
            const difference_type s = 2;
            const pointer p = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p, s);
            const pointer f = x.allocate(s + 1);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(f, s + 1);}

        catch (...) {}}


    // ----------
    // test_view1
    // ----------

    void test_view1 () {
	try {
            A x;
            int size = x.SIZE;
            int sentinel = size - (sizeof(int) << 1);
            CPPUNIT_ASSERT(sentinel == x.view(x.a[0]));
            CPPUNIT_ASSERT(sentinel == x.view(x.a[sentinel + sizeof(int)]));
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}

    // ----------
    // test_view2
    // ----------

    void test_view2 () {
	try {
            A x;
            const difference_type s = 1;
            const pointer         p = x.allocate(s);
            int sizeT = sizeof(value_type);
            int sentinel = s * sizeT;
            CPPUNIT_ASSERT(sentinel == x.view(x.a[0]));
            CPPUNIT_ASSERT(sentinel == x.view(x.a[sizeof(int) - sentinel]));
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p, s);}
        catch (...) {}}


    // ----------
    // test_view3
    // ----------

    void test_view3 () {
	try {
            A x;
            const difference_type  s = 2;
            const pointer         p1 = x.allocate(s);
            const pointer         p2 = x.allocate(s);
            int sizeT = sizeof(value_type);
            int sentinel = s * sizeT;
            CPPUNIT_ASSERT(sentinel == x.view(x.a[0]));
            CPPUNIT_ASSERT(sentinel == x.view(x.a[sizeof(int) - sentinel]));
            CPPUNIT_ASSERT(sentinel == x.view(x.a[(sizeof(int) << 1) - sentinel]));
            CPPUNIT_ASSERT(sentinel == x.view(x.a[(sizeof(int) << 1) - (sentinel << 1)]));
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p1, s);
            x.deallocate(p2, s);}
        catch (...) {}}


    // ----------------
    // test_constructor
    // ----------------

    void test_constructor () {
	try {
            A x;}
        catch (...) {}}


    // --------------
    // test_allocate1
    // --------------

    void test_allocate1 () {
        try {
            A x;
            const difference_type s = 1;
            const pointer         p = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p, s);}
        catch (...) {}} 


    // --------------
    // test_allocate2
    // --------------

    void test_allocate2 () {
        try {
            A x;
            const difference_type s = 3;
            const pointer p1 = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            const pointer p2 = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());

            x.deallocate(p1, s); 
            x.deallocate(p2, s);} 
        catch (...) {}} 


    // --------------
    // test_allocate3
    // --------------

    void test_allocate3 () {
        try {
            A x;
            const difference_type s1 = 10; 
            const difference_type s2 = 5;
            const difference_type s3 = 2;
            const pointer         p1 = x.allocate(s2);
            CPPUNIT_ASSERT(x.valid());
            const pointer         p2 = x.allocate(s1);
            CPPUNIT_ASSERT(x.valid());
            const pointer         p3 = x.allocate(s3);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p1, s2);
            x.deallocate(p2, s1);
            x.deallocate(p3, s3);}
        catch (...) {}}


    // ----------------
    // test_deallocate1
    // ----------------

    // combine with free block on each side
    void test_deallocate1 () {
        try {
            A x;
            const difference_type  s = 5;
            const pointer         p1 = x.allocate(s);
            const pointer         p2 = x.allocate(s);
            const pointer         p3 = x.allocate(s);
            x.deallocate(p3, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p1, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p2, s);
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}


    // ----------------
    // test_deallocate2
    // ----------------

    // deallocate at beginning of array a, combine with free block on left
    void test_deallocate2 () {
        try {
            A x;
            const difference_type  s = 3;
            const pointer         p1 = x.allocate(s);
            const pointer         p2 = x.allocate(s);
            x.deallocate(p1, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p2, s);
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}


    // ----------------
    // test_deallocate3
    // ----------------

    // deallocate at end of array a, combine with free block on right
    void test_deallocate3 () {
        try {
            A x;
            const difference_type  s = 5;
            const pointer         p1 = x.allocate(s);
            const pointer         p2 = x.allocate(s);
            x.deallocate(p2, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p1, s);
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}


    // ----------------------
    // test_alloc_deallocate1
    // ----------------------

     void test_alloc_dealloc1 () {
         try {
             A x;
             const difference_type s = 5;
             x.allocate(5);
             CPPUNIT_ASSERT(x.valid());
             const pointer p2 = x.allocate(10);
             CPPUNIT_ASSERT(x.valid());
             x.allocate(5);
             CPPUNIT_ASSERT(x.valid());
             x.deallocate(p2, s);   // deallocate 10
             CPPUNIT_ASSERT(x.valid());
             const pointer p4 = x.allocate(10);
             CPPUNIT_ASSERT(x.valid());
             x.deallocate(p4, s);   // deallocate 10
             CPPUNIT_ASSERT(x.valid());
             x.allocate(3);
             CPPUNIT_ASSERT(x.valid());
              x.allocate(2);
             CPPUNIT_ASSERT(x.valid());}
         catch (...) {}}


    // ----------------------
    // test_alloc_deallocate2
    // ----------------------

    void test_alloc_dealloc2 () {
        try {
            A x;
            const difference_type s = 5;
            x.allocate(5);
            CPPUNIT_ASSERT(x.valid());
            const pointer p2 = x.allocate(10);
            CPPUNIT_ASSERT(x.valid());
            const pointer p3 = x.allocate(5);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p2, s);
            CPPUNIT_ASSERT(x.valid());
            const pointer p4 = x.allocate(10);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p4, s);
            CPPUNIT_ASSERT(x.valid());
            x.allocate(3);
            CPPUNIT_ASSERT(x.valid());
            const pointer p6 = x.allocate(2);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p3, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p6, s);
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}


    // ----------------------
    // test_alloc_deallocate3
    // ----------------------

    void test_alloc_dealloc3 () {
        try {
            A x;
            const difference_type s = 5;
            const pointer p1 = x.allocate(5);
            CPPUNIT_ASSERT(x.valid());
            const pointer p2 = x.allocate(5);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p2, s);
            CPPUNIT_ASSERT(x.valid());
            const pointer p3 = x.allocate(3);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p1, s);
            CPPUNIT_ASSERT(x.valid());
            const pointer p4 = x.allocate(3);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p3, s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p4, s);
            CPPUNIT_ASSERT(x.valid());
            x.allocate(6);
            CPPUNIT_ASSERT(x.valid());}
        catch (...) {}}


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(OurTestAllocator);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_view1);
    CPPUNIT_TEST(test_view2);
    CPPUNIT_TEST(test_view3);
    CPPUNIT_TEST(test_constructor);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST(test_alloc_dealloc1);
    CPPUNIT_TEST(test_alloc_dealloc2);
    CPPUNIT_TEST(test_alloc_dealloc3);
    CPPUNIT_TEST_SUITE_END();};


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
        try {
            A x;
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);
            x.construct(p, v);
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);}
        catch (...) {}}

/*
    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}
*/

    // --------
    // test_ten
    // --------

    void test_ten () {
        try {
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
        catch (...) {}}

/*
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
*/

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
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

//    tr.addTest(TestAllocator< Allocator<int,     0> >::suite()); // causes C++ warning
//    tr.addTest(OurTestAllocator< Allocator<int,     0> >::suite()); // causes C++ warning

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int,     4> >::suite());
    tr.addTest(TestAllocator< Allocator<int,    12> >::suite());
    tr.addTest(TestAllocator< Allocator<int,   100> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 25000> >::suite());

    tr.addTest(OurTestAllocator< Allocator<int,     4> >::suite());
    tr.addTest(OurTestAllocator< Allocator<int,    12> >::suite());
    tr.addTest(OurTestAllocator< Allocator<int,   100> >::suite());
    tr.addTest(OurTestAllocator< Allocator<int, 25000> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double,     4> >::suite());
    tr.addTest(TestAllocator< Allocator<double,    16> >::suite());
    tr.addTest(TestAllocator< Allocator<double,   100> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 25000> >::suite());

    tr.addTest(OurTestAllocator< Allocator<double,     4> >::suite());
    tr.addTest(OurTestAllocator< Allocator<double,    12> >::suite());
    tr.addTest(OurTestAllocator< Allocator<double,   100> >::suite());
    tr.addTest(OurTestAllocator< Allocator<double, 25000> >::suite());

    tr.addTest(TestAllocator< std::allocator<short> >::suite());
    tr.addTest(TestAllocator< Allocator<short,     4> >::suite());
    tr.addTest(TestAllocator< Allocator<short,    10> >::suite());
    tr.addTest(TestAllocator< Allocator<short,   100> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 25000> >::suite());

    tr.addTest(OurTestAllocator< Allocator<short,     4> >::suite());
    tr.addTest(OurTestAllocator< Allocator<short,    12> >::suite());
    tr.addTest(OurTestAllocator< Allocator<short,   100> >::suite());
    tr.addTest(OurTestAllocator< Allocator<short, 25000> >::suite());

    tr.addTest(TestAllocator< std::allocator<char> >::suite());
    tr.addTest(TestAllocator< Allocator<char,     4> >::suite());
    tr.addTest(TestAllocator< Allocator<char,     9> >::suite());
    tr.addTest(TestAllocator< Allocator<char,   100> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 25000> >::suite());

    tr.addTest(OurTestAllocator< Allocator<char,     4> >::suite());
    tr.addTest(OurTestAllocator< Allocator<char,    12> >::suite());
    tr.addTest(OurTestAllocator< Allocator<char,   100> >::suite());
    tr.addTest(OurTestAllocator< Allocator<char, 25000> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
