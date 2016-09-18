#include <stack.cpp>
#include <catch.hpp>
#include <iostream>

using namespace std;

SCENARIO("push_pop_count", "[push_pop_count]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.count()==1);
  REQUIRE(st.pop()==1);
  st.push(1);
  REQUIRE(st.count()==2);
  REQUIRE(st.pop()==1);
}

SCENARIO("prisvoenie", "[prisvoenie]")
{
  stack<int> st;
  st.push(1);
  stack<int> st2;
  st2=st;
  REQUIRE(st2.count()==1);
  REQUIRE(st2.pop()==1);
}

