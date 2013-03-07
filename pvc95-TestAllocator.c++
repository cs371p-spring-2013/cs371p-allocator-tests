// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// Angela Lloyd aml2732
// Paul Carroll pvc95
// ------------------------------------

/*
To test the program:
    % ls /usr/include/cppunit/
    ...
    TestFixture.h
    ...
    % locate libcppunit.a
    /usr/lib/libcppunit.a
    % g++ -pedantic -std=c++0x -Wall Allocator.h TestAllocator.c++ -o TestAllocator -lcppunit -ldl
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

// To test private methods



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

    // ---------
    // test_none
    // ---------

    void test_none () {
        A x;										//Allocator x is declared
        const difference_type s = 0;				//
        const pointer         p = x.allocate(s);
        x.deallocate(p, s);
       }

    // --------
    // test_one
    // --------

    void test_one () {
        A x;										//Allocator x is declared
        const difference_type s = 1;				//
        const value_type      v = 2;				//Type passed in (like int/double)
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
        x.deallocate(b, s);}

    // -------------------------
    // Test Allocate Constructor
    // -------------------------

	
    void test_constructor_1(){
        try{
            Allocator<int, 1> x;
            CPPUNIT_ASSERT(false);
        }
        catch(std::bad_alloc& ba){
            //Exception should be caught
            CPPUNIT_ASSERT(true);
        }
    }

    void test_constructor_2(){
        try{
            //ints and other datatypes <=4 will alloc successfully
            Allocator<int, 12> x;
            CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            //data types larger than 4 will result in a bad_alloc
            if(sizeof(value_type)>4)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }
    }


    void test_constructor_3(){
        try{
            Allocator<int, 100> x;
            CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type)>92)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }
    }

    // --------------
    // Allocate Tests
    //---------------
    
	/**
	* Tests 1 test common cases and edge cases/perfect cases
	*/
    void test_allocate_1 () {
        A x; 
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        assert(v == 2);
        assert(p!=0);
    }

	/**
	* Tests 2-6 take any value type; but limit the size of the heap to 100
	*/
    void test_allocate_2() { //edge case

        try{
		    Allocator<value_type,100> x;
            const difference_type s = 21;

            x.allocate(s);
            if(sizeof(value_type)<4)
                CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type) > 4)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }


    }

    void test_allocate_3(){ //tries to allocate exactly
        //For an int: alloc of 23 should pass (100-92) perfectly    

        try{
		    Allocator<value_type,100> x;
            const difference_type s = 23;

            x.allocate(s);
            if(sizeof(value_type)<4)
                CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type) > 4)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }


        
    }

    void test_allocate_4(){//tries to allocate 4 more than exactly (should fail)
        //For an int: alloc of 24 should fail (100-96)
        //A x;
        try{
		    Allocator<value_type,100> x;
            const difference_type s = 24;

            x.allocate(s);
            CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type) > 3)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }
        
    }

    void test_allocate_5(){//Tries to allocate 100 at size 100 (should fail)
        //for an int: alloc of 25 should fail (100-100)


        try{
		    Allocator<value_type,100> x;
            const difference_type s = 245;

            x.allocate(s);
            CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type) > 3)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }
    }

    void test_allocate_6(){
        //for an int: alloc of 22 should pass

        try{
		    Allocator<value_type,100> x;
            const difference_type s = 22;

            x.allocate(s);
            CPPUNIT_ASSERT(true);
        }
        catch(std::bad_alloc& ba){
            if(sizeof(value_type) > 4)
                CPPUNIT_ASSERT(true);
            else
                CPPUNIT_ASSERT(false);
        }

    }

	/**
	 * 7 AND 8 test specific heaps with predefined value_types and sizes
	*/
    void test_allocate_7(){
        //for more than 1 alloc
         Allocator<int, 100> x; 
        const difference_type s = 5;
         x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[96] == 64);
		CPPUNIT_ASSERT((int)x.a[28] == 64);
		CPPUNIT_ASSERT((int)x.a[0] == -20);
        x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[0] == -20);
		CPPUNIT_ASSERT((int)x.a[96] == 36);
		CPPUNIT_ASSERT((int)x.a[56] == 36);
		CPPUNIT_ASSERT((int)x.a[28] == -20);

    }

    void test_allocate_8(){
        //for more than 1 alloc
         Allocator<double, 100> x; 
        const difference_type s = 5;
         x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[96] == 44);
		CPPUNIT_ASSERT((int)x.a[48] == 44);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
        x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
		CPPUNIT_ASSERT((int)x.a[96] == -44);
		CPPUNIT_ASSERT((int)x.a[48] == -44);

    }

    void test_allocate_9(){
        //for more than 1 alloc
         Allocator<double, 100> x; 
        const difference_type s = 5;
         x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[96] == 44);
		CPPUNIT_ASSERT((int)x.a[48] == 44);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
        x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
		CPPUNIT_ASSERT((int)x.a[96] == -44);
		CPPUNIT_ASSERT((int)x.a[48] == -44);
		CPPUNIT_ASSERT(x.allocate(s)==0);


    }

    // ---------------
    // test_deallocate
    // ---------------

	// single unallocated block
    void test_deallocate_1 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = 92;
		x.view(x.a[96]) = 92;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[sizeof(int)], 0);
		CPPUNIT_ASSERT(x.a[0] == 92);
		CPPUNIT_ASSERT(x.a[96] == 92);
    }

	// single allocated block
    void test_deallocate_2 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = -92;
		x.view(x.a[96]) = -92;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[sizeof(int)], 0);
		CPPUNIT_ASSERT(x.a[0] == 92);
		CPPUNIT_ASSERT(x.a[96] == 92);
    }

	// free block to right
    void test_deallocate_3 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = -10;
		x.view(x.a[14]) = -10;
		x.view(x.a[18]) = 74;
		x.view(x.a[96]) = 74;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[4], 0);
		CPPUNIT_ASSERT(x.a[0] == 92);
		CPPUNIT_ASSERT(x.a[96] == 92);
    }

	// free block to left
    void test_deallocate_4 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = 10;
		x.view(x.a[14]) = 10;
		x.view(x.a[18]) = -74;
		x.view(x.a[96]) = -74;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[22], 0);
		CPPUNIT_ASSERT(x.a[0] == 92);
		CPPUNIT_ASSERT(x.a[96] == 92);
    }

	// free block to left, allocated block to right
    void test_deallocate_5 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = 20;
		x.view(x.a[24]) = 20;
		x.view(x.a[28]) = -20;
		x.view(x.a[52]) = -20;
		x.view(x.a[56]) = -36;
		x.view(x.a[96]) = -36;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[32], 0);
		CPPUNIT_ASSERT(x.a[0] == 48);
		CPPUNIT_ASSERT(x.a[52] == 48);
		CPPUNIT_ASSERT(x.a[56] == -36);
		CPPUNIT_ASSERT(x.a[96] == -36);
    }

	// allocated block to left, free block to right
    void test_deallocate_6 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = -20;
		x.view(x.a[24]) = -20;
		x.view(x.a[28]) = -20;
		x.view(x.a[52]) = -20;
		x.view(x.a[56]) = 36;
		x.view(x.a[96]) = 36;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[32], 0);
		CPPUNIT_ASSERT(x.a[0] == -20);
		CPPUNIT_ASSERT(x.a[24] == -20);
		CPPUNIT_ASSERT(x.a[28] == 64);
		CPPUNIT_ASSERT(x.a[96] == 64);
    }

	// free block to left, free block to right
    void test_deallocate_7 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = 20;
		x.view(x.a[24]) = 20;
		x.view(x.a[28]) = -20;
		x.view(x.a[52]) = -20;
		x.view(x.a[56]) = 36;
		x.view(x.a[96]) = 36;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[32], 0);
		CPPUNIT_ASSERT(x.a[0] == 92);
		CPPUNIT_ASSERT(x.a[96] == 92);
    }

	// allocated block to left, allocated block to right
    void test_deallocate_8 () {
		Allocator<int, 100> x; 
		x.view(x.a[0]) = -20;
		x.view(x.a[24]) = -20;
		x.view(x.a[28]) = -20;
		x.view(x.a[52]) = -20;
		x.view(x.a[56]) = -36;
		x.view(x.a[96]) = -36;
        CPPUNIT_ASSERT(x.valid());
        x.deallocate((int*) &x.a[32], 0);
		CPPUNIT_ASSERT(x.a[0] == -20);
		CPPUNIT_ASSERT(x.a[24] == -20);
		CPPUNIT_ASSERT(x.a[28] == 20);
		CPPUNIT_ASSERT(x.a[52] == 20);
		CPPUNIT_ASSERT(x.a[56] == -36);
		CPPUNIT_ASSERT(x.a[96] == -36);
    }
	

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_none);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);
    CPPUNIT_TEST(test_allocate_6);
    CPPUNIT_TEST(test_allocate_7);
    CPPUNIT_TEST(test_allocate_8);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST(test_deallocate_6);
    CPPUNIT_TEST(test_deallocate_7);
    CPPUNIT_TEST(test_deallocate_8);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST_SUITE_END();};




template <typename A>
struct TestAllocator2 : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

	void test_valid_0(){
		Allocator<int, 100> x; 
		CPPUNIT_ASSERT((int)x.a[0] == 92);
		CPPUNIT_ASSERT((int)x.a[96] == 92);
		CPPUNIT_ASSERT(x.valid());

	}

	void test_valid_1(){
		Allocator<int, 100> x; 
        const difference_type s = 5;
         x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[96] == 64);
		CPPUNIT_ASSERT((int)x.a[28] == 64);
		CPPUNIT_ASSERT((int)x.a[0] == -20);
        x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[0] == -20);
		CPPUNIT_ASSERT((int)x.a[96] == 36);
		CPPUNIT_ASSERT((int)x.a[56] == 36);
		CPPUNIT_ASSERT((int)x.a[28] == -20);

		assert(x.valid());

	}

    void test_valid_2(){
        //for more than 1 alloc
         Allocator<double, 100> x; 
        const difference_type s = 5;
         x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[96] == 44);
		CPPUNIT_ASSERT((int)x.a[48] == 44);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
        x.allocate(s);
		CPPUNIT_ASSERT((int)x.a[0] == -40);
		CPPUNIT_ASSERT((int)x.a[96] == -44);
		CPPUNIT_ASSERT((int)x.a[48] == -44);
		CPPUNIT_ASSERT(x.valid());

    }


    CPPUNIT_TEST_SUITE(TestAllocator2); 

	CPPUNIT_TEST(test_valid_0);
	CPPUNIT_TEST(test_valid_1);
	CPPUNIT_TEST(test_valid_2);

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
    tr.run();


	CppUnit::TextTestRunner tr2;
	  tr2.addTest(TestAllocator2< Allocator<int, 100> >::suite()); // uncomment!
      tr2.addTest(TestAllocator2< Allocator<double, 100> >::suite()); // uncomment!
	tr2.run();

    cout << "Done." << endl;
    return 0;}
