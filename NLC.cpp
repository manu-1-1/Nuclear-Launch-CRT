#include<iostream>
using namespace std;
using int64=long long;
using int128 = __int128_t;
//Utilities
class Pg{
public:
    static int isPrime(int64 x){
        if (x<2) {
                return 0;
        }
        if (x%2==0 && x!=2) {
                return 0;
        }
        for (int64 i=3;i*i<=x;i+=2){
            if (x%i==0) {
                    return 0;
            }
        return 1;
        }
    }
    static int64 np(int64 s){
        int64 p=(s<2)? 2:s;
        while(!isPrime(p)){
            ++p;
        }
        return p;
    }
};
class CryptoUtils{
public:
    static int64 gcd(int64 a, int64 b, int64 &x, int64 &y){
        if (b==0){
            x=1;
            y=0;
            return a;
        }
        int64 x1, y1;
        int64 g = gcd(b, a%b, x1, y1);
        x=y1;
        y=x1-(a/b)*y1;
        return g;
    }
    static int64 mod_inv(int64 a, int64 m){
        int64 x, y;
        int64 g=gcd((a%m+m)%m, m, x, y);
        if (g!=1) return -1;
        x=(x%m+m)%m;
        return x;
    }
};
class CRTsolver{
public:
    static int64 solver(const vector<int64>& mods, const vector<int64>& rems){
        int128 prod=1;
        for (auto m:mods)prod*=(int128)m;
        int128 result=0;
        for (size_t i=0; i < mods.size(); ++i){
            int128 m_i=mods[i];
            int128 r_i=rems[i];
            int128 p=prod/m_i;
            int64 inv=CryptoUtils::mod_inv((int64)(p%m_i), mods[i]);
            result+=r_i*(int128)inv*p;
        }
        return (int64)(result%prod);
    }
};
int main(){
    return 0;
}
