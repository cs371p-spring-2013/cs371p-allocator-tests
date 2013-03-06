// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------


// ------------------------------------
//    Name: Faisal Ejaz, EID: fe2333
//    Name: Jeffrey Li, EID: JL37656
//    Project 3, Allocator
//    CS 371P - Professor Downing
//    TestAllocator.c++
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

#include "Allocator.h"


#define private public
#define protected public
#define class struct

// -------------
// TestStdAllocator
// -------------

template <typename A>
struct TestStdAllocator : CppUnit::TestFixture {

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
    // test_one
    // Tests basic allocation of 3 valid blocks 
    // --------
    void test_two () {            
        A x;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;

        const difference_type s1 = 1;
        const difference_type s2 = 3;
        const difference_type s3 = 5;
        
        try{
            const pointer         p1 = x.allocate(s1);
            const pointer         p2 = x.allocate(s2);
            const pointer         p3 = x.allocate(s3);
            x.construct(p1, v1);
            CPPUNIT_ASSERT(*p1 == v1);

            x.construct(p2, v2);
            CPPUNIT_ASSERT(*p2 == v2);

            x.construct(p3, v3);
            CPPUNIT_ASSERT(*p3 == v3);

             x.deallocate(p1, s1);
            
            x.deallocate(p2, s2);

            x.deallocate(p3, s3);
        }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(false);} 
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

        assert(sizeof(double) == 8);
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

    CPPUNIT_TEST_SUITE(TestStdAllocator);

    //Downing's test
    CPPUNIT_TEST(test_one);

    CPPUNIT_TEST(test_two);

    //Downing's test
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};


// -------------
// TestStdAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // -----
    // test_constructor
    // Test default constructor of allocator, and simple allocate and deallocate
    // -----  
    void test_constructor () {
            
            A x;
            CPPUNIT_ASSERT(x.num_sentinels == 2);
            const difference_type s = 1;
            const pointer         p = x.allocate(s);
            CPPUNIT_ASSERT(x.num_sentinels == 4);
            x.deallocate(p, s);
            CPPUNIT_ASSERT(x.num_sentinels == 2);
    }

    // -----
    // test_allocate_1
    // Tests basic allocation of 3 blocks with bad allocations
    // ----- 
    void test_allocate_1 () {
        A x;
        const value_type      v1 = 1;
        const value_type      v2 = 2;
        const value_type      v3 = 3;

        const difference_type s1 = 1;
        const difference_type s2 = 10;
        const difference_type s3 = x.getN();      

        try{

            const pointer         p1 = x.allocate(s1);
            const pointer         p2 = x.allocate(s2);
            const pointer         p3 = x.allocate(s3);
            x.construct(p1, v1);
            CPPUNIT_ASSERT(*p1 == v1);
            CPPUNIT_ASSERT(x.isValid());

            x.construct(p2, v2);
            CPPUNIT_ASSERT(*p2 == v2);
            CPPUNIT_ASSERT(x.isValid());

            x.construct(p3, v3);
            CPPUNIT_ASSERT(*p3 == v3);
            CPPUNIT_ASSERT(x.isValid());

             x.deallocate(p1, s1);
            CPPUNIT_ASSERT(x.isValid());
            
            x.deallocate(p2, s2);
            CPPUNIT_ASSERT(x.isValid());

            x.deallocate(p3, s3);
            CPPUNIT_ASSERT(x.num_sentinels == 2);
            CPPUNIT_ASSERT(x.isValid());
            CPPUNIT_ASSERT(false);
        }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(x.valid());}}
    // -----
    // test_allocate_2
    // Tests 1 block which is == size of entire block
    // ----- 
    void test_allocate_2 () {
        A x;
        const difference_type s1 = x.getN();

        try{
            x.allocate(s1);
            CPPUNIT_ASSERT(false); }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(x.num_sentinels == 2);} 
    }

    // -----
    // test_allocate_3
    // Tests 1 block which is size 0, throws exception
    // ----- 
    void test_allocate_3 () {
        A x;
        const difference_type s1 = 0;
        try{
            x.allocate(s1);
            CPPUNIT_ASSERT(false); }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(x.num_sentinels == 2);}
    }


    // -----
    // test_deallocate_1
    // Tests deallocate with normal 3 blocks
    // ----- 
    void test_deallocate_1 () {  
        A x;
        const value_type      v = 2;
        const difference_type s1 = 1;

        try{
            const pointer     p1 = x.allocate(s1);
            x.construct(p1, v);

            const pointer     p2 = x.allocate(s1);
            x.construct(p2, v);

            const pointer     p3 = x.allocate(s1);
            x.construct(p3, v);

            CPPUNIT_ASSERT(x.isValid());
            CPPUNIT_ASSERT(*p1 == v);
            CPPUNIT_ASSERT(*p2 == v);
            CPPUNIT_ASSERT(*p3 == v);
            x.deallocate(p1);
            x.deallocate(p2);
            x.deallocate(p3);
            CPPUNIT_ASSERT(x.isValid());
            CPPUNIT_ASSERT(x.num_sentinels == 2); }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(false); }
    }


    // -----
    // test_deallocate_2
    // Tests deallocate with 3 blocks, with no merging
    // ----- 
    void test_deallocate_2 () {       
        A x;
        const value_type      v = 2;
        try{
            const pointer     p1 = x.allocate(1);
            x.construct(p1, v);

            const pointer     p2 = x.allocate(3);
            x.construct(p2, v);

            const pointer     p3 = x.allocate(5);
            x.construct(p3, v);

            CPPUNIT_ASSERT(x.isValid());
            CPPUNIT_ASSERT(*p1 == v);
            CPPUNIT_ASSERT(*p2 == v);
            CPPUNIT_ASSERT(*p3 == v);

            int num_s = x.num_sentinels;
            x.deallocate(p2);
            CPPUNIT_ASSERT(num_s == x.num_sentinels);
            x.deallocate(p3);
            CPPUNIT_ASSERT(x.num_sentinels == 4);
            x.deallocate(p1);
            CPPUNIT_ASSERT(x.num_sentinels == 2);
            CPPUNIT_ASSERT(x.isValid());
        }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(false);
        }
    }

    // -----
    // test_deallocate_3
    // Tests deallocate with 3 blocks and with merging 
    // ----- 
    void test_deallocate_3 () {       
        A x;
        const value_type      v = 2;
        try{
            const pointer     p1 = x.allocate(1);
            x.construct(p1, v);

            const pointer     p2 = x.allocate(3);
            x.construct(p2, v);

            const pointer     p3 = x.allocate(5);
            x.construct(p3, v);

            CPPUNIT_ASSERT(x.isValid());
            CPPUNIT_ASSERT(*p1 == v);
            CPPUNIT_ASSERT(*p2 == v);
            CPPUNIT_ASSERT(*p3 == v);

            int num_s = x.num_sentinels;
            x.deallocate(p1);
            x.deallocate(p2);
            CPPUNIT_ASSERT(num_s-2 == x.num_sentinels);
            x.deallocate(p3);
            CPPUNIT_ASSERT(x.num_sentinels == 2);
            CPPUNIT_ASSERT(x.isValid());
        }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(false);
        }
    }

    // -----
    // test_second_sentinel_index_1
    // Tests second_sentinel_index with default constructor
    // [N - sizeof(int)] ---- [N - sizeof(int)]
    // ----- 
    void test_second_sentinel_index_1(){
        A x;
        CPPUNIT_ASSERT(x.isValid());
        CPPUNIT_ASSERT(x.second_sentinel_index(0) == x.getN()-4 );
    }

    // -----
    // test_second_sentinel_index_2
    // Tests second_sentinel_index with 1 block allocation
    // ----- 
    void test_second_sentinel_index_2(){
        A x;
        x.allocate(5);
        CPPUNIT_ASSERT(x.isValid());
        CPPUNIT_ASSERT(x.second_sentinel_index(0) == 5*x.getTypeSize()+4 );
    }

    // -----
    // test_second_sentinel_index_3
    // Tests second_sentinel_index with 2 block allocation
    // -----
    void test_second_sentinel_index_3(){
        A x;
        const value_type v = 1;
        const pointer p = x.allocate(1);
        const pointer p2 = x.allocate(1);
        x.construct(p,v);
        x.construct(p2,v);
        CPPUNIT_ASSERT(*p == v);
        CPPUNIT_ASSERT(*p2 == v);
        CPPUNIT_ASSERT(x.isValid());
        CPPUNIT_ASSERT(x.second_sentinel_index(0) == x.getTypeSize()+4);
    }

    // -----
    // test_valid_1
    // Tests valid with default constructor
    // -----
    void test_valid_1(){
        //Tests second_sentinel_index after allocating a block
        A x;
        CPPUNIT_ASSERT(x.isValid() == true);
    }

    // -----
    // test_valid_2
    // Tests second_sentinel_index after allocating a block too big
    // -----
    void test_valid_2(){
        A x;
        try{
            x.allocate(x.getN()); 
            CPPUNIT_ASSERT(false);
        }
        catch(std::bad_alloc& ba){
            CPPUNIT_ASSERT(x.isValid() == true);
        }

    }

    // -----
    // suite
    // -----
    CPPUNIT_TEST_SUITE(TestAllocator);
    
    CPPUNIT_TEST(test_constructor);

    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);

    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);

    CPPUNIT_TEST(test_second_sentinel_index_1);
    CPPUNIT_TEST(test_second_sentinel_index_2);
    CPPUNIT_TEST(test_second_sentinel_index_3);

    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);

    CPPUNIT_TEST_SUITE_END();};




// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    //Test C++ Allocator along with built Allocator
    tr.addTest(TestStdAllocator< std::allocator<int> >::suite());
    tr.addTest(TestStdAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<int> >::suite());
    tr.addTest(TestStdAllocator< Allocator<int, 1000> >::suite());  

    tr.addTest(TestStdAllocator< std::allocator<double> >::suite());
    tr.addTest(TestStdAllocator< Allocator<double, 100> >::suite());    

    tr.addTest(TestStdAllocator< std::allocator<char> >::suite());
    tr.addTest(TestStdAllocator< Allocator<char, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<long> >::suite());
    tr.addTest(TestStdAllocator< Allocator<long, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<long> >::suite());
    tr.addTest(TestStdAllocator< Allocator<long, 1000> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<long long> >::suite());
    tr.addTest(TestStdAllocator< Allocator<long long, 100> >::suite()); 
   
    tr.addTest(TestStdAllocator< std::allocator<double> >::suite());
    tr.addTest(TestStdAllocator< Allocator<double, 100> >::suite());
    //End of C++ Allocator and Allocator.h testing


    //Testing Allocator.h and written functions
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestStdAllocator< Allocator<int, 1000> >::suite());

    tr.addTest(TestAllocator< Allocator<char, 100> >::suite());
    tr.addTest(TestStdAllocator< Allocator<char, 50> >::suite());

    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestStdAllocator< Allocator<double, 1000> >::suite());

    tr.addTest(TestAllocator< Allocator<long, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<long, 1000> >::suite());

    tr.addTest(TestAllocator< Allocator<long long, 100> >::suite());
    tr.addTest(TestStdAllocator< Allocator<long long, 1000> >::suite());
    //End of testing Allocator.h

    tr.run();

    cout << "Done." << endl;
    return 0;}
