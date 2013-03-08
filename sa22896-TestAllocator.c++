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

  using namespace std;
  // -------------
  // TestAllocator
  // -------------
  // view 
  int& view (char& c) 
  {
      return *reinterpret_cast<int*>(&c);
  }


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

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

  template <typename A>
  struct TestAllocator2 : CppUnit::TestFixture 
  {
      // --------
      // typedefs
      // --------

      typedef typename A::value_type      value_type;
      typedef typename A::difference_type difference_type;
      typedef typename A::pointer         pointer;
       // --------
      // test_allocate's
      // --------
      //request max allocatable block
      void test_allocate1() 
      {
          A x;
          const int numFree=sizeof(x.a)-8; //bytes of allocatable space in char a[]
          const difference_type s = numFree / sizeof (value_type); //count of typename objs to be allocated

          const pointer         b = x.allocate(s);
                pointer         e = b+s;
          const int req_numbytes = sizeof(value_type) *s ; //number of bytes to be allocated
            //Check Allocated block's sent val's
            CPPUNIT_ASSERT(view(x.a[0]) == -(sizeof(x.a)-8));
            CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == -(sizeof(x.a)-8));
        }   

    //not enough for leftover, give rest of block.
    //After allocation: leftover size < sizeof(value_type) +8
     void test_allocate2() 
     {
          A x;
          const int numFree=sizeof(x.a)-8; //bytes of allocatable space in char a[]
          const difference_type s =13 ;       
          try{
            x.allocate(s);
            const int req_numbytes = sizeof(value_type) *s ;
          CPPUNIT_ASSERT(view(x.a[0])==-(sizeof(value_type)*s)); 
          CPPUNIT_ASSERT(view(x.a[(sizeof(value_type)*s)+4])==-(sizeof(value_type)*s));
          }
          catch (std::bad_alloc& e) {
              CPPUNIT_ASSERT (e.what() != 0);
          }
          
       
      } 
    //Allocate (13*4+8==60)+(5*4+8==28bytes)==88 ==(N- (sizeof(T)+8) bytes) bytes total. 
    //Enough leftover for just 1 sizeof(T)+8 bytes
    void test_allocate3() 
   {
          A x;
          const int numFree=sizeof(x.a)-8; //bytes of allocatable space in char a[]
          const difference_type s = (numFree-(sizeof(value_type)+8))/(sizeof(value_type)) ;
          x.allocate(s);
          int tempSize=(-s)*(sizeof(value_type));
          CPPUNIT_ASSERT(view(x.a[0])== tempSize);
          CPPUNIT_ASSERT(view(x.a[s*sizeof(value_type)+4]) == tempSize);
          CPPUNIT_ASSERT( view(x.a[ (s*sizeof(value_type)) + 8])  == (sizeof(x.a) -((s*sizeof(value_type)) + 16)) );
          CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == (sizeof(x.a) -((s*sizeof(value_type)) + 16)));
         //Can't allocate negative amount items
          try{
                  x.allocate(-1);
          } catch (std::bad_alloc& e) {
              CPPUNIT_ASSERT (e.what() != 0);
          }
   }  

      // --------
      // test_allocator
      // --------
     void test_allocator1 () 
     {
          A x;
          //Assert Sentinel Values at Beginning and End of Block equivalent to N-8
          CPPUNIT_ASSERT(view(x.a[0]) == sizeof(x.a)-8);
          CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == sizeof(x.a)-8);
      }

    
      void test_deallocate1()
    {
      A x;
      pointer p = x.allocate(5); 
      CPPUNIT_ASSERT(view(x.a[0]) == -(sizeof(value_type)*5));
      CPPUNIT_ASSERT(view(x.a[(sizeof(value_type)*5)+4]) == -(sizeof(value_type)*5));
      x.deallocate(p, 0); 
      CPPUNIT_ASSERT(view(x.a[0]) == sizeof(x.a)-8);
      CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == sizeof(x.a)-8);
    
    }

    void test_deallocate2()
    {
      A x; 
      pointer p = x.allocate(4);
      CPPUNIT_ASSERT(view(x.a[0]) == -(4*sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 4]) == -(4*sizeof(value_type)));
      pointer s = x.allocate(3); 
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 8]) == -(3*sizeof(value_type)));
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 3*sizeof(value_type)+ 12 ]) == -(3*sizeof(value_type)));
      x.deallocate(p, 0);  
      CPPUNIT_ASSERT(view(x.a[0]) == (4*sizeof(value_type)));
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) +4]) == (4*sizeof(value_type)));
      x.deallocate(s, 0); 
      CPPUNIT_ASSERT(view(x.a[0]) == sizeof(x.a)-8);
      CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == sizeof(x.a)-8); 
                                                                                                   
    }
    void test_deallocate3()
    {
      A x; 
      pointer p = x.allocate(1);
      CPPUNIT_ASSERT(view(x.a[0]) == -(sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[sizeof(value_type) + 4]) == -(sizeof(value_type)));
      pointer s = x.allocate(1); 
      CPPUNIT_ASSERT(view(x.a[sizeof(value_type) + 8]) == -(sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[2*sizeof(value_type) + 12]) == -(sizeof(value_type)));
      pointer t = x.allocate(1); 
      CPPUNIT_ASSERT(view(x.a[2*sizeof(value_type) + 16]) == -(sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[3*sizeof(value_type) + 20]) == -(sizeof(value_type)));
      x.deallocate(s,0); 
      CPPUNIT_ASSERT(view(x.a[sizeof(value_type) + 8]) == (sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[2*sizeof(value_type) + 12]) == (sizeof(value_type)));
      x.deallocate(p,0);
      CPPUNIT_ASSERT(view(x.a[0]) == (2*sizeof(value_type) +8) );
      CPPUNIT_ASSERT(view(x.a[2*sizeof(value_type) + 12]) == (2*sizeof(value_type))+8);
      x.deallocate(t,0);
      CPPUNIT_ASSERT(view(x.a[0]) == sizeof(x.a)-8 );
      CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == sizeof(x.a)-8 );
    }
    
    void test_valid1(){
          A x;
          const int numFree=sizeof(x.a)-8;
          const difference_type s = numFree / sizeof (value_type); 
          const pointer         b = x.allocate(s);
          const int req_numbytes = sizeof(value_type) *s ; 
          CPPUNIT_ASSERT(view(x.a[0]) == -(sizeof(x.a)-8));
          CPPUNIT_ASSERT(view(x.a[sizeof(x.a)-4]) == -(sizeof(x.a)-8));
          CPPUNIT_ASSERT(x.valid() == true);

    }
    void test_valid2(){
      A x; 
      x.allocate(4);
      CPPUNIT_ASSERT(view(x.a[0]) == -(4*sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 4]) == -(4*sizeof(value_type)));
      x.allocate(3); 
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 8]) == -(3*sizeof(value_type)));
      CPPUNIT_ASSERT(view(x.a[4*sizeof(value_type) + 3*sizeof(value_type)+ 12 ]) == -(3*sizeof(value_type)));
      CPPUNIT_ASSERT(x.valid() == true);
    }
    void test_valid3(){
    A x; 
      pointer p = x.allocate(1);
      CPPUNIT_ASSERT(view(x.a[0]) == -(sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[sizeof(value_type) + 4]) == -(sizeof(value_type)));
      pointer s = x.allocate(1); 
      CPPUNIT_ASSERT(view(x.a[sizeof(value_type) + 8]) == -(sizeof(value_type) ) );
      CPPUNIT_ASSERT(view(x.a[2*sizeof(value_type) + 12]) == -(sizeof(value_type)));
      pointer t = x.allocate(1);
      x.deallocate(s,0); 
      CPPUNIT_ASSERT(x.valid() == true);
      x.deallocate(p,0);
      CPPUNIT_ASSERT(x.valid() == true);
      x.deallocate(t,0);
      CPPUNIT_ASSERT(x.valid() == true);
    }

      // -----
      // suite
      // -----

       CPPUNIT_TEST_SUITE(TestAllocator2);
       //call test_allocate's
        CPPUNIT_TEST(test_allocate1); 
        CPPUNIT_TEST(test_allocate2); 
        CPPUNIT_TEST(test_allocate3); 
      CPPUNIT_TEST(test_allocator1); 
      CPPUNIT_TEST(test_deallocate1); 
      CPPUNIT_TEST(test_deallocate2);
      CPPUNIT_TEST(test_deallocate3);
      CPPUNIT_TEST(test_valid1);
      CPPUNIT_TEST(test_valid2);
      CPPUNIT_TEST(test_valid3);

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


     tr.addTest(TestAllocator2< Allocator<int, 100> >::suite()); // uncomment!
     tr.addTest(TestAllocator2< Allocator<double, 100> >::suite()); // uncomment!
     tr.addTest(TestAllocator2< Allocator<char, 100> >::suite()); // uncomment!
     tr.addTest(TestAllocator2< Allocator<double, 1000> >::suite()); // uncomment!




    tr.run();

    cout << "Done." << endl;
    return 0;}