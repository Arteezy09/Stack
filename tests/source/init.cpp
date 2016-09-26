#include <stack.cpp>
#include <catch.hpp>
#include <iostream>

using namespace std;

SCENARIO("count", "[count]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.count()==1);
}

SCENARIO("push_top", "[push_top]")
{
  stack<int> st;
  st.push(1);
  REQUIRE(st.top()==1);
}

SCENARIO("pop", "[pop]")
{
  stack<int> st;
  st.push(1); 
  st.pop();
  REQUIRE(st.count()==0);
}

SCENARIO("cop", "[cop]")
{
  stack<int> st1;
  st1.push(1);
  stack<int> st2=st1;
  REQUIRE(st2.count()==1);
  REQUIRE(st2.top()==1);
}

SCENARIO("empty", "[empty]")
{
  stack<int> st1, st2;
  st1.push(1);
  REQUIRE(!st1.empty());
  REQUIRE(st2.empty());
}
