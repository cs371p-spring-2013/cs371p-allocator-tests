// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Hyunchel Kim
// Manoj Dhanapal
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
// StructAllocator
// -------------

struct y {
  long x;
  char t;

  y(): x(0), t(0) { }
  y(const y& r): x(r.x), t(r.t) { }
  ~y() {
     x = 0;
     t = 0;
  }
};

template <typename A>
struct StructAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::size_type       size_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one
    // --------

    void test_struct () {
        A x;
        const difference_type s = 1;
              value_type      v;
                            v.x = 423;
                            v.t = 127;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(p->x == 423);
        CPPUNIT_ASSERT(p->t == 127);
        x.destroy(p);
        CPPUNIT_ASSERT(p->x == 0);
        CPPUNIT_ASSERT(p->t == 0);
        x.deallocate(p, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(StructAllocator);
    CPPUNIT_TEST(test_struct);
    CPPUNIT_TEST_SUITE_END();};

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::size_type       size_type;
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
        catch (...) { while (b != p) {
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

// ------------
// OurAllocator
// ------------

template <typename A>
struct OurAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::size_type       size_type;
    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // ----------------
    // test_constructor
    // ----------------

    void test_constructor1 () {
        // call the constructor
        A x;
        // check if the created object is valid
        CPPUNIT_ASSERT(x.valid());}

    void test_constructor2 () {
        // call the constructor
        A x;
        // check that the first and last values are proper
        size_type first = x.view(0,x.a);
        size_type final = x.view(x.getSize()-sizeof(size_type),x.a);
        CPPUNIT_ASSERT(first == final);
        CPPUNIT_ASSERT(first == (size_type)(x.getSize()-(2*sizeof(size_type))));}

    void test_constructor3 () {
        // call the constructor as an array
        A x[2];
        // check that both the constructors are valid
        CPPUNIT_ASSERT(x[0].valid());
        CPPUNIT_ASSERT(x[1].valid());}

    // ----------
    // test_valid
    // ----------

    void test_valid1 () {
        // create an object
        A x;
        // check if the last and initial values are the same
        size_type first = x.view(0, x.a);
        size_type final = x.view(x.getSize()-sizeof(size_type), x.a);
        CPPUNIT_ASSERT(first == final);
        // the valid method should confirm this
        CPPUNIT_ASSERT(x.valid());
        // change the first value
        *(size_type*)x.a = *(size_type*)x.a + 4;
        // check if final is less than first by 4
        first = x.view(0, x.a);
        final = x.view(x.getSize()-sizeof(size_type), x.a);
        CPPUNIT_ASSERT(first == (final+4));
        // make sure valid method catches this!
        CPPUNIT_ASSERT(!x.valid());}

    void test_valid2 () {
        // create an object
        A x;
        // check valid
        CPPUNIT_ASSERT(x.valid());
        // modify the available space so that not even a single element fits
        // modify the first sentinel
        *(size_type*)x.a = sizeof(value_type) - 1;
        // modify the final sentinel
        *((size_type*)(x.a+sizeof(value_type)-1+sizeof(size_type))) = sizeof(value_type) - 1;
        // make sure valid catches this issue
        CPPUNIT_ASSERT(!x.valid());}

    void test_valid3 () {
        // create an object
        A x;
        // make sure valid succeeds after the constructor was called
        CPPUNIT_ASSERT(x.valid());}


    // --------------
    // test_construct
    // --------------

    void test_contruct1 () {
        A x;
        const difference_type s = 5;
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

    void test_contruct2 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}        

    void test_contruct3 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 92;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}

    // ------------
    // test_destroy
    // ------------

    void test_destroy1 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p, s);}
        
    void test_destroy2 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 92;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p, s);}

    void test_destroy3 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 127;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(p, s);}

    // -------------
    // test_allocate
    // -------------

    void test_allocate1 () {
        A x;
        try {
          const pointer b = x.allocate(400);
          *b = 12;
        } catch (std::bad_alloc& y) {
          // do not have enough space
          return;
        }
        // should have returned bad_alloc when out of memory
        throw;}

    void test_allocate2 () {
        A x;
        pointer one, two;

        // check if there's enough space for the test
        CPPUNIT_ASSERT(x.getSize() >= (size_type)(6*sizeof(size_type)+8*sizeof(value_type)));
         
        CPPUNIT_ASSERT(x.valid());
        one = x.allocate(5);
        CPPUNIT_ASSERT(x.valid());

        two = x.allocate(3);
        CPPUNIT_ASSERT(x.valid());
         
        //checking the first sentinel
        CPPUNIT_ASSERT(*(size_type*)x.a == ((size_type)(5*sizeof(value_type)*(-1))));

        // checking the last sentinel
        size_type last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(last_sen == ((size_type)(x.getSize()-6*sizeof(size_type)-8*sizeof(value_type))));

        // deallocate the allocated space
        x.deallocate(one);
        x.deallocate(two);}

    void test_allocate3 () {
        A x;
        pointer one, two;

        CPPUNIT_ASSERT(x.valid());
        one = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(one);

        size_type last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(*(size_type*)x.a == last_sen);
        CPPUNIT_ASSERT(*(size_type*)x.a == (size_type)(x.getSize()-(2*sizeof(size_type))));

        CPPUNIT_ASSERT(x.valid());
        two = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());

        last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(last_sen == (size_type)(x.getSize()-sizeof(value_type)-(4*sizeof(size_type))));
        CPPUNIT_ASSERT(*(size_type*)x.a == (size_type)((-1)*sizeof(value_type)));

        // deallocate the allocated space
        x.deallocate(two);}

    // ---------------
    // test_deallocate
    // ---------------

    void test_deallocate1 () {
        A x;
        pointer one, two, three, four, five;

        // allocate the data
        one   = x.allocate(1);
        two   = x.allocate(1);
        three = x.allocate(1);
        four  = x.allocate(1);
        five  = x.allocate(1);

        // deallocate two and four
        x.deallocate(two);
        x.deallocate(four);

        // check if valid
        CPPUNIT_ASSERT(x.valid());

        // deallocate space
        x.deallocate(one);
        x.deallocate(three);
        x.deallocate(five);
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(*(size_type*)x.a == (size_type)(x.getSize()-(2*sizeof(size_type))));}

    void test_deallocate2 () {
        A x;
        size_type last_sen;
        pointer one, two;

        // check if there's enough space for the test
        CPPUNIT_ASSERT(x.getSize() >= (size_type)(6*sizeof(size_type)+8*sizeof(value_type)));

        CPPUNIT_ASSERT(x.valid());
        one = x.allocate(5);
        CPPUNIT_ASSERT(x.valid());

        two = x.allocate(3);
        CPPUNIT_ASSERT(x.valid());

        // deallocate the allocated space
        x.deallocate(one);
        CPPUNIT_ASSERT(x.valid());

        // check first and last sentinels
        CPPUNIT_ASSERT(*(size_type*)x.a == ((size_type)(5*sizeof(value_type))));
        last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(last_sen == ((size_type)(x.getSize()-6*sizeof(size_type)-8*sizeof(value_type))));

        x.deallocate(two);
        CPPUNIT_ASSERT(x.valid());

        // check first and last sentinels
        last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(*(size_type*)x.a == last_sen);
        CPPUNIT_ASSERT(last_sen = (size_type)(x.getSize()-sizeof(size_type)));}

    void test_deallocate3 () {
        A x;
        pointer one, two, three, four, five;

        // allocate the data
        one   = x.allocate(1);
        two   = x.allocate(1);
        three = x.allocate(1);
        four  = x.allocate(1);
        five  = x.allocate(1);

        // deallocate
        x.deallocate(five);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(four);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(three);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(two);
        CPPUNIT_ASSERT(x.valid());
        x.deallocate(one);
        CPPUNIT_ASSERT(x.valid());
        size_type last_sen = *((size_type*)&x.a[x.getSize()-sizeof(size_type)]);
        CPPUNIT_ASSERT(*(size_type*)x.a == last_sen);
        CPPUNIT_ASSERT(*(size_type*)x.a == (size_type)(x.getSize()-(2*sizeof(size_type))));}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(OurAllocator);
    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);
    CPPUNIT_TEST(test_constructor1);
    CPPUNIT_TEST(test_constructor2);
    CPPUNIT_TEST(test_constructor3);
    CPPUNIT_TEST(test_contruct1);
    CPPUNIT_TEST(test_contruct2);
    CPPUNIT_TEST(test_contruct3);
    CPPUNIT_TEST(test_destroy1);
    CPPUNIT_TEST(test_destroy2);
    CPPUNIT_TEST(test_destroy3);
    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    // commenting sync_with_stdio as this leaks memory when checked with valgrind
    //ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O

    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<char> >::suite());
    tr.addTest(TestAllocator< Allocator<char, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<char, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<short> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<short, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<double, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<float> >::suite());
    tr.addTest(TestAllocator< Allocator<float, 200> >::suite());
    tr.addTest(OurAllocator< Allocator<float, 200> >::suite());

    tr.addTest(StructAllocator< std::allocator<y> >::suite());
    tr.addTest(StructAllocator< Allocator<y, 200> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}

