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
#define protected public
#define class struct
#include "Allocator.h"


using namespace std;
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

struct TestMyAllocator : CppUnit::TestFixture {

    
    // ---------
    // test_view
    // ---------
    
    void test_view_1 () {
    	char c[100];
    	Allocator<char, 1000> B;		//only to access view method
    	*reinterpret_cast<int*>(c) = 6;
    	CPPUNIT_ASSERT(B.view(c[0]) == 6);
    }
    
    void test_view_2 () {
    	char c[100];
    	Allocator<char, 1000> B;		//only to access view method
    	*reinterpret_cast<int*>(c+54) = 809;
    	CPPUNIT_ASSERT(B.view(c[54]) == 809);
    }
    
    void test_view_3 () {
    	char c[100];
    	Allocator<char, 1000> B;		//only to access view method
    	*reinterpret_cast<int*>(c+22) = -206;
    	CPPUNIT_ASSERT(B.view(c[22]) == -206);
    }
    
    // ---------------
    // test_const_view
    // ---------------
    
    void test_const_view_1 (){
    	const char c[100] = {56, 0, 0, 0};
    	Allocator<char, 1000> B;		//only to access view method
    	CPPUNIT_ASSERT(B.view(c[0]) == 56);
    }
    
	void test_const_view_2 (){
    	const char c[100] = {2, 0, 0, 0};
    	Allocator<char, 1000> B;		//only to access view method
    	CPPUNIT_ASSERT(B.view(c[0]) == 2);
    }
    
    void test_const_view_3 (){
    	const char c[100] = {3, 0, 0 , 0};
    	Allocator<char, 1000> B;		//only to access view method
    	CPPUNIT_ASSERT(B.view(c[0]) == 3);
    }
    
	// ----------------
    // test_constructor
    // ----------------

    void test_constructor_1 () {
    	Allocator<int, 16> B;
    	CPPUNIT_ASSERT(B.view(B.a[0]) == 8);
    	CPPUNIT_ASSERT(B.view(B.a[12]) == 8);
    }
    
    void test_constructor_2 () {
    	Allocator<char, 1000> B;
    	CPPUNIT_ASSERT(B.view(B.a[0]) == 992);
    	CPPUNIT_ASSERT(B.view(B.a[996]) == 992);
    }
    
    void test_constructor_3 () {
    	try{
		    Allocator<long, 9> B;
		    CPPUNIT_ASSERT(false);
    	}
    	catch (std::bad_alloc::bad_alloc& e){
			CPPUNIT_ASSERT(true);    	
    	}
    }
    
    // ----------
    // test_valid
    // ----------

    void test_valid_1 () {
    	Allocator<int, 100> B;
    	B.view(B.a[0]) = -12;
    	B.view(B.a[16]) = -12;
    	B.view(B.a[20]) = 72;
    	B.view(B.a[96]) = 72;
    	
    	CPPUNIT_ASSERT(B.valid());
    }
    
    void test_valid_2 () {
    	Allocator<char, 100> B;
    	B.view(B.a[0]) = -12;
    	B.view(B.a[16]) = -12;
    	B.view(B.a[20]) = -30;
    	B.view(B.a[54]) = -30;
    	B.view(B.a[58]) = 34;
    	B.view(B.a[96]) = 34;
    	
    	CPPUNIT_ASSERT(B.valid());	
    }
    
    void test_valid_3 () {
        Allocator<long, 100> B;
        B.view(B.a[0]) = -12;
    	B.view(B.a[16]) = -12;
    	B.view(B.a[20]) = 30;
    	B.view(B.a[54]) = 30;
    	B.view(B.a[58]) = 34;
    	B.view(B.a[96]) = 34;
    	CPPUNIT_ASSERT(!B.valid());
    }
    
    // -------------
    // test_allocate
    // -------------

    void test_allocate_1 () {
    	Allocator<int, 100> B;
    	B.allocate(3);

    	CPPUNIT_ASSERT(B.view(B.a[0]) == -12);
    	CPPUNIT_ASSERT(B.view(B.a[16]) == -12);
    	CPPUNIT_ASSERT(B.view(B.a[20]) == 72);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == 72);
    	CPPUNIT_ASSERT(B.valid());
    }
    
    void test_allocate_2 () {
    	Allocator<char, 100> B;
    	B.allocate(90);
    	
    	CPPUNIT_ASSERT(B.view(B.a[0]) == -92);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == -92);
    	CPPUNIT_ASSERT(B.valid());	
    }
    
    void test_allocate_3 () {
        Allocator<double, 100> B;
    	B.allocate(6);
    	B.allocate(2);
    	B.allocate(1);
    	
    	CPPUNIT_ASSERT(B.view(B.a[0]) == -48);
    	CPPUNIT_ASSERT(B.view(B.a[52]) == -48);
    	CPPUNIT_ASSERT(B.view(B.a[56]) == -16);
    	CPPUNIT_ASSERT(B.view(B.a[76]) == -16);
    	CPPUNIT_ASSERT(B.view(B.a[80]) == -12);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == -12);
    	CPPUNIT_ASSERT(B.valid());
    }
    
    void test_allocate_4 () {
        Allocator<double, 100> B;
		try{
			B.allocate(500);
			CPPUNIT_ASSERT(false);
		}
		catch(std::bad_alloc::bad_alloc& e){
			CPPUNIT_ASSERT(true);
		}
    }
    
    // ---------------
    // test_deallocate
    // ---------------
    
    void test_deallocate_1 () {
    	Allocator<int, 100> B;
    	int *p1 = B.allocate(3);
    	
    	B.deallocate(p1);

    	CPPUNIT_ASSERT(B.view(B.a[0]) == 92);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == 92);
    	CPPUNIT_ASSERT(B.valid());
    }
    
    void test_deallocate_2 () {
    	Allocator<char, 100> B;
    	char *p1 = B.allocate(90);
    	
    	B.deallocate(p1);
    	
    	CPPUNIT_ASSERT(B.view(B.a[0]) == 92);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == 92);
    	CPPUNIT_ASSERT(B.valid());	
    }
    
    void test_deallocate_3 () {
        Allocator<double, 100> B;
    	double *p1 = B.allocate(6);
    	double *p2 = B.allocate(2);
    	double *p3 = B.allocate(1);
    	
    	B.deallocate(p3);
    	B.deallocate(p1);
    	B.deallocate(p2);
    	
    	CPPUNIT_ASSERT(B.view(B.a[0]) == 92);
    	CPPUNIT_ASSERT(B.view(B.a[96]) == 92);
    	CPPUNIT_ASSERT(B.valid());
    }

	//Do not need tests for:
	//Construct
	//Destroy
	//==
	//!=
	
	// -----
	// suite
	// -----

	CPPUNIT_TEST_SUITE(TestMyAllocator);
	CPPUNIT_TEST(test_view_1);
	CPPUNIT_TEST(test_view_2);
	CPPUNIT_TEST(test_view_3);
	CPPUNIT_TEST(test_const_view_1);
	CPPUNIT_TEST(test_const_view_2);
	CPPUNIT_TEST(test_const_view_3);
	CPPUNIT_TEST(test_constructor_1);
	CPPUNIT_TEST(test_constructor_2);
	CPPUNIT_TEST(test_constructor_3);
	CPPUNIT_TEST(test_valid_1);
	CPPUNIT_TEST(test_valid_2);
	CPPUNIT_TEST(test_valid_3);
	CPPUNIT_TEST(test_allocate_1);
	CPPUNIT_TEST(test_allocate_2);
	CPPUNIT_TEST(test_allocate_3);
	CPPUNIT_TEST(test_allocate_4);
	CPPUNIT_TEST(test_deallocate_1);
	CPPUNIT_TEST(test_deallocate_2);
	CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST_SUITE_END();

};

// ----
// main
// ----

int main () {
    using namespace std;
    
    //This line caused memory leaks before, and it still does now
    //ios_base::sync_with_stdio(false); // turn off synchronization with C I/O 
    cout << "mattj-TestAllocator.c++" << endl;
    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    
	tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
	
	//Student Tests
	
	tr.addTest(TestMyAllocator::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;
}
