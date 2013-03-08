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

/* Cameron Rison
 * CSID: 
 * eId: 
*/

/* Christopher Word
 * CSID: word
 * eId: cword
 */

// --------
// includes
// --------


#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator
#include <cstdlib>
#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define private public
#define protected public
#define class struct 

#include "Allocator.h"

//        a version of valid for testing
        bool static t_valid (char in[], int size){
            //Halt once first goes out of range of array
            int first = 0;
            //Loop until we've checked pairwise all sentinels 
            while(first < size){
                int current = abs(ctoi(in[first]));
                int last = sizeof(int) + first + current;
                //If the pair sentinel values aren't equal short circuit
                if(ctoi(in[first]) == 0 ){
                    return false;
                }
                if(ctoi(in[first]) != ctoi(in[last])){
                   return false;
                }
                first = last + sizeof(int);
            }
            //If we make all the way through the while and all sentinel values 
            //we good 
            return true;
        }



// -------------
// TestAllocator
// -------------



template <typename A>
struct OurAllocator : CppUnit::TestFixture {


    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // int& ctoi (char& c) {
    //     return *reinterpret_cast<int*>(&c);}


// 3 for allocate

    void test_allocate_one () {
        A x;
        const difference_type s = 3;
        const value_type      v = 2;

        const pointer         p = x.allocate(s);
        pointer g = p;
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.construct(++g, v);
        CPPUNIT_ASSERT(*(g) == v);
        x.construct(++g, v);
        CPPUNIT_ASSERT(*(g) == v);

        x.destroy(p);

        x.deallocate(p, s);

    }

    void test_allocate_two () {

        A x;
        const difference_type s = 4;
        const value_type      v = 3;

        const pointer         p = x.allocate(s);
        pointer g = p;
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.construct(++g, v);
        CPPUNIT_ASSERT(*(g) == v);
        x.construct(++g, v);
        CPPUNIT_ASSERT(*(g) == v);
        x.construct(++g,v);
        CPPUNIT_ASSERT(*(g) == v);

        x.destroy(p);

        x.deallocate(p, s);

    }

    void test_allocate_three(){
       A x;
        const difference_type s = 2;
        const value_type      v = 1;

        const pointer         p = x.allocate(s);
        pointer g = p;
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.construct(++g, v);
        CPPUNIT_ASSERT(*(g) == v);

        x.destroy(p);

        x.deallocate(p, s);
    }


// 3 for deallocate
    void test_deallocate_one () {
        A x;
        const difference_type s= 1;
        const value_type v = 2;
        const pointer pOne = x.allocate(s);
        const pointer pTwo = x.allocate(s);
        const pointer pThree = x.allocate(s);
        x.construct(pOne, v);
        x.construct(pTwo, v);
        x.construct(pThree, v);
        x.deallocate(pOne, v);
        x.deallocate(pTwo, v);
        x.deallocate(pThree, v);

    }

    void test_deallocate_two () {

        A x;
        const difference_type s = 3;
        const value_type      v1 = 7;
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
    
    void test_deallocate_three () {


       
        A x;
        const difference_type s = 5;
        const value_type      v1 = 7;
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









    // -----
    // suite
    // -----



    CPPUNIT_TEST_SUITE(OurAllocator);


    CPPUNIT_TEST(test_allocate_one);
    CPPUNIT_TEST(test_allocate_two);
    CPPUNIT_TEST(test_allocate_three);

    CPPUNIT_TEST(test_deallocate_one);
    CPPUNIT_TEST(test_deallocate_two);
    CPPUNIT_TEST(test_deallocate_three);


    CPPUNIT_TEST_SUITE_END();};



template <typename A>
struct TestAllocator : CppUnit::TestFixture {


    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // int& ctoi (char& c) {
    //     return *reinterpret_cast<int*>(&c);}

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

//3 for valid

    // --------
    // test_valid_one
    // --------
    void test_1_valid(){
    char arr [30] ={0};
/* an x = 1 free byte

  |_-1_|x|_-1_|_-2_|xx|_-2_|__3_|xxx|__3_|
*/


    ctoi(arr[0]) = -1;
    ctoi(arr[5]) = -1;
    ctoi(arr[9]) = -2;
    ctoi(arr[15]) = -2;
    ctoi(arr[19]) = 3;
    ctoi(arr[26]) = 3;

    CPPUNIT_ASSERT(t_valid(arr, 30));

    }

    // --------
    // test_valid_two
    // --------
    void test_2_valid(){
    char arr [30] ={0};

    ctoi(arr[0]) = -1;
    ctoi(arr[5]) = -1;
    ctoi(arr[9]) = -2;
    ctoi(arr[15]) = 2;
    ctoi(arr[19]) = 3;
    ctoi(arr[26]) = 3;

    CPPUNIT_ASSERT(!t_valid(arr, 30));

    }

    // --------
    // test_valid_three
    // --------
    void test_3_valid(){
    char arr [30] ={0};

    ctoi(arr[0]) = -1;
    ctoi(arr[5]) = -1;
    ctoi(arr[9]) = -2;
    ctoi(arr[15]) = -2;
    ctoi(arr[19]) = 3;
    ctoi(arr[25]) = 3;

    CPPUNIT_ASSERT(!t_valid(arr, 30));

    }

//3 for constructor

    void test_constructor_one(){

        Allocator<int, 108> x;

        CPPUNIT_ASSERT(ctoi(x.a[0]) == 100 );
        CPPUNIT_ASSERT(ctoi(x.a[104]) == 100);

    }

    void test_constructor_two(){

        Allocator<int, 30> x;

        CPPUNIT_ASSERT(ctoi(x.a[0]) == 22 );
        CPPUNIT_ASSERT(ctoi(x.a[26]) == 22);

    }

    void test_constructor_three(){

        Allocator<int, 12> x;

        CPPUNIT_ASSERT(ctoi(x.a[0]) == 4);
        CPPUNIT_ASSERT(ctoi(x.a[8]) == 4);

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
        x.deallocate(b, s);}


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);

    CPPUNIT_TEST(test_constructor_one);
    CPPUNIT_TEST(test_constructor_two);
    CPPUNIT_TEST(test_constructor_three);

    CPPUNIT_TEST(test_1_valid);
    CPPUNIT_TEST(test_2_valid);
    CPPUNIT_TEST(test_3_valid);

    CPPUNIT_TEST_SUITE_END();};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    

    tr.addTest(OurAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.addTest(OurAllocator< Allocator<int, 100> >::suite()); // uncomment!


    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
