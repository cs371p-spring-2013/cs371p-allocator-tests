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
#include <typeinfo>


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
struct Test_C_Allocator : CppUnit::TestFixture {
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
		  cout << "[C++ Allocator Test:1]......\n";		
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}


    // --------
    // test_two
    // --------

    void test_two () {
		  cout << "[C++ Allocator Test:2]......\n";		
        A x;
        const difference_type s = 20;
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
		  cout << "[C++ Allocator Test:10]......\n";		
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

    void test_bad () {
		  cout << "[C++ Allocator Test:Bad]......\n";			
        A x;
        const difference_type s = 20;
        const pointer b = x.allocate(s);
        try {
            x.allocate(s);
         CPPUNIT_ASSERT(false);
        }
        catch (...) {
            x.deallocate(b, s);
        }
        
    }



   	CPPUNIT_TEST_SUITE(Test_C_Allocator);
      CPPUNIT_TEST(test_one);
      CPPUNIT_TEST(test_two);
      CPPUNIT_TEST(test_ten);
      CPPUNIT_TEST(test_bad);
    CPPUNIT_TEST_SUITE_END();

};





template <typename A>
struct Test_Our_Allocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_constructor()
    // --------

	void test_constructor_1(){
		cout << "[Test constructor 1]......" <<  endl ;
		A x;
		CPPUNIT_ASSERT(true);		
	}

	void test_constructor_2(){
		cout << "[Test constructor 2]......" <<  endl ;
		Allocator<int, 8> x;
		Allocator<int, 8> y;
		Allocator<double, 8> z;
		CPPUNIT_ASSERT(true);		
	}

	void test_constructor_3(){
		cout << "[Test constructor 3]......" <<  endl ;
		try{
			Allocator<int, 1> x;
			Allocator<int, 2> y;
			Allocator<double, 1> z;
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}
		
	}

	void test_constructor_4(){
		cout << "[Test constructor 4]......" <<  endl ;
		A x;
		Allocator<int, 100> y;
		CPPUNIT_ASSERT(true);	
	}

    // --------
    // test_valid()
    // --------

	void test_valid_1(){
		cout << "[Test valid 1]......" <<  endl ;
		try{
			A x;
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}
	}

	void test_valid_2(){
		cout << "[Test valid 2]......" <<  endl ;
		try{
			A x;
			const int size = 2;
			x.allocate(size);
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}
	}

	void test_valid_3(){
		cout << "[Test valid 3]......" <<   endl ;
		try{
			A x;
			const int size = 13;
			x.allocate(size);
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}

	}

	void test_valid_4(){
		cout << "[Test valid 4]......"  << endl ;
		try{
			A x;
			x.allocate(22);
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}

	}

	void test_valid_5(){
		cout << "[Test valid 5]......" << endl;
		try{
			A x;
			x.allocate(25);
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}

	}

    // --------
    // test_allocate()
    // --------

	void test_allocate_1(){
		cout << "[Test allocate 1]......\n";
		try{
			A x;
		   const difference_type s = 2;
		   const value_type      v = 9;
		   const pointer         p = x.allocate(s);
			int unit_size = (int)sizeof(value_type);
			x.construct(p, v);
			char* vp = (char*)p - 4;
			int size = *reinterpret_cast<int*>(vp);
			string name = typeid(x).name();
			if(name == "9AllocatorIiLi100EE"){
				CPPUNIT_ASSERT(size == -(s * unit_size));
			}
			CPPUNIT_ASSERT(*p == v);

		}catch(bad_alloc& exception){
			 
		}
	}

	void test_allocate_2(){
		cout << "[Test allocate 2]......\n";
		try{
			A x;
		   const difference_type s = 5;
		   const value_type      v = 12;
			const pointer p = x.allocate(s);
			int unit_size = (int)sizeof(value_type);
			x.construct(p, v);
			char* vp = (char*)p - 4;
			int size = *reinterpret_cast<int*>(vp);
			string name = typeid(x).name();
			if(name == "9AllocatorIiLi100EE"){
				CPPUNIT_ASSERT(size == -(s * unit_size));
			}
			CPPUNIT_ASSERT(*p == v);
		}catch(bad_alloc& exception){
			 
		}



	}

	void test_allocate_3(){
			cout << "[Test allocate 3]......\n";
		try{
			A x;
		   const difference_type s = 3;
		   const value_type      v = 12;
			const pointer p = x.allocate(s);
			int unit_size = (int)sizeof(value_type);
			string name = typeid(x).name();
			x.construct(p, v);


			char* vp = (char*)p - 4;
			int size = *reinterpret_cast<int*>(vp);
			if(name == "9AllocatorIiLi100EE"){
				CPPUNIT_ASSERT(size == -(s * unit_size));
			}
			CPPUNIT_ASSERT(*p == v);
		}catch(bad_alloc& exception){
			 
		}

	}

	void test_allocate_4(){
		cout << "[Test allocate 4]......\n";
		try{
			A x;
		   difference_type s = 1;
		   value_type      v = 12;
			pointer p = x.allocate(s);
			int unit_size = (int)sizeof(value_type);
			string name = typeid(x).name();
			x.construct(p, v);


			char* vp = (char*)p - 4;
			int size = *reinterpret_cast<int*>(vp);

			if(name == "9AllocatorIiLi100EE"){
				CPPUNIT_ASSERT(size == -(s * unit_size));
			}
			CPPUNIT_ASSERT(*p == v);

			s = 2;
			v = 100;
			p = x.allocate(s);
			x.construct(p, v);

			vp = (char*)p - 4;
			size = *reinterpret_cast<int*>(vp);

			if(name == "9AllocatorIiLi100EE"){
				CPPUNIT_ASSERT(size == -(s * unit_size));
			}
			CPPUNIT_ASSERT(*p == v);
		}catch(bad_alloc& exception){
			 
		}




	}

    // --------
    // test_deallocate()
    // --------

	void test_deallocate_1(){ //only block
		cout << "[Test Deallocate 1]......\n";
		try{
			A x;
			pointer p = x.allocate(10);
		   x.deallocate(p, 0);
		   CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}
	}

	void test_deallocate_2(){
		cout << "[Test Deallocate 2]......\n";
		try{
			A x;
		 	pointer p = x.allocate(10);
      	x.allocate(3);
      	x.deallocate(p, 0);
     		CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}

	}

	void test_deallocate_3(){
		cout << "[Test Deallocate 3]......\n";
		try{
			A x;
			x.allocate(10);
        pointer p = x.allocate(3);
        x.deallocate(p, 0);
        CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}
	}

	void test_deallocate_4(){
		cout << "[Test Deallocate 4]......\n";

		try{
			A x;
			x.allocate(2);
       	pointer p = x.allocate(3);
         x.allocate(4);
         x.deallocate(p,0);
         CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}


	}

	void test_deallocate_5(){
		cout << "[Test Deallocate 5]......\n";

		try{
			A x;
			x.allocate(2);
        pointer p1 = x.allocate(3);
        pointer p2 = x.allocate(4);
        x.deallocate(p2,0);
        x.deallocate(p1,0);
        CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}


	}

	void test_deallocate_6(){
		cout << "[Test Deallocate 6]......\n";


		try{
			A x;
			pointer p1 = x.allocate(2);
        pointer p2 = x.allocate(3);
        pointer p3 = x.allocate(4);
        x.deallocate(p1,0);
        x.deallocate(p2,0);
        x.deallocate(p3,0);
        CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}


	}

	void test_deallocate_7(){
		cout << "[Test Deallocate 7]......\n";

		try{
			A x;
			pointer p1 = x.allocate(10);
        pointer p2 = x.allocate(5);
        pointer p3 = x.allocate(4);
        x.deallocate(p1,0);
        x.deallocate(p2,0);
        x.deallocate(p3,0);
        CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
			 
		}


	}

	void test_deallocate_8(){
		cout << "[Test Deallocate 8]......\n";

		try{
			A x;
			pointer p1 = x.allocate(10);
			pointer p2 = x.allocate(5);
			pointer p3 = x.allocate(4);
			x.deallocate(p3,0);
			x.deallocate(p1,0);
			x.deallocate(p2,0);
			CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){

		}


	}

	void test_deallocate_9(){
		cout << "[Test Deallocate 9]......\n";

		try{
			A x;
			pointer p1 = x.allocate(23);
        x.deallocate(p1,0);
        CPPUNIT_ASSERT(true);
		}catch(bad_alloc& exception){
		}
	}


    // --------
    // test_merge()
    // --------
	//test simple merge of 2 free blocks that returns pointer to left block
	void test_merge_1 () {
		cout << "[Test merge 1].......\n";
		try {
			A x;
			pointer b1 = x.allocate(2);
			pointer b2 = x.allocate(3);
			char* b1_left_sentinel = (char*)b1 - x.sentinel_size;
			char* b2_left_sentinel = (char*)b2 - x.sentinel_size;
			*reinterpret_cast<int*>(b1_left_sentinel) = 8; //making left sentinels positive
			*reinterpret_cast<int*>(b2_left_sentinel) = 24;	
			pointer result = x.merge(b1_left_sentinel,b2_left_sentinel);
			CPPUNIT_ASSERT(true); 
			CPPUNIT_ASSERT(result == b1);
		}
		catch (bad_alloc& exception) {}
	}

	//test that second parameter pointer is not changed
	void test_merge_2 () { 
		cout << "[Test merge 2]......\n";
		try {
			A x;
			pointer value_after_block1_sentinel = x.allocate(3);
			pointer value_after_block2_sentinel = x.allocate(4);
			pointer value_after_block3_sentinel = x.allocate(12);
			char* block1_left_sentinel = (char*)value_after_block1_sentinel - x.sentinel_size;
			char* block2_left_sentinel = (char*)value_after_block2_sentinel - x.sentinel_size;
			char* block3_left_sentinel = (char*)value_after_block3_sentinel - x.sentinel_size;
			*reinterpret_cast<int*>(block1_left_sentinel) = 12;
			*reinterpret_cast<int*>(block2_left_sentinel) = 16;
			*reinterpret_cast<int*>(block3_left_sentinel) = 48;
			pointer b2b3merge = x.merge(block2_left_sentinel, block3_left_sentinel);
			pointer result = x.merge(block1_left_sentinel, block2_left_sentinel);
			CPPUNIT_ASSERT(result == value_after_block1_sentinel);
			CPPUNIT_ASSERT(b2b3merge == value_after_block2_sentinel);
			CPPUNIT_ASSERT(*reinterpret_cast<int*>(block3_left_sentinel) == 48);	
			CPPUNIT_ASSERT(*reinterpret_cast<int*>(block1_left_sentinel) == 92);
		}catch (bad_alloc& exception) {}
	}

	//simple merge
	void test_merge_3 () {
		cout << "[Test merge 3].......\n";
		try {
			A x;
			pointer value_after_sentinel = x.allocate(15);
			pointer value_after_another_sentinel = x.allocate(3);
			char* block_sentinel = (char*)value_after_sentinel - x.sentinel_size;
			char* another_block_sentinel = (char*)value_after_another_sentinel - x.sentinel_size;
			*reinterpret_cast<int*>(block_sentinel) = 60;
			*reinterpret_cast<int*>(another_block_sentinel) = 24;
			x.merge(block_sentinel, another_block_sentinel);
			CPPUNIT_ASSERT(true);
			
		} catch (...) {}
	}




    // -----
    // suite
    // -----

   	CPPUNIT_TEST_SUITE(Test_Our_Allocator);
		CPPUNIT_TEST(test_constructor_1);
			CPPUNIT_TEST(test_constructor_2);
			CPPUNIT_TEST(test_constructor_3);
			CPPUNIT_TEST(test_constructor_4);

			CPPUNIT_TEST(test_valid_1);
			CPPUNIT_TEST(test_valid_2);
			CPPUNIT_TEST(test_valid_3);
			CPPUNIT_TEST(test_valid_4);
			CPPUNIT_TEST(test_valid_5);

			CPPUNIT_TEST(test_allocate_1);
			CPPUNIT_TEST(test_allocate_2);
			CPPUNIT_TEST(test_allocate_3);
			CPPUNIT_TEST(test_allocate_4);

			CPPUNIT_TEST(test_deallocate_1);
		   CPPUNIT_TEST(test_deallocate_2);
		   CPPUNIT_TEST(test_deallocate_3);
		 	CPPUNIT_TEST(test_deallocate_4);
		   CPPUNIT_TEST(test_deallocate_5);
		   CPPUNIT_TEST(test_deallocate_6);
		   CPPUNIT_TEST(test_deallocate_7);
		   CPPUNIT_TEST(test_deallocate_8);
		   CPPUNIT_TEST(test_deallocate_9);

  		CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct Test_Merge : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_merge()
    // --------
	//test simple merge of 2 free blocks that returns pointer to left block
	void test_merge_1 () {
		cout << "[Test merge 1].......\n";
		try {
			A x;
			pointer b1 = x.allocate(2);
			pointer b2 = x.allocate(3);
			char* b1_left_sentinel = (char*)b1 - x.sentinel_size;
			char* b2_left_sentinel = (char*)b2 - x.sentinel_size;
			*reinterpret_cast<int*>(b1_left_sentinel) = 8; //making left sentinels positive
			*reinterpret_cast<int*>(b2_left_sentinel) = 24;	
			pointer result = x.merge(b1_left_sentinel,b2_left_sentinel);
			CPPUNIT_ASSERT(true); 
			CPPUNIT_ASSERT(result == b1);
		}
		catch (bad_alloc& exception) {}
	}

	//test that second parameter pointer is not changed
	void test_merge_2 () { 
		cout << "[Test merge 2]......\n";
		try {
			A x;
			pointer value_after_block1_sentinel = x.allocate(3);
			pointer value_after_block2_sentinel = x.allocate(4);
			pointer value_after_block3_sentinel = x.allocate(12);
			char* block1_left_sentinel = (char*)value_after_block1_sentinel - x.sentinel_size;
			char* block2_left_sentinel = (char*)value_after_block2_sentinel - x.sentinel_size;
			char* block3_left_sentinel = (char*)value_after_block3_sentinel - x.sentinel_size;
			*reinterpret_cast<int*>(block1_left_sentinel) = 12;
			*reinterpret_cast<int*>(block2_left_sentinel) = 16;
			*reinterpret_cast<int*>(block3_left_sentinel) = 48;
			pointer b2b3merge = x.merge(block2_left_sentinel, block3_left_sentinel);
			pointer result = x.merge(block1_left_sentinel, block2_left_sentinel);
			CPPUNIT_ASSERT(result == value_after_block1_sentinel);
			CPPUNIT_ASSERT(b2b3merge == value_after_block2_sentinel);
			CPPUNIT_ASSERT(*reinterpret_cast<int*>(block3_left_sentinel) == 48);	
			CPPUNIT_ASSERT(*reinterpret_cast<int*>(block1_left_sentinel) == 92);
		}catch (bad_alloc& exception) {}
	}

	//simple merge
	void test_merge_3 () {
		cout << "[Test merge 3].......\n";
		try {
			A x;
			pointer value_after_sentinel = x.allocate(15);
			pointer value_after_another_sentinel = x.allocate(3);
			char* block_sentinel = (char*)value_after_sentinel - x.sentinel_size;
			char* another_block_sentinel = (char*)value_after_another_sentinel - x.sentinel_size;
			*reinterpret_cast<int*>(block_sentinel) = 60;
			*reinterpret_cast<int*>(another_block_sentinel) = 24;
			x.merge(block_sentinel, another_block_sentinel);
			CPPUNIT_ASSERT(true);
			
		} catch (...) {}
	}
   	CPPUNIT_TEST_SUITE(Test_Merge);
			CPPUNIT_TEST(test_merge_1);
			CPPUNIT_TEST(test_merge_2);
			CPPUNIT_TEST(test_merge_3);
		CPPUNIT_TEST_SUITE_END();};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "Testing..." << endl;

    CppUnit::TextTestRunner tr2;
	 cout << "========================Testing Merge Utility Function====================\n";
	 tr2.addTest(Test_Merge< Allocator<int, 100> >::suite());
	 tr2.addTest(Test_Merge< Allocator<double, 100> >::suite());
	 tr2.addTest(Test_Merge< Allocator<float, 100> >::suite());
	 tr2.addTest(Test_Merge< Allocator<char, 100> >::suite());
	 tr2.addTest(Test_Merge< Allocator<long, 100> >::suite());
	 tr2.addTest(Test_Merge< Allocator<long long, 100> >::suite());
    tr2.run();



    CppUnit::TextTestRunner tr0;
	 cout << "===============================C++ Allocator===================================\n";
	 tr0.addTest(Test_C_Allocator< allocator<int> >::suite());
	 tr0.addTest(Test_C_Allocator< allocator<double> >::suite());
	 tr0.addTest(Test_C_Allocator< allocator<float> >::suite());
	 tr0.addTest(Test_C_Allocator< allocator<long> >::suite());
	 tr0.addTest(Test_C_Allocator< allocator<char> >::suite());
	 tr0.addTest(Test_C_Allocator< allocator<long long> >::suite());
    tr0.run();

    CppUnit::TextTestRunner tr1;
	 cout << "===============================My Allocator===================================\n";
 	 tr1.addTest(Test_Our_Allocator< Allocator<int, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<double, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<float, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<char, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<long, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<long long, 100> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<int, 50> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<double, 50> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<float, 50> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<char, 50> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<long, 50> >::suite());
	 tr1.addTest(Test_Our_Allocator< Allocator<long long, 50> >::suite());
    tr1.run();



    cout << "Done." << endl;
    return 0;}

