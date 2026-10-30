#pragma once
#include<iostream>
#include<ctime>
#include<map>
#include <cstdlib>

using namespace std;

class rolldice{
  public:
    rolldice(){ srand(std::time(nullptr)); }
    
    int rolldic (){
      return ( rand() % 6 ) +1 ;
    }
};