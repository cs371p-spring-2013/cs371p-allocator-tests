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
#include <iostream> // ios_base
#include <memory> // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h" // TestFixture
#include "cppunit/TextTestRunner.h" // TestRunner

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

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p = x.allocate(s);
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
        const value_type v = 2;
        const pointer b = x.allocate(s);
              pointer e = b + s;
              pointer p = b;
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


template <typename B>
struct TestAllocatorLab : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename B::value_type value_type;
    typedef typename B::difference_type difference_type;
    typedef typename B::pointer pointer;

    // --------
    // test_view
    // --------

    void test_view(){
      char a[100] = "abcdefghijklmnopqrstuv";
      view(a[96]) = view(a[0]);
      assert(view(a[0]) == view(a[96]));
    }

    void test_view_mult(){
      char a[100] = "abcdefghijklmnopqrstuvwxyz1234567890abcdefghijklm";
      int i;
      for(i = 0; i < 10; i++){
        int* ptr = reinterpret_cast<int*>(&a[i*4]);
        CPPUNIT_ASSERT(*ptr == view(a[i*4]));
      }
    }
 
    void test_view_types(){
      char a[100] = "1234";
      int i = 0;
      int* ptr = reinterpret_cast<int*>(&a[i]);
      CPPUNIT_ASSERT(*ptr == view(a[i]));
    }


    // ----------------
    // test_constructor
    // ----------------
 
    void test_construct () {
      B all;
      CPPUNIT_ASSERT(view(all.a[0]) == view(all.a[all.len-4]));
    }
    
    // ----------------
    // test_allocate
    // ----------------

    void test_alloc_mult () {
        B x;
        const difference_type s1 = 1;
        const difference_type s2 = 3;
        const difference_type s3 = 4;
        const value_type v = 2;
        const pointer p = x.allocate(s1);
        const pointer q = x.allocate(s2);
        const pointer r = x.allocate(s3);

        x.construct(p, v);
        x.construct(q, v);
        x.construct(r, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        CPPUNIT_ASSERT(*r == v);
        x.destroy(p);
        x.destroy(r);
        x.destroy(q);
        x.deallocate(r, s3);   
        x.deallocate(p, s1);   
        x.deallocate(q, s2);   
    }

    void test_bad_alloc () {
        B x;
        const difference_type s1 = 100;
        try{
          x.allocate(s1);
          CPPUNIT_ASSERT(false);}
        catch(...){
          CPPUNIT_ASSERT(true);
        }
    }

    void test_bad2_alloc () {
        B x;
        const difference_type s1 = 100;
        const difference_type s2 = 0;
        const difference_type s3 = -1;
        try{
          x.allocate(s1);
          CPPUNIT_ASSERT(false);}
        catch(...){
          CPPUNIT_ASSERT(true);
        }
        try{
          x.allocate(s2);
          CPPUNIT_ASSERT(false);}
        catch(...){
          CPPUNIT_ASSERT(true);
        }
        try{
          x.allocate(s3);
          CPPUNIT_ASSERT(false);}
        catch(...){
          CPPUNIT_ASSERT(true);
        }
    }

    // --------
    // test_dealloc
    // --------

    void test_dealloc () {
        B x;
        const difference_type s1 = 1;
        const difference_type s2 = 3;
        const difference_type s3 = 4;
        const value_type v = 2;
        const pointer p = x.allocate(s1);
        const pointer q = x.allocate(s2);
        const pointer r = x.allocate(s3);

        x.construct(p, v);
        x.construct(q, v);
        x.construct(r, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        CPPUNIT_ASSERT(*r == v);
        x.destroy(p);
        x.destroy(r);
        x.destroy(q);
        x.deallocate(r, s3);   
        x.deallocate(p, s1);   
        x.deallocate(q, s2);
        CPPUNIT_ASSERT(view(x.a[0]) == view(x.a[x.len-4]));
    }

    void test_dealloc_reverse () {
        B x;
        const difference_type s1 = 1;
        const difference_type s2 = 3;
        const difference_type s3 = 4;
        const value_type v = 2;
        const pointer p = x.allocate(s1);
        const pointer q = x.allocate(s2);
        const pointer r = x.allocate(s3);

        x.construct(p, v);
        x.construct(q, v);
        x.construct(r, v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        CPPUNIT_ASSERT(*r == v);
        x.destroy(p);
        x.destroy(r);
        x.destroy(q);
        x.deallocate(r, s3);   
        x.deallocate(q, s2);
        x.deallocate(p, s1);   
        CPPUNIT_ASSERT(view(x.a[0]) == view(x.a[x.len-4]));
    }

    void test_dealloc_realloc () {
        B x;
        const difference_type s1 = 1;
        const difference_type s2 = 3;
        const difference_type s3 = 4;
        const difference_type r1 = 1;
        const difference_type r2 = 3;

        const value_type v = 2;

        const pointer p = x.allocate(s1);
        const pointer q = x.allocate(s2);
        const pointer r = x.allocate(s3);

        x.construct(p, v);
        x.construct(q, v);
        x.construct(r, v);

        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*q == v);
        CPPUNIT_ASSERT(*r == v);

        x.destroy(p);
        x.destroy(q);
        x.destroy(r);

        x.deallocate(r, s3);
        x.deallocate(p, s1); 
        const pointer t = x.allocate(r2);
        const pointer u = x.allocate(r1);

        x.deallocate(q, s2); 
        x.deallocate(t, r2); 
        x.deallocate(u, r1);
        CPPUNIT_ASSERT(view(x.a[0]) == view(x.a[x.len-4]));
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocatorLab);
    CPPUNIT_TEST(test_view);
    CPPUNIT_TEST(test_view_mult);
    CPPUNIT_TEST(test_view_types);
    CPPUNIT_TEST(test_construct);
    CPPUNIT_TEST(test_alloc_mult);
    CPPUNIT_TEST(test_bad_alloc);
    CPPUNIT_TEST(test_bad2_alloc);
    CPPUNIT_TEST(test_dealloc);
    CPPUNIT_TEST(test_dealloc_reverse);
    CPPUNIT_TEST(test_dealloc_realloc);
    CPPUNIT_TEST_SUITE_END();};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    //tr.addTest(TestAllocator< std::allocator<int> >::suite());
   tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    //tr.addTest(TestAllocator< std::allocator<double> >::suite());
   tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

   tr.addTest(TestAllocatorLab< Allocator<int, 100> >::suite());

   tr.addTest(TestAllocatorLab< Allocator<double, 100> >::suite());
   tr.run();

    cout << "Done." << endl;
    return 0;}
