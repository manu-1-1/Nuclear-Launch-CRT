#include<iostream>
using namespace std;
using int64=long long;

//Utilities
class Pg{
public:
    static int isPrime(int64 x){
        if (x<2) return 0;
        if (x%2==0 && x!=2) return 0;
        for (int64 i=3;i*i<=x;i+=2)
            if (x%i==0) return 0;
        return 1;
    }

    static int64 np(int64 s){
        int64 p=(s<2)? 2:s;  //clamping if s<2
        while(!isPrime(p)){
            ++p;
        }
        return p;
    }
};


int main(){
    return 0;
}
