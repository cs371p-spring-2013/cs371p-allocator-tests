// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/**
 * To test the program:
 *     g++ -ansi -pedantic -lcppunit -ldl -Wall TestAllocator.c++ -o TestAllocator.app
 *     valgrind TestAllocator.app > TestAllocator.out
 */

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TestSuite.h"               // TestSuite
#include "cppunit/TextTestRunner.h"          // TestRunner

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
        if (p)
            CPPUNIT_ASSERT(*p == v); // uncomment!
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
        if (b)
            CPPUNIT_ASSERT(std::count(b, e, v) == s); // uncomment!
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}


    // -------------------
    // test_oneMultAllocs1
    // -------------------

    void test_oneMultAllocs1 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p1);
        x.destroy(p2);
        x.destroy(p3);
        x.deallocate(p1, s);
        x.deallocate(p2, s);
        x.deallocate(p3, s);}

    // -------------------
    // test_oneMultAllocs2
    // -------------------

    void test_oneMultAllocs2 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p3);
        x.destroy(p2);
        x.destroy(p1);
        x.deallocate(p3, s);
        x.deallocate(p2, s);
        x.deallocate(p1, s);}

    // -------------------
    // test_oneMultAllocs3
    // -------------------

    void test_oneMultAllocs3 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p1);
        x.destroy(p3);
        x.destroy(p2);
        x.deallocate(p3, s);
        x.deallocate(p1, s);
        x.deallocate(p2, s);}

    // -------------------
    // test_oneMultAllocs4
    // -------------------

    void test_oneMultAllocs4 () {
        A x;
        const difference_type s = 2;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p1);
        x.destroy(p3);
        x.destroy(p2);
        x.deallocate(p2, s);
        x.deallocate(p3, s);
        x.deallocate(p1, s);}

    // -----------------------
    // test_varyingMultAllocs1
    // -----------------------

    void test_varyingMultAllocs1 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(2);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        const pointer p4 = x.allocate(s);
        const pointer p5 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        x.construct(p4, v);
        x.construct(p5, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        CPPUNIT_ASSERT(*p4 == v);
        CPPUNIT_ASSERT(*p5 == v);
        x.destroy(p1);
        x.destroy(p4);
        x.destroy(p5);
        x.destroy(p2);
        x.destroy(p3);
        x.deallocate(p4, s);
        x.deallocate(p5, s);
        x.deallocate(p2, s);
        x.deallocate(p1, s);
        x.deallocate(p3, s);}

    // -----------------------
    // test_varyingMultAllocs2
    // -----------------------

    void test_varyingMultAllocs2 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        const pointer p4 = x.allocate(s);
        const pointer p5 = x.allocate(2);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        x.construct(p4, v);
        x.construct(p5, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        CPPUNIT_ASSERT(*p4 == v);
        CPPUNIT_ASSERT(*p5 == v);
        x.destroy(p2);
        x.destroy(p1);
        x.destroy(p4);
        x.destroy(p3);
        x.destroy(p5);
        x.deallocate(p4, s);
        x.deallocate(p2, s);
        x.deallocate(p3, s);
        x.deallocate(p1, s);
        x.deallocate(p5, s);}

    // -----------------------
    // test_varyingMultAllocs3
    // -----------------------

    void test_varyingMultAllocs3 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(2);
        const pointer p4 = x.allocate(s);
        const pointer p5 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        x.construct(p4, v);
        x.construct(p5, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        CPPUNIT_ASSERT(*p4 == v);
        CPPUNIT_ASSERT(*p5 == v);
        x.destroy(p2);
        x.destroy(p1);
        x.destroy(p4);
        x.destroy(p3);
        x.destroy(p5);
        x.deallocate(p4, s);
        x.deallocate(p2, s);
        x.deallocate(p3, s);
        x.deallocate(p1, s);
        x.deallocate(p5, s);}

    // -------------------
    // test_oneOutOfOrder1
    // -------------------

    void test_oneOutOfOrder1 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p3);
        x.deallocate(p3, s);
        const pointer p4 = x.allocate(s);
        x.construct(p4, v);
        CPPUNIT_ASSERT(*p4 == v);
        x.destroy(p1);
        x.destroy(p2);
        x.destroy(p4);
        x.deallocate(p1, s);
        x.deallocate(p2, s);
        x.deallocate(p4, s);}

    // -------------------
    // test_oneOutOfOrder2
    // -------------------

    void test_oneOutOfOrder2 () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p1 = x.allocate(s);
        const pointer p2 = x.allocate(s);
        const pointer p3 = x.allocate(s);
        x.construct(p1, v);
        x.construct(p2, v);
        x.construct(p3, v);
        CPPUNIT_ASSERT(*p1 == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(*p3 == v);
        x.destroy(p2);
        x.deallocate(p2, s);
        const pointer p4 = x.allocate(s);
        x.construct(p4, v);
        CPPUNIT_ASSERT(*p4 == v);
        x.destroy(p1);
        x.destroy(p3);
        x.destroy(p4);
        x.deallocate(p1, s);
        x.deallocate(p3, s);
        x.deallocate(p4, s);}

    // --------
    // test_3 (allocate 10 and deallocate 10 two times)
    // --------

    void test_3 () {
        A x;
        difference_type s = 10;
        value_type v = 2;
        pointer b = x.allocate(s);
        pointer e = b + s;
        pointer p = b;
        try {
            while(p != e){
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while(b != p){
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        if(b){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
        }
        while(b != e){
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
        
        pointer b2 = x.allocate(s);
        pointer e2 = b2 + s;
        pointer p2 = b2;
        try {
            while(p2 != e2){
                x.construct(p2, v);
                ++p2;
            }
        }
        catch (...){
            while(b2 != p2){
                --p2;
                x.destroy(p2);
            }
            x.deallocate(b2, s);
            throw;
        }
        if(b2){
            CPPUNIT_ASSERT(std::count(b2, e2, v) == s);
        }
        while(b2 != e2){
            --e2;
            x.destroy(e2);
        }
        x.deallocate(b2, s);
    }

    // --------
    // test_4 (allocate 3, then allocate 3 again)
    // --------

    void test_4 () {
        A x;
        difference_type s = 3;
        value_type v = 2;
        pointer b = x.allocate(s);
        pointer e = b + s;
        pointer p = b;
        try {
            while(p != e){
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while(b != p){
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        if(b){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
        }
        
        pointer b2 = x.allocate(s);
        pointer e2 = b2 + s;
        pointer p2 = b2;
        try {
            while(p2 != e2){
                x.construct(p2, v);
                ++p2;
            }
        }
        catch (...){
            while(b2 != p2){
                --p2;
                x.destroy(p2);
            }
            x.deallocate(b2, s);
            throw;
        }
        if(b2){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v) == s);
        }
        while(b != e){
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
        while(b2 != e2){
            --e2;
            x.destroy(e2);
        }
        x.deallocate(b2, s);
    }
    
    // --------
    // test_5 (allocate 3, then allocate 2)
    // --------

    void test_5 () {
        A x;
        difference_type s = 3;
        value_type v = 2;
        pointer b = x.allocate(s);
        pointer e = b + s;
        pointer p = b;
        try {
            while(p != e){
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while(b != p){
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        if(b){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
        }
        
        difference_type s2 = 2;
        value_type v2 = 5;
        pointer b2 = x.allocate(s2);
        pointer e2 = b2 + s2;
        pointer p2 = b2;
        try {
            while(p2 != e2){
                x.construct(p2, v2);
                ++p2;
            }
        }
        catch (...){
            while(b2 != p2){
                --p2;
                x.destroy(p2);
            }
            x.deallocate(b2, s2);
            throw;
        }
        if(b2){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        }
        while(b != e){
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
        while(b2 != e2){
            --e2;
            x.destroy(e2);
        }
        x.deallocate(b2, s2);
    }
    
    // --------
    // test_6 (allocate 2, then 3, then 1, then free left and right blocks, then middle to coalesce)
    // --------

    void test_6 () {
        A x;
        difference_type s = 2;
        value_type v = 2;
        pointer b = x.allocate(s);
        pointer e = b + s;
        pointer p = b;
        try {
            while(p != e){
                x.construct(p, v);
                ++p;
            }
        }
        catch (...) {
            while(b != p){
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;
        }
        if(b){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
        }
        
        difference_type s2 = 3;
        value_type v2 = 5;
        pointer b2 = x.allocate(s2);
        pointer e2 = b2 + s2;
        pointer p2 = b2;
        try {
            while(p2 != e2){
                x.construct(p2, v2);
                ++p2;
            }
        }
        catch (...){
            while(b2 != p2){
                --p2;
                x.destroy(p2);
            }
            x.deallocate(b2, s2);
            throw;
        }
        if(b2){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        }
        
  
        difference_type s3 = 1;
        value_type v3 = 7;
        pointer b3 = x.allocate(s3);
        pointer e3 = b3 + s3;
        pointer p3 = b3;
        try {
            while(p3 != e3){
                x.construct(p3, v3);
                ++p3;
            }
        }
        catch (...){
            while(b3 != p3){
                --p3;
                x.destroy(p3);
            }
            x.deallocate(b3, s3);
            throw;
        }
        if(b3){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
            CPPUNIT_ASSERT(std::count(b3, e3, v3) == s3);
        }
        
        while(b != e){
            --e;
            x.destroy(e);
        }
        x.deallocate(b, s);
        
        CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        CPPUNIT_ASSERT(std::count(b3, e3, v3) == s3);
        
        while(b3 != e3){
            --e3;
            x.destroy(e3);
        }
        x.deallocate(b3, s3);
        
        CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        
        while(b2 != e2){
            --e2;
            x.destroy(e2);
        }
        x.deallocate(b2, s2);
    }
    
    // --------
    // test_7 (allocate 2 three times, then free, then allocate 10 to check if coalesce is successful)
    // --------

    void test_7 () {
        A x;
        difference_type s = 2;
        value_type v = 9;
        pointer b = x.allocate(s);
        pointer e = b + s;
        pointer p = b;
        try {
            while(p != e){
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while(b != p){
                --p;
                x.destroy(p);
            }
            x.deallocate(b, s);
            throw;}
        if(b){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);}
        
        difference_type s2 = 2;
        value_type v2 = 5;
        pointer b2 = x.allocate(s2);
        pointer e2 = b2 + s2;
        pointer p2 = b2;
        try {
            while(p2 != e2){
                x.construct(p2, v2);
                ++p2;}}
        catch (...){
            while(b2 != p2){
                --p2;
                x.destroy(p2);}
            x.deallocate(b2, s2);
            throw;}
        if(b2){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);}
  
        difference_type s3 = 2;
        value_type v3 = 7;
        pointer b3 = x.allocate(s3);
        pointer e3 = b3 + s3;
        pointer p3 = b3;
        try {
            while(p3 != e3){
                x.construct(p3, v3);
                ++p3;}}
        catch (...){
            while(b3 != p3){
                --p3;
                x.destroy(p3);}
            x.deallocate(b3, s3);
            throw;}
        if(b3){
            CPPUNIT_ASSERT(std::count(b, e, v) == s);
            CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
            CPPUNIT_ASSERT(std::count(b3, e3, v3) == s3);}
        
        while(b != e){
            --e;
            x.destroy(e);}
        x.deallocate(b, s);
        
        CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        CPPUNIT_ASSERT(std::count(b3, e3, v3) == s3);
        
        while(b3 != e3){
            --e3;
            x.destroy(e3);}
        x.deallocate(b3, s3);
        
        CPPUNIT_ASSERT(std::count(b2, e2, v2) == s2);
        
        while(b2 != e2){
            --e2;
            x.destroy(e2);}
        x.deallocate(b2, s2);
        
        difference_type s4 = 10;
        value_type v4 = 4;
        pointer b4 = x.allocate(s4);
        pointer e4 = b4 + s4;
        pointer p4 = b4;
        try {
            while(p4 != e4){
                x.construct(p4, v4);
                ++p4;}}
        catch (...){
            while(b4 != p4){
                --p4;
                x.destroy(p4);}
            x.deallocate(b4, s4);
            throw;}
        if(b4){
            CPPUNIT_ASSERT(std::count(b4, e4, v4) == s4);}
        
        while(b4 != e4){
            --e4;
            x.destroy(e4);}
        x.deallocate(b4, s4);}

 
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_oneMultAllocs1);
    CPPUNIT_TEST(test_oneMultAllocs2);
    CPPUNIT_TEST(test_oneMultAllocs3);
    CPPUNIT_TEST(test_oneMultAllocs4);
    CPPUNIT_TEST(test_varyingMultAllocs1);
    CPPUNIT_TEST(test_varyingMultAllocs2);
    CPPUNIT_TEST(test_varyingMultAllocs3);
    CPPUNIT_TEST(test_oneOutOfOrder1);
    CPPUNIT_TEST(test_oneOutOfOrder2);
    CPPUNIT_TEST(test_3);
    CPPUNIT_TEST(test_4);
    CPPUNIT_TEST(test_5);
    CPPUNIT_TEST(test_6);
    CPPUNIT_TEST(test_7);
    CPPUNIT_TEST_SUITE_END();};

//--------------
// InternalTests
//--------------
template <typename A>
struct InternalTests : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;


    // -------------
    // test_coalesce
    // -------------
    void test_coalesce () {
      A x;
      const difference_type s = 1;
      const value_type      v = 2;
      
      pointer p0 = x.allocate(s);
      pointer p1 = x.allocate(s);
      pointer p2 = x.allocate(s);
      pointer p3 = x.allocate(s);
      pointer p4 = x.allocate(s);

      x.construct(p0, v);
      x.construct(p1, v);
      x.construct(p2, v);
      x.construct(p3, v);
      x.construct(p4, v);

      //destroy the middle
      x.destroy(p1);
      x.destroy(p2);
      x.destroy(p3);
      
      //deallocate the middle
      x.deallocate(p1, s);
      x.deallocate(p2, s);
      x.deallocate(p3, s);

      pointer pc = x.allocate(2);
      x.construct(pc, v);
      x.construct(pc+1, v);

      //check to see if the new allocation
      //returns the pointer the pointer that
      //p1 return since it was freed.
      CPPUNIT_ASSERT(p1 == pc);}
  
 
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(InternalTests);
    CPPUNIT_TEST(test_coalesce);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;
 
    CppUnit::TextTestRunner tr;
    CppUnit::TextTestRunner ours;
    
    tr.addTest(TestAllocator< std::allocator<bool> >::suite());
    tr.addTest(TestAllocator< Allocator<bool, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<short> >::suite());
    tr.addTest(TestAllocator< Allocator<short, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    ours.addTest(InternalTests< Allocator<bool, 100> >::suite());
    
    ours.addTest(InternalTests< Allocator<short, 100> >::suite());

    ours.addTest(InternalTests< Allocator<int, 100> >::suite());

    ours.addTest(InternalTests< Allocator<double, 100> >::suite());
    
    tr.run();
    ours.run();
    cout << "Done." << endl;
    return 0;}
