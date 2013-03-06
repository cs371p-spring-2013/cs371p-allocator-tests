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
#include <string>

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

#define private public
#define protected public
#define class struct

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

    public:
    //------------------
    // getTypeSize Tests
    //------------------
    
    void test_getTypeSize_1 (){
        Allocator<int, 200> a;
        CPPUNIT_ASSERT (sizeof(int) == a.getTypeSize());
    }
    //test 2 for getTypeSize
    void test_getTypeSize_2 (){
        Allocator<char, 200> b;
        CPPUNIT_ASSERT (sizeof(char) == b.getTypeSize());
    }
    //test 3 for getTypeSize
    void test_getTypeSize_3 (){
        Allocator<std::string, 200> c;
        CPPUNIT_ASSERT (sizeof(std::string) == c.getTypeSize());
    }
    
    // -------------------
    // getMemorySize Tests
    // -------------------
    
    void test_getMemorySize_1 (){
        Allocator<int, 200> a;
        CPPUNIT_ASSERT (200 == a.getMemorySize());
    }
    //test 2 for getTypeSize
    void test_getMemorySize_2 (){
        Allocator<char, 300> b;
        CPPUNIT_ASSERT (300 == b.getMemorySize());
    }
    //test 3 for getTypeSize
    void test_getMemorySize_3 (){
        Allocator<std::string, 400> c;
        CPPUNIT_ASSERT (400 == c.getMemorySize());
    }
    
    // ----------
    // View Tests
    // ----------
    
    void test_view_1 (){
        Allocator<int, 20> E;
        *reinterpret_cast<int*>(&E.a[0]) = 4;
        CPPUNIT_ASSERT(E.view(E.a[0]) == 4);
    }
    
    void test_view_2 (){
        Allocator<int, 20> E;
        *reinterpret_cast<int*>(&E.a[10]) = 16;
        CPPUNIT_ASSERT(E.view(E.a[10]) == 16);
    }

    void test_view_3 (){
        Allocator<int, 20> E;
        *reinterpret_cast<int*>(&E.a[2]) = 32;
        CPPUNIT_ASSERT(E.view(E.a[2]) == 32);
    }
    
    // -----------
    // Write Tests
    // -----------
    
    void test_write_1 (){
        Allocator<int, 20> E;
        E.write(E.a[0], 4);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 4);
    }
    
    void test_write_2 (){
        Allocator<int, 20> E;
        E.write(E.a[10], 16);
        CPPUNIT_ASSERT(E.view(E.a[10]) == 16);
    }

    void test_write_3 (){
        Allocator<int, 20> E;
        E.write(E.a[2], 32);
        CPPUNIT_ASSERT(E.view(E.a[2]) == 32);
    }
    
    //----------------
    //test Constructor
    //----------------
    
    void test_Constructor_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
    }
    
    void test_Constructor_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
    }

    void test_Constructor_3 (){
        Allocator<char, 10> E;
        CPPUNIT_ASSERT (10 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 2);
        CPPUNIT_ASSERT(E.view(E.a[6]) == 2);
    }
    
    //-------------------
    //test valid
    //-------------------
    
    void test_valid_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
    }
    
    void test_valid_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
        E.write(E.a[0], -10);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -10);
        CPPUNIT_ASSERT(E.valid() == false);
        
    }

    void test_valid_3 (){
        Allocator<char, 10> E;
        CPPUNIT_ASSERT (10 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 2);
        CPPUNIT_ASSERT(E.view(E.a[6]) == 2);
        E.write(E.a[6], 3);
        CPPUNIT_ASSERT(E.view(E.a[6]) == 3);
        CPPUNIT_ASSERT(E.valid() == false);
    }
    
    //-------------------
    //test allocate
    //-------------------
    
    void test_allocate_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocate(1); //will allocate all in this case as there will not be room for another block size T + 2*sizeof(int)
        CPPUNIT_ASSERT(E.view(E.a[0]) == -12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == -12);
        CPPUNIT_ASSERT(E.valid() == true);
        try{
                E.allocate(1);
        //memory array is full. Cannot allocate more
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT (e.what() != 0);
        }
        
    }
    
    void test_allocate_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocate(3);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[28]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[32]) == 160);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 160);
        CPPUNIT_ASSERT(E.valid() == true);
    }

    void test_allocate_3 (){
        Allocator<char, 100> E;
        CPPUNIT_ASSERT (100 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        E.allocate(10);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -10);
        CPPUNIT_ASSERT(E.view(E.a[14]) == -10);
        E.allocate(5);
        CPPUNIT_ASSERT(E.view(E.a[18]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[27]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[31]) == 61);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 61);
        CPPUNIT_ASSERT(E.valid() == true);
        try{
                E.allocate(0);
        //cannot allocate for 0 items
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT (e.what() != 0);
        }
    }
    
    //--------------
    //test findRoom
    //--------------
    
    void test_findRoom_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocate(1); //will allocate all in this case as there will not be room for another block size T + 2*sizeof(int)
        CPPUNIT_ASSERT(E.view(E.a[0]) == -12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == -12);
        CPPUNIT_ASSERT(E.valid() == true);
        int i = E.findRoom(1);
        //there is no more room in the memory array so findRoom will return -1
        CPPUNIT_ASSERT(i == -1);
        
    }
    
    void test_findRoom_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocate(3);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[28]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[32]) == 160);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 160);
        CPPUNIT_ASSERT(E.valid() == true);
        int i = E.findRoom(30);
        CPPUNIT_ASSERT(i == 32);
    }

    void test_findRoom_3 (){
        Allocator<char, 100> E;
        CPPUNIT_ASSERT (100 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        E.allocate(10);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -10);
        CPPUNIT_ASSERT(E.view(E.a[14]) == -10);
        E.allocate(5);
        CPPUNIT_ASSERT(E.view(E.a[18]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[27]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[31]) == 61);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 61);
        CPPUNIT_ASSERT(E.valid() == true);
        int i = E.findRoom(10);
        CPPUNIT_ASSERT(i == 31);
        i = E.findRoom(61);
        CPPUNIT_ASSERT(i == 31);
        i = E.findRoom(100);
        CPPUNIT_ASSERT(i == -1);
    }
    
    //--------------
    //test allocateAll
    //--------------
    
    void test_allocateAll_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocateAll(3, 0); //will allocate all in this case as there will not be room for another block size T + 2*sizeof(int)
        CPPUNIT_ASSERT(E.view(E.a[0]) == -12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == -12);
        CPPUNIT_ASSERT(E.valid() == true);
        
    }
    
    void test_allocateAll_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
        E.allocateAll(25, 0);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == -192);
        CPPUNIT_ASSERT(E.valid() == true);

    }

    void test_allocateAll_3 (){
        Allocator<char, 100> E;
        CPPUNIT_ASSERT (100 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        E.allocateAll(92, 0);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == -92);
        CPPUNIT_ASSERT(E.valid() == true);
        
    }
    
    //-------------------
    //test deallocate
    //-------------------
    
    void test_deallocate_1 (){
        Allocator<int, 20> E;
        CPPUNIT_ASSERT (20 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(int) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
        int* p = E.allocate(1); //will allocate all in this case as there will not be room for another block size T + 2*sizeof(int)
        CPPUNIT_ASSERT(E.view(E.a[0]) == -12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == -12);
        CPPUNIT_ASSERT(E.valid() == true);
        try{
                E.allocate(1);
        //memory array is full. Cannot allocate more
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT (e.what() != 0);
        }
        E.deallocate(p);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 12);
        CPPUNIT_ASSERT(E.view(E.a[16]) == 12);
        CPPUNIT_ASSERT(E.valid() == true);
    
        
    }
    
    void test_deallocate_2 (){
        Allocator<double, 200> E;
        CPPUNIT_ASSERT (200 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(double) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
        double* p = E.allocate(3);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[28]) == -24);
        CPPUNIT_ASSERT(E.view(E.a[32]) == 160);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 160);
        CPPUNIT_ASSERT(E.valid() == true);
        E.deallocate(p);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
    }

    void test_deallocate_3 (){
        Allocator<char, 100> E;
        CPPUNIT_ASSERT (100 == E.getMemorySize());
        CPPUNIT_ASSERT (sizeof(char) == E.getTypeSize());
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        char* p = E.allocate(10);
        CPPUNIT_ASSERT(E.view(E.a[0]) == -10);
        CPPUNIT_ASSERT(E.view(E.a[14]) == -10);
        char* pp = E.allocate(5);
        CPPUNIT_ASSERT(E.view(E.a[18]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[27]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[31]) == 61);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 61);
        CPPUNIT_ASSERT(E.valid() == true);
        try{
                E.allocate(0);
        //cannot allocate for 0 items
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT (e.what() != 0);
        }
        E.deallocate(p);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 10);
        CPPUNIT_ASSERT(E.view(E.a[14]) == 10);
        CPPUNIT_ASSERT(E.view(E.a[18]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[27]) == -5);
        CPPUNIT_ASSERT(E.view(E.a[31]) == 61);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 61);
        E.deallocate(pp);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        CPPUNIT_ASSERT(E.valid() == true);
    }
    
    
    //-------------------
    // test allocator
    //-------------------
    
    void test_allocator_1 () {
        
        const difference_type s = 10;
        const value_type      v = 2;
        Allocator<value_type, 200> E;
        const pointer         b = E.allocate(s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == ((signed)-(s * sizeof(value_type))));
        CPPUNIT_ASSERT(E.view(E.a[0 + sizeof(int) + (s * sizeof(value_type))]) ==  ((signed)-(s * sizeof(value_type))));
        CPPUNIT_ASSERT(E.valid() == true);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                E.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                E.destroy(p);
            }
            E.deallocate(b, s);
            throw;
        }
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            E.destroy(e);
        }
        E.deallocate(b, s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 192);
        CPPUNIT_ASSERT(E.view(E.a[196]) == 192);
        CPPUNIT_ASSERT(E.valid() == true);
    }
    
    void test_allocator_2 () {
        
        const difference_type s = 20;
        const value_type      v = 33;
        Allocator<value_type, 2000> E;
        const pointer         b = E.allocate(s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == ((signed)-(s * sizeof(value_type))) );
        CPPUNIT_ASSERT(E.view(E.a[0 + sizeof(int) + (s * sizeof(value_type))]) ==  ((signed)-(s * sizeof(value_type))) );
        CPPUNIT_ASSERT(E.valid() == true);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                E.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                E.destroy(p);
            }
            E.deallocate(b, s);
            throw;
        }
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            E.destroy(e);
        }
        E.deallocate(b, s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 1992);
        CPPUNIT_ASSERT(E.view(E.a[1996]) == 1992);
        CPPUNIT_ASSERT(E.valid() == true);
    }
    
    void test_allocator_3 () {
        
        const difference_type s = 2;
        const value_type      v = 2;
        Allocator<value_type, 100> E;
        const pointer         b = E.allocate(s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == ((signed)-(s * sizeof(value_type))) );
        CPPUNIT_ASSERT(E.view(E.a[0 + sizeof(int) + (s * sizeof(value_type))]) ==  ((signed)-(s * sizeof(value_type))) );
        CPPUNIT_ASSERT(E.valid() == true);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                E.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                E.destroy(p);
            }
            E.deallocate(b, s);
            throw;
        }
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            E.destroy(e);
        }
        E.deallocate(b, s);
        CPPUNIT_ASSERT(E.view(E.a[0]) == 92);
        CPPUNIT_ASSERT(E.view(E.a[96]) == 92);
        CPPUNIT_ASSERT(E.valid() == true);
    }
    
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
    CPPUNIT_TEST(test_getTypeSize_1);
    CPPUNIT_TEST(test_getTypeSize_2);
    CPPUNIT_TEST(test_getTypeSize_3);
    CPPUNIT_TEST(test_getMemorySize_1);
    CPPUNIT_TEST(test_getMemorySize_2);
    CPPUNIT_TEST(test_getMemorySize_3);
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_write_1);
    CPPUNIT_TEST(test_write_2);
    CPPUNIT_TEST(test_write_3);
    CPPUNIT_TEST(test_Constructor_1);
    CPPUNIT_TEST(test_Constructor_2);
    CPPUNIT_TEST(test_Constructor_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_findRoom_1);
    CPPUNIT_TEST(test_findRoom_2);
    CPPUNIT_TEST(test_findRoom_3);
    CPPUNIT_TEST(test_allocateAll_1);
    CPPUNIT_TEST(test_allocateAll_2);
    CPPUNIT_TEST(test_allocateAll_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_allocator_1);
    CPPUNIT_TEST(test_allocator_2);
    CPPUNIT_TEST(test_allocator_3);
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

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
//  tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
//  tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
