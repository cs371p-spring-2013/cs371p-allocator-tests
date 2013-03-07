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

	//Test Constructor
	void test_contructor_1()
	{
		//Arrange, Act
		Allocator<int, 100> allocator;
		char* memory = allocator.a;
		int firstSentinal = allocator.view(*memory);
		char* lastSentinalChar = memory + 96;
		int lastSentinal = allocator.view(*lastSentinalChar);
		
		//Assert
		CPPUNIT_ASSERT(firstSentinal == 92);
		CPPUNIT_ASSERT(lastSentinal == 92);
	}
	
	void test_contructor_2()
	{
		//Arrange, Act
		Allocator<int, 200> allocator;
		char* memory = allocator.a;
		int firstSentinal = allocator.view(*memory);
		char* lastSentinalChar = memory + 196;
		int lastSentinal = allocator.view(*lastSentinalChar); 
		
		//Assert
		CPPUNIT_ASSERT(firstSentinal == 192);
		CPPUNIT_ASSERT(lastSentinal == 192);
	}
	
	void test_contructor_3()
	{
		//Arrange, Act
		Allocator<double, 1000> allocator;
		char* memory = allocator.a;
		int firstSentinal = allocator.view(*memory);
		char* lastSentinalChar = memory + 996;
		int lastSentinal = allocator.view(*lastSentinalChar); 
	
		//Assert
		CPPUNIT_ASSERT(firstSentinal == 992);
		CPPUNIT_ASSERT(lastSentinal == 992);
	}

	void test_allocate_once()
	{
		//Arrange
		Allocator<int, 100> allocator;
		char* memory = allocator.a;
		char* firstPairAddress = memory + 16;
		char* secondSentinalAddress = memory + 20;
		char* secondPairAddress = memory + 96;
		const int val = 123;

		//Act
		int* p1 = allocator.allocate(3);
		allocator.construct(p1, val);
		int firstSentinal = *reinterpret_cast<int*>(memory);
		int firstSentinalPair = *reinterpret_cast<int*>(firstPairAddress);
		int secondSentinal = *reinterpret_cast<int*>(secondSentinalAddress);
		int secondSentinalPair = *reinterpret_cast<int*>(secondPairAddress);
		
		//Assert
		CPPUNIT_ASSERT(firstSentinal == -12);
		CPPUNIT_ASSERT(firstSentinalPair == -12);
		CPPUNIT_ASSERT(secondSentinal == 72);
		CPPUNIT_ASSERT(secondSentinalPair == 72);
		CPPUNIT_ASSERT(*p1 == 123);

		//Teardown
		allocator.deallocate(p1);
		allocator.destroy(p1);
	}
	
	void test_allocate_twice()
	{
		//Arrange
		Allocator<int, 100> allocator;
		char* memory = allocator.a;
		char* firstPairAddress = memory + 8;
		char* secondSentinalAddress = firstPairAddress + 4;
		char* secondPairAddress = secondSentinalAddress + 8;
		char* thirdSentinalAddress = secondPairAddress + 4;
		char* thirdPairAddress = thirdSentinalAddress + 4 + 68;
		
		//Act
		int* p1 = allocator.allocate(1);
		int* p2 = allocator.allocate(1);	
		const int a = 150;
		const int b = 300;
		allocator.construct(p1, a);
		allocator.construct(p2, b);
		int firstSentinal = *reinterpret_cast<int*>(memory);
		int firstSentinalPair = *reinterpret_cast<int*>(firstPairAddress);
		int secondSentinal = *reinterpret_cast<int*>(secondSentinalAddress);
		int secondSentinalPair = *reinterpret_cast<int*>(secondPairAddress);
		int thirdSentinal = *reinterpret_cast<int*>(thirdSentinalAddress);
		int thirdSentinalPair = *reinterpret_cast<int*>(thirdPairAddress);
		
		//Assert
		CPPUNIT_ASSERT(*p1 == 150);
		CPPUNIT_ASSERT(*p2 == 300);
		CPPUNIT_ASSERT(firstSentinal == -4);
		CPPUNIT_ASSERT(firstSentinalPair == -4);
		CPPUNIT_ASSERT(secondSentinal == -4);
		CPPUNIT_ASSERT(secondSentinalPair == -4);
		CPPUNIT_ASSERT(thirdSentinal == 68);
		CPPUNIT_ASSERT(thirdSentinalPair == 68);
		
		//Teardown
		allocator.deallocate(p1);
		allocator.deallocate(p2);
		allocator.destroy(p1);
		allocator.destroy(p2);
	}

	void test_allocate_no_space()
	{
		//Arrange
		Allocator<int, 100> allocator;
		bool exceptionThrown = false;

		//Act
		try
		{
			allocator.allocate(100);
		}
		catch(std::bad_alloc& e)
		{
			exceptionThrown = true;
		}

		//Assert
		CPPUNIT_ASSERT(exceptionThrown);

		
	}
	void test_deallocate()
	{
		//Arrange
		Allocator<int, 100> allocator;
		
		char* memory = allocator.a;
		char* lastSentinalAddress = memory + 96;
		int* pointer = allocator.allocate(3);
		
		//Act
		allocator.deallocate(pointer);
		
		//Assert
		int firstSentinal = *reinterpret_cast<int*>(memory);
		int lastSentinal = *reinterpret_cast<int*>(lastSentinalAddress);
		CPPUNIT_ASSERT(firstSentinal == 92);
		CPPUNIT_ASSERT(lastSentinal == 92);

	}
	
	void test_deallocate_left_coalescing()
	{
		//Arrange
		Allocator<int, 200> allocator;
		char* mockMemory = allocator.a;
		char* firstSentinalAddress = mockMemory;
		char* firstSentinalPairAddress = firstSentinalAddress + 4 + 92;
		char* secondSentinalAddress = firstSentinalPairAddress + 4;
		char* secondSentinalPairAddress = secondSentinalAddress + 4 + 12;
		char* thirdSentinalAddress = secondSentinalPairAddress + 4;
		char* thirdSentinalPairAddress = thirdSentinalAddress + 4 + 72;
		*reinterpret_cast<int*>(firstSentinalAddress) = 92;
		*reinterpret_cast<int*>(firstSentinalPairAddress) = 92;
		*reinterpret_cast<int*>(secondSentinalAddress) = -12;
		*reinterpret_cast<int*>(secondSentinalPairAddress) = -12;
		*reinterpret_cast<int*>(thirdSentinalAddress) = 72;
		*reinterpret_cast<int*>(thirdSentinalPairAddress) = 72;

		int* pointerToData = reinterpret_cast<int*>(secondSentinalAddress + 4);
		//Act
		allocator.deallocate(pointerToData);
		int firstSentinal = *reinterpret_cast<int*>(mockMemory);
		int lastSentinal = *reinterpret_cast<int*>(mockMemory + 4 + 192);
		
		//Assert
		CPPUNIT_ASSERT(firstSentinal == 192);
		CPPUNIT_ASSERT(lastSentinal == 192);
	}
	
	void test_deallocate_twice()
	{
		//Arrange
		Allocator<int, 100> allocator;
		char* memory = allocator.a;
		int* p1 = allocator.allocate(1);
		int* p2 = allocator.allocate(1);
		
		//Act
		allocator.deallocate(p1);
		allocator.deallocate(p2);
		int sentinal = allocator.view(*memory);

		//Assert
		CPPUNIT_ASSERT(sentinal == 92);
	}
	void test_valid() {
		Allocator<int, 100> allocator;
		char* mockMemory = allocator.a;
		char* firstSentinalAddress = mockMemory;
		char* lastSentinalAddress = mockMemory + 96;

		CPPUNIT_ASSERT(allocator.valid());

		*reinterpret_cast<int*>(firstSentinalAddress) = 20;
		CPPUNIT_ASSERT(!allocator.valid());
		*reinterpret_cast<int*>(firstSentinalAddress) = 92;
		CPPUNIT_ASSERT(allocator.valid());

		*reinterpret_cast<int*>(lastSentinalAddress) = -24;
		CPPUNIT_ASSERT(!allocator.valid());
		*reinterpret_cast<int*>(lastSentinalAddress) = 92;
		CPPUNIT_ASSERT(allocator.valid());
	}

	void test_valid_with_allocate() {
		Allocator<int, 100> allocator;
		char* mockMemory = allocator.a;
		char* firstSentinalAddress = mockMemory;
		char* firstSentinalPairAddress = firstSentinalAddress + 4 + 12;
		char* lastSentinalAddress = mockMemory + 96;

		CPPUNIT_ASSERT(allocator.valid());

		allocator.allocate(3);

		*reinterpret_cast<int*>(firstSentinalAddress) = 12;
		CPPUNIT_ASSERT(!allocator.valid());
		*reinterpret_cast<int*>(firstSentinalAddress) = -12;
		CPPUNIT_ASSERT(allocator.valid());

		*reinterpret_cast<int*>(firstSentinalPairAddress) = 12;
		CPPUNIT_ASSERT(!allocator.valid());
		*reinterpret_cast<int*>(firstSentinalPairAddress) = -12;
		CPPUNIT_ASSERT(allocator.valid());

		*reinterpret_cast<int*>(lastSentinalAddress) = 68;
		CPPUNIT_ASSERT(!allocator.valid());
		*reinterpret_cast<int*>(lastSentinalAddress) = 72;
		CPPUNIT_ASSERT(allocator.valid());
	}

	void test_valid_with_deallocate() {
		Allocator<int, 200> allocator;
		char* mockMemory = allocator.a;
		char* firstSentinalAddress = mockMemory;
		char* firstSentinalPairAddress = firstSentinalAddress + 4 + 92;
		char* secondSentinalAddress = firstSentinalPairAddress + 4;
		char* secondSentinalPairAddress = secondSentinalAddress + 4 + 12;
		char* thirdSentinalAddress = secondSentinalPairAddress + 4;
		char* thirdSentinalPairAddress = thirdSentinalAddress + 4 + 72;
		*reinterpret_cast<int*>(firstSentinalAddress) = 92;
		*reinterpret_cast<int*>(firstSentinalPairAddress) = 92;
		*reinterpret_cast<int*>(secondSentinalAddress) = -12;
		*reinterpret_cast<int*>(secondSentinalPairAddress) = -12;
		*reinterpret_cast<int*>(thirdSentinalAddress) = 72;
		*reinterpret_cast<int*>(thirdSentinalPairAddress) = 72;

		int* pointerToData = reinterpret_cast<int*>(secondSentinalAddress + 4);
		allocator.deallocate(pointerToData);
		CPPUNIT_ASSERT(allocator.valid());
	}

	//View
	void test_view_1()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		int firstSentinal = allocator.view(*allocator.a);

		//Assert
		CPPUNIT_ASSERT(firstSentinal == 92);
	}

	void test_view_2()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		allocator.allocate(1);
		int firstSentinal = allocator.view(*allocator.a);

		//Assert
		CPPUNIT_ASSERT(firstSentinal == -4);
	}

	void test_view_3()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		allocator.allocate(10);
		int firstSentinal = allocator.view(*allocator.a);

		//Assert
		CPPUNIT_ASSERT(firstSentinal == -40);
	}
	
	void test_set_1()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		allocator.set(10, *allocator.a, 0);
		int view = allocator.view(*allocator.a);

		//Assert
		CPPUNIT_ASSERT(view == 10);
	}

	void test_set_2()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		allocator.set(10, *allocator.a, 20);
		int view = allocator.view(*(allocator.a + 20));

		//Assert
		CPPUNIT_ASSERT(view == 10);
	}

	void test_set_3()
	{
		//Arrange
		Allocator<int, 100> allocator;

		//Act
		allocator.set(10, *(allocator.a + 30), 20);
		int view = allocator.view(*(allocator.a + 50));

		//Assert
		CPPUNIT_ASSERT(view == 10);
	}

	// -----
	// suite
	// -----

	CPPUNIT_TEST_SUITE(TestAllocator);
	CPPUNIT_TEST(test_one);
	CPPUNIT_TEST(test_ten);
	CPPUNIT_TEST(test_contructor_1);
	CPPUNIT_TEST(test_contructor_2);
	CPPUNIT_TEST(test_contructor_3);
	CPPUNIT_TEST(test_allocate_once);
	CPPUNIT_TEST(test_allocate_twice);
	CPPUNIT_TEST(test_allocate_no_space);
	CPPUNIT_TEST(test_deallocate);
	CPPUNIT_TEST(test_deallocate_left_coalescing);
	CPPUNIT_TEST(test_deallocate_twice);
	CPPUNIT_TEST(test_valid);
	CPPUNIT_TEST(test_valid_with_allocate);
	CPPUNIT_TEST(test_valid_with_deallocate); 
	CPPUNIT_TEST(test_view_1);
	CPPUNIT_TEST(test_view_2);
	CPPUNIT_TEST(test_view_3);
	CPPUNIT_TEST(test_set_1);
	CPPUNIT_TEST(test_set_2);
	CPPUNIT_TEST(test_set_3);
	CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
	using namespace std;
	ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
	cout << "TestAllocator.c++" << endl;

	CppUnit::TextTestRunner tr;

	//Tests both the allocators
	tr.addTest(TestAllocator< std::allocator<int> >::suite());
	tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

	tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

	tr.run();

	cout << "Done." << endl;
	return 0;}
