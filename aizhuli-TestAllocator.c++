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
#define protected public 
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
        x.deallocate(b, s);
    }

    

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    
    CPPUNIT_TEST_SUITE_END();
};




template <typename A>
struct MyTests : CppUnit::TestFixture {
     

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // ------------
    // test valid()
    // ------------

    void test_valid_1(){
      A x;
	CPPUNIT_ASSERT(x.valid());
	const difference_type s = 20;
	if (sizeof(value_type) == 4){
             const pointer p = x.allocate(s);
	     CPPUNIT_ASSERT(x.valid());
	     x.deallocate(p, s);
	     CPPUNIT_ASSERT(x.valid());
	}
	else{
	   try{
	      x.allocate(s);
	      CPPUNIT_ASSERT(false);
	      }
	   catch(...){
	      CPPUNIT_ASSERT(true);}
	   }
	}

    void test_valid_2(){
    	A x;
	const difference_type s = 10;
	const pointer p = x.allocate(s);
	CPPUNIT_ASSERT(x.valid());
        x.deallocate(p, s);
	CPPUNIT_ASSERT(x.valid());
	}


    void test_valid_3(){
    	A x;
	const difference_type s = 5;
	const value_type  v = 2;
	const pointer p = x.allocate(s);
	x.construct(p, v);
	CPPUNIT_ASSERT(*p == v);	
	CPPUNIT_ASSERT(x.valid());
	x.destroy(p);
	x.deallocate(p, s);
	}


    //---------------
    // test_view
    //------------

    	void test_view_1 () {
           A x;
	   char temp[50];
	   x.view(*(temp+4)) = 1;
	   CPPUNIT_ASSERT(x.view(*(temp+4)) == 1);
	}

	void test_view_2 () {
	   A x;
 	   char temp[50];
	   x.view(*(temp+5)) = -2;
	   CPPUNIT_ASSERT(x.view(*(temp+5)) == -2);
	}

	void test_view_3 () {
	   A x;
	   char temp[50];
	   x.view(*temp) = -2;
	   CPPUNIT_ASSERT(x.view(*temp) == -2);
 	   x.view(*temp) = 2;
	   CPPUNIT_ASSERT(x.view(*temp) == 2);
	}

    //---------------
    // test_allocate
    //---------------

    void test_allocate_1(){
    	A x;
	const difference_type s = 10;
	const pointer p = x.allocate(s);
	int sent = x.view(x.a[0]);
	if (sizeof(value_type) == 8)
	   CPPUNIT_ASSERT(sent == -92);
	else 
	   CPPUNIT_ASSERT(sent == -40);
	CPPUNIT_ASSERT(x.valid());
	x.deallocate(p, s);
	}
    

    void test_allocate_2(){
    	A x;
    	const difference_type s = 5;
    	const pointer p = x.allocate(s);
	int sent = x.view(x.a[0]);
	if (sizeof(value_type) == 8)
 	   CPPUNIT_ASSERT(sent == -40);
	else 
	   CPPUNIT_ASSERT(sent == -20);
	CPPUNIT_ASSERT(x.valid());
	x.deallocate(p, s);
    }

    void test_allocate_3(){
    	A x;
	const difference_type s = 1;
	const pointer p = x.allocate(s);
	CPPUNIT_ASSERT(p != 0);
	x.deallocate(p, s);
	CPPUNIT_ASSERT(x.valid());
	}
   
    void test_allocate_4(){
       A x;	
       if (sizeof(value_type) == 4){
       	x.allocate(23);}
       else 
	  x.allocate(11);
      try
      {
         x.allocate(1);
	 CPPUNIT_ASSERT(false);
	}
      catch(...)
      {
	CPPUNIT_ASSERT(true);
	}
     }

     void test_allocate_5(){
        A x;
	try{
	   x.allocate(-1);
	   CPPUNIT_ASSERT(false);
	   }
	catch(...){
	   CPPUNIT_ASSERT(true);
	   }
     }

    //-----------------
    //test deallocate()
    //-----------------

    void test_deallocate_1(){
   	 A x;
    	const difference_type s = 10;
    	const pointer b = x.allocate(s);
    	x.deallocate(b);
    	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
    }
    
    void test_deallocate_2(){
    	A x;
    	const difference_type s = 5;
    	const pointer p1 = x.allocate(s);
    	const pointer p2 = x.allocate(s);
    	if (sizeof(value_type) == 4)
    	   CPPUNIT_ASSERT(x.view(x.a[96]) == 36);
    	else 
    	   CPPUNIT_ASSERT(x.view(x.a[96]) == -44);
    	x.deallocate(p1);
    	if (sizeof(value_type) == 4)
           CPPUNIT_ASSERT(x.view(x.a[96]) == 36);
    	else
    	   CPPUNIT_ASSERT(x.view(x.a[96]) == -44);
    	x.deallocate(p2);
    	CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
    }
    

    void test_deallocate_3(){
    	A x;
    	const difference_type s = 5;
    	const pointer p1 = x.allocate(s);
    	const pointer p2 = x.allocate(s);
    	x.deallocate(p2);
    	if (sizeof(value_type) == 4)
	   CPPUNIT_ASSERT(x.view(x.a[96]) == 64);
        else 
    	   CPPUNIT_ASSERT(x.view(x.a[96]) == 44);
   	 x.deallocate(p1);
   }

     void test_deallocate_4(){
        A x;
	const difference_type s = 1;
	const pointer p = 0;
	try{
	   x.deallocate(p, s);
	   CPPUNIT_ASSERT(true);}
        catch(...){
           CPPUNIT_ASSERT(false);
        }
      }

   
   //------------------
   // test constructor()
   //-------------------


   void test_constructor_1(){
   	A x;
	CPPUNIT_ASSERT(sizeof(x.a) == 100);
	}

   void test_constructor_2(){
   	A x;
	CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
	}

   void test_constructor_3(){
   	A x;
	CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
	}


    void test_constructor_4() {
        try {
            A x;
            int index = 0;
            const int first_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            index += sizeof(int);
            index += abs(first_sentinel);
            const int second_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            CPPUNIT_ASSERT(first_sentinel == second_sentinel);
        } catch(...) {
            CPPUNIT_ASSERT(true);
        }
        
    }

    

    //---------------------
    // test bad_alloc throw
    //--------------------

    void test_throw_1(){
   	 A x;
   	 const difference_type s = 25;
	 try{
	    x.allocate(s);
	    CPPUNIT_ASSERT(false);
	    }
	 catch(...){
	    CPPUNIT_ASSERT(true);
	    }
   }

    void test_throw_2(){
       A x;
       const difference_type s = 20;
       if (sizeof(value_type) == 4){
    	  x.allocate(s);
	  int index = s*sizeof(value_type) + 8;
	  CPPUNIT_ASSERT(x.view(x.a[index]) == 4);
	  }
       else{
    	 try{
	   x.allocate(s);
	   CPPUNIT_ASSERT(false);
	   }
	 catch(...){
	   CPPUNIT_ASSERT(true);
	   }}}

     void test_throw_3(){
        A x;
	const difference_type s = 23;
	if(sizeof(value_type) == 4){
	   x.allocate(s);
	   CPPUNIT_ASSERT(x.view(x.a[0]) == - 92);
	   }
	else{
	    try{
	       x.allocate(s);
	       CPPUNIT_ASSERT(false);
	    }
	    catch(...){
	    CPPUNIT_ASSERT(true);
	    }
	}}
 
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(MyTests);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_constructor_4);
    CPPUNIT_TEST(test_throw_1);
    CPPUNIT_TEST(test_throw_2);
    CPPUNIT_TEST(test_throw_3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    // ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
     tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.addTest(MyTests< Allocator<double, 100> > :: suite());
    tr.addTest(MyTests< Allocator<int, 100> > :: suite());
    tr.run();

    cout << "Done." << endl;
    return 0;}
