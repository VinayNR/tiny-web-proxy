// C++ program to convert string 
// to char array using .data() 
#include <iostream> 
#include <string> 

using namespace std;
  
// Driver Code 

void func(const char * d) {
    cout << strlen(d) << endl;
}

int main() 
{ 
    std::string s = "Geeks\0ForGeeks"; 
  
    char* char_arr = s.data(); 
  
    std::cout << strlen(char_arr);

    func(s);
  
    return 0; 
}