#include <stack.cpp>
#include <catch.hpp>
#include <iostream>

using namespace std;

SCENARIO("push", "[push]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.count()==1);
  REQUIRE(st.pop()==1);
  st.push(2);
  REQUIRE(st.count()==2);

}

SCENARIO("count", "[count]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.count()==1);
}

SCENARIO("pop", "[pop]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.count()==1);
  REQUIRE(st.pop()==1);
}

SCENARIO("prisvoenie", "[prisvoenie]")
{
  stack<int> st;
  st.push(1);
  stack<int> st2;
  st2=st;
  REQUIRE(st.count()==1);
  REQUIRE(st.pop()==1);
}

