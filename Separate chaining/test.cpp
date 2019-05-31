#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "ADS_set.h"
 
using namespace std;

 int main(){
 
ADS_set<size_t> test {7,1,5,3,0,15,100,250,45,70};
   for(auto element: test){
     cout << " element = " << element <<endl;;
   }
   
 return 0;

}   
