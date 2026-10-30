#pragma once
#include<iostream>
#include<ctime>
#include<map>
#include <cstdlib>

using namespace std;
   
class snakesandladders {
 public:

 map <int,int> snakes;
 map < int , int > ladders;

 
   snakesandladders(){

   ladders[4]= 14;
   ladders[9]= 31;
   ladders[20]= 38;
   ladders[28]= 84;
   ladders[40]= 59;
   ladders[51]= 67;
   ladders[63]= 81;
   ladders[71]= 91;


    snakes[17]= 7;
    snakes[54]= 34;
    snakes[62]= 19;
    snakes[64]= 60;
    snakes[87]= 24;
    snakes[93]= 73;
    snakes[95]= 75;
    snakes[99]= 78;
    
   }

};