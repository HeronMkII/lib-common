#include <uart/uart.h>
#include <test/test.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define TEST_NO 3  //this is total number of tests

void fun1(void); // To check if put_char(uint8_t c) is working
void fun2(void); // To check if get_char(uint8_t* c) is working; Test 1 needs to be working for the results of Test 2 to be sensible
void fun3(void); //WORK IN PROGRESS!



test_t t1 = { .name = "Test 1", .fn = fun1 };
test_t t2 = { .name = "Test 2", .fn = fun2 };
test_t t3 = { .name = "Test 3", .fn = fun3 };

test_t* suite[TEST_NO] = { &t1, &t2, , &t3 };




void fun1(void){
    //print("TEST 1\n\n");

    for(int i=0;i<8;i++){
      put_char(_BV(i));
      //LINDAT >>= i ;
      ASSERT_EQ(LINDAT,_BV(i));
    }
  }

void fun2(void){
    //print("TEST 2\n\n");

    for(int i=0;i<8;i++){
      put_char(_BV(i));
      uint8_t temp;
      get_char(&temp);
      ASSERT_EQ(temp,LINDAT);
    }
  }

void fun3(void){
    //print("TEST 3\n\n");

    init_uart();
    ASSERT_EQ(LDIV,25);
    ASSERT_EQ(LINBRRH,0);
    ASSERT_EQ(LINBRRL,25);
    ASSERT_EQ(LINBTR,4);
    ASSERT_EQ(LINCR,( _BV(LENA) + _BV(LCMD2) + _BV(LCMD1) + _BV(LCMD0) ));
    ASSERT_EQ(LINENIR,_BV(LENRXOK));



}


int main(){
  init_uart();
  run_tests(suite, TEST_NO);
  return 0;

}
