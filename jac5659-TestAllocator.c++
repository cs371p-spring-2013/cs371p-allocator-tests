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

    // -------
    // test_allocate
    // -------

    void test_allocate1(){
        A x;
        pointer p1,p2;
        try{
            p1 = x.allocate(5);
            p2 = x.allocate(2);
        }
        catch(...){
            CPPUNIT_ASSERT(false);
        }
        x.deallocate(p1,0);
        x.deallocate(p2,0);
        CPPUNIT_ASSERT(true);
    }

    void test_allocate2(){
        A x;
        pointer p;
        try{
            p = x.allocate(100);
        }
        catch (std::bad_alloc e){
            // for my allocator
            CPPUNIT_ASSERT(true);
        }
        // for std::allocator
        x.deallocate(p,0);
        CPPUNIT_ASSERT(true);
    }

    void test_allocate3(){
        A x;
        pointer p = x.allocate(5);
        x.deallocate(p,0);
        CPPUNIT_ASSERT(true);
    }

    // ------
    // test_deallocate
    // ------

    void test_deallocate1(){
        A x;
        pointer p = x.allocate(5);
        x.deallocate(p, 0);
        CPPUNIT_ASSERT(true);
    }

    void test_deallocate2(){
        A x;
        pointer p1, p2;
        p1  = x.allocate(5);
        p2 = x.allocate(5);
        x.deallocate(p1, 0);
        x.deallocate(p2, 10);
        CPPUNIT_ASSERT(true);
    }

    void test_deallocate3(){
        A x;
        pointer p;
        try{
            p = x.allocate(0);
        }
        catch (...){
            CPPUNIT_ASSERT(false);
        }
        x.deallocate(p,0);
        CPPUNIT_ASSERT(true);
    }    

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);

    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);
    
    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);
    
    CPPUNIT_TEST_SUITE_END();
};

// -----
// TestMyAllocator
// -----

struct TestMyAllocator : CppUnit::TestFixture {

    // --------
    // test_constructor
    // --------

    void test_constructor1(){
        try{
            Allocator<int,4> x;
            CPPUNIT_ASSERT(false);
        }
        catch(std::bad_alloc& e){
            CPPUNIT_ASSERT(strcmp(e.what(),"std::bad_alloc") == 0);
        }
    }

    void test_constructor2(){
        Allocator<int,100> x;
        CPPUNIT_ASSERT(x.valid());
    }
    
    void test_constructor3(){
        Allocator<double,100> x;
        CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // test_valid
    // --------

    void test_valid1(){
        Allocator<int, 100> x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid2(){
        Allocator<double, 100> x;
        double* p = x.allocate(5);
        x.set(*(((char*)p)-4),0 );
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid3(){
        Allocator<int, 100> x;
        int* p = x.allocate(23);
        *(p+23) = 92;
        CPPUNIT_ASSERT(!x.valid());
    }

    // -------
    // test_set
    // -------

    void test_set1(){
        Allocator<int, 100> x;
        x.set(x.a[0],20);
        CPPUNIT_ASSERT(x.view(x.a[0]) == 20);
    }

    void test_set2(){
        Allocator<int, 100> x;
        x.set(x.a[96],20);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 20);
    }

    void test_set3(){
        Allocator<int, 100> x;
        x.set(x.a[50],20);
        CPPUNIT_ASSERT(x.view(x.a[50]) == 20);
    }


    // suite
    CPPUNIT_TEST_SUITE(TestMyAllocator);

    CPPUNIT_TEST(test_constructor1);
    CPPUNIT_TEST(test_constructor2);
    CPPUNIT_TEST(test_constructor3);

    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);

    CPPUNIT_TEST(test_set1);
    CPPUNIT_TEST(test_set2);
    CPPUNIT_TEST(test_set3);

    CPPUNIT_TEST_SUITE_END();

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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
    
    tr.addTest(TestMyAllocator::suite());
    tr.run();

    cout << "Done." << endl;
    return 0;
}
