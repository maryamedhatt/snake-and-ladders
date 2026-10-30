#pragma once
#include<iostream>
#include<ctime>
#include<map>
#include <cstdlib>

using namespace std;
class player{
    private:
    string name_;
    int position_;
    
    public:
   
     player(){
      int winnerscore=100;
     }
     
     explicit player(string name) : name_(name) , position_(0){}

     string getname() const{return name_;}
     int getposition() const {return position_;}

     void moveby(int steps){
        position_ += steps;
     }

     bool haswon() const{
        return position_ == 100;
     }
    
};
