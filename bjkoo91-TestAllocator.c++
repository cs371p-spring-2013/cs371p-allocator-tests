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

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct OurTests : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    typedef typename A::size_type       size_type;

    static const size_type value_size = sizeof (value_type);
    static const size_type sentinel_size = sizeof (size_type);

    // ----------------
    // test_constructor
    // ----------------

    /* test if the allocator is allocated correctly */
    void test_constructor () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]));
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == x.allocator_size - 2 * sentinel_size);}}

    // ---------
    // test_view
    // ---------

    /* test if view can read the sentinel value given the char pointer */
    void test_view_1 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            size_type sentinel_value = x.allocator_size - 2 * sentinel_size;
            CPPUNIT_ASSERT (x.view (&x.a[0]) == sentinel_value);
            CPPUNIT_ASSERT (x.view (&x.a[x.allocator_size - sentinel_size]) == sentinel_value);}}

    /* test if view can modify the sentinel value (aka is it returning l-value) */
    void test_view_2 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            x.view (&x.a[0]) = 123;
            x.view (&x.a[x.allocator_size - sentinel_size]) = -321;
            CPPUNIT_ASSERT (x.view (&x.a[0]) == 123);
            CPPUNIT_ASSERT (x.view (&x.a[x.allocator_size - sentinel_size]) == -321);}}
    
    /* test if view can write to any location given a char pointer */
    void test_view_3 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            for (size_type i = 0; i < x.allocator_size; i+=4)
                x.view (&x.a[i]) = i;
            for (size_type i = 0; i < x.allocator_size; i+=4)
                CPPUNIT_ASSERT (x.view (&x.a[i]) == i);}}

    // ----------
    // test_valid
    // ----------

    /* test if valid works for the initial allocator (with 2 correct sentinels) */
    void test_valid_1 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]));
            CPPUNIT_ASSERT (x.valid ());}}
    
    /* test if valid returns false with an incorrect sentinel */
    void test_valid_2 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            x.a[x.allocator_size - sentinel_size] = 1;
            CPPUNIT_ASSERT (!x.valid ());}}

    /* test if valid works for a negative pair of sentinels */
    void test_valid_3 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            size_type sentinel_value = x.allocator_size - 2 * sentinel_size;
            *reinterpret_cast<size_type*>(&x.a[0]) = -sentinel_value;
            *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) = -sentinel_value;
            CPPUNIT_ASSERT (x.valid ());}}

    // -------------
    // test_allocate
    // -------------

    /* test if allocate correctly adds and modifies sentinels */
    void test_allocate_1 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            x.allocate (5);
            size_type allocated = 5 * value_size;
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == -allocated);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[sentinel_size + allocated]) == -allocated);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[2 * sentinel_size + allocated]) == x.allocator_size - 4 * sentinel_size - allocated);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) == x.allocator_size - 4 * sentinel_size - allocated);}}

    /* test if multiple allocate calls correctly add and modify sentinels */
    void test_allocate_2 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            x.allocate (5);
            size_type allocated_1 = 5 * value_size;
            x.allocate (1);
            size_type allocated_2 = value_size;
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == -allocated_1);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[sentinel_size + allocated_1]) == -allocated_1);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[2 * sentinel_size + allocated_1]) == -allocated_2);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[3 * sentinel_size + allocated_1 + allocated_2]) == -allocated_2);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[4 * sentinel_size + allocated_1 + allocated_2]) == x.allocator_size - 6 * sentinel_size - allocated_1 - allocated_2);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) == x.allocator_size - 6 * sentinel_size - allocated_1 - allocated_2);}}

    /* test if bad_alloc exception is thrown for attempting to allocate negative space */
    void test_allocate_3 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            try {
                x.allocate (-5);
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}}

    /* test if bad_alloc exception is thrown when there is not enough space to allocate */
    void test_allocate_4 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            try {
                x.allocate (x.allocator_size);
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}}

    // ---------------
    // test_deallocate
    // ---------------

    /* test if deallocate correctly changes negative sentinel values to positive */
    void test_deallocate_1 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            size_type sentinel_value = x.allocator_size - 2 * sentinel_size;
            *reinterpret_cast<size_type*>(&x.a[0]) = -sentinel_value;
            *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) = -sentinel_value;
            x.deallocate (reinterpret_cast<pointer>(&x.a[4]));
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == sentinel_value);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) == sentinel_value);}}

    /* test if deallocate does not merge this block with adjacent busy blocks */
    void test_deallocate_2 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            *reinterpret_cast<size_type*>(&x.a[0]) = -value_size;
            *reinterpret_cast<size_type*>(&x.a[sentinel_size + value_size]) = -value_size;
            *reinterpret_cast<size_type*>(&x.a[2 * sentinel_size + value_size]) = -x.allocator_size + 4 * sentinel_size + value_size;
            *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) = -x.allocator_size + 4 * sentinel_size + value_size;
            x.deallocate (reinterpret_cast<pointer>(&x.a[4]));
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == value_size);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[sentinel_size + value_size]) == value_size);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[2 * sentinel_size + value_size]) == -x.allocator_size + 4 * sentinel_size + value_size);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) == -x.allocator_size + 4 * sentinel_size + value_size);}}

    /* test if deallocate merges this block with adjacent free blocks */
    void test_deallocate_3 () {
        if (A::allocator_size <= value_size + 2 * sentinel_size) {
            try {
                A x;
                CPPUNIT_ASSERT (false);}
            catch (std::bad_alloc&) {}}
        else {
            A x;
            size_type sentinel_value = x.allocator_size - 2 * sentinel_size;
            *reinterpret_cast<size_type*>(&x.a[0]) = -value_size;
            *reinterpret_cast<size_type*>(&x.a[sentinel_size + value_size]) = -value_size;
            *reinterpret_cast<size_type*>(&x.a[2 * sentinel_size + value_size]) = x.allocator_size - 4 * sentinel_size - value_size;
            *reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) = x.allocator_size - 4 * sentinel_size - value_size;
            x.deallocate (reinterpret_cast<pointer>(&x.a[4]));
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[0]) == sentinel_value);
            CPPUNIT_ASSERT (*reinterpret_cast<size_type*>(&x.a[x.allocator_size - sentinel_size]) == sentinel_value);}}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE (OurTests);
    CPPUNIT_TEST (test_constructor);
    CPPUNIT_TEST (test_view_1);
    CPPUNIT_TEST (test_view_2);
    CPPUNIT_TEST (test_view_3);
    CPPUNIT_TEST (test_valid_1);
    CPPUNIT_TEST (test_valid_2);
    CPPUNIT_TEST (test_valid_3);
    CPPUNIT_TEST (test_allocate_1);
    CPPUNIT_TEST (test_allocate_2);
    CPPUNIT_TEST (test_allocate_3);
    CPPUNIT_TEST (test_allocate_4);
    CPPUNIT_TEST (test_deallocate_1);
    CPPUNIT_TEST (test_deallocate_2);
    CPPUNIT_TEST (test_deallocate_3);
    CPPUNIT_TEST_SUITE_END ();};

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

    /* We pass these tests, but we get warnings as C++ forbids zero-size array
       Uncomment them to test those */
    // tr.addTest(OurTests< Allocator<char, 0> >::suite());
    // tr.addTest(OurTests< Allocator<int, 0> >::suite());
    // tr.addTest(OurTests< Allocator<double, 0> >::suite());
    // tr.addTest(OurTests< Allocator<long long, 0> >::suite ());

    tr.addTest(OurTests< Allocator<char, 4> >::suite());
    tr.addTest(OurTests< Allocator<int, 4> >::suite());
    tr.addTest(OurTests< Allocator<double, 4> >::suite());
    tr.addTest(OurTests< Allocator<long long, 4> >::suite ());

    tr.addTest(OurTests< Allocator<char, 100> >::suite());
    tr.addTest(OurTests< Allocator<int, 100> >::suite());
    tr.addTest(OurTests< Allocator<double, 100> >::suite());
    tr.addTest(OurTests< Allocator<long long, 100> >::suite ());

    tr.addTest(OurTests< Allocator<char, 1000> >::suite());
    tr.addTest(OurTests< Allocator<int, 1000> >::suite());
    tr.addTest(OurTests< Allocator<double, 1000> >::suite());
    tr.addTest(OurTests< Allocator<long long, 1000> >::suite ());

    tr.addTest(OurTests< Allocator<char, 10000> >::suite());
    tr.addTest(OurTests< Allocator<int, 10000> >::suite());
    tr.addTest(OurTests< Allocator<double, 10000> >::suite());
    tr.addTest(OurTests< Allocator<long long, 10000> >::suite ());

    tr.run();

    cout << "Done." << endl;
    return 0;}