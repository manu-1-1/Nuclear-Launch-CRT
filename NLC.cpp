#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;
using int64=long long;

int64 mul_mod(int64 a,int64 b,int64 m){
    int64 res=0;
    a%=m;
    while(b>0){
        if(b%2==1) res=(res+a)%m;
        a=(a*2)%m;
        b/=2;
    }
    return res;
}

class Pg{
public:
    static int isPrime(int64 x){
        if(x<2) return 0;
        if(x%2==0 && x!=2) return 0;
        for(int64 i=3;i*i<=x;i+=2)
            if(x%i==0) return 0;
        return 1;
    }

    static int64 np(int64 s){
        int64 p=(s<2)?2:s;
        while(!isPrime(p)){
            ++p;
        }
        return p;
    }
};

class gi{
public:
    static int64 gcd(int64 a,int64 b,int64 &x,int64 &y){
        if(b==0){
            x=1;
            y=0;
            return a;
        }
        int64 x1,y1;
        int64 g=gcd(b,a%b,x1,y1);
        x=y1;
        y=x1-(a/b)*y1;
        return g;
    }

    static int64 mod_inv(int64 a,int64 m){
        int64 x,y;
        int64 g=gcd((a%m+m)%m,m,x,y);
        if(g!=1) return -1;
        x=(x%m+m)%m;
        return x;
    }
};

class CRTsolver{
public:
    static int64 solver(const vector<int64>& mods,const vector<int64>& rems){
        int64 prod=1;
        for(auto m:mods){
            prod*=m;
        }
        int64 result=0;
        for(size_t i=0;i<mods.size();++i){
            int64 m_i=mods[i];
            int64 r_i=rems[i];
            int64 p=prod/m_i;
            int64 inv=gi::mod_inv((p%m_i),mods[i]);

            if(inv==-1){
                return -1;
            }

            int64 term=mul_mod(r_i,inv,prod);
            term=mul_mod(term,p,prod);

            result=(result+term)%prod;
        }
        return result;
    }
};

struct Share{
    int id;
    int64 share;
    int64 mod;
};

void generate_shares(int n,int k){
    mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());
    vector<int64> mods;

    uniform_int_distribution<int64> prime_dist(1000,5000);
    int64 p=prime_dist(rng);

    while((int)mods.size()<n){
        p=Pg::np(p);
        mods.push_back(p);
        ++p;
    }

    vector<int64> sorted_mods=mods;
    sort(sorted_mods.begin(),sorted_mods.end());

    int64 ub=1;
    for(int i=0;i<k;i++){
        ub*=sorted_mods[i];
    }

    int64 lb=1;
    for(int i=n-k+1;i<n;i++){
        lb*=sorted_mods[i];
    }

    if(lb>=ub){
        ub=lb+1000;
    }

    uniform_int_distribution<int64> dist(lb+1,ub-1);
    int64 secret=dist(rng);

    cout<<"{";
    cout<<"\"secret\": \""<<secret<<"\",";
    cout<<"\"shares\": [";
    for(int i=0;i<n;i++){
        int64 sh=(int64)(secret%mods[i]);
        cout<<"{\"id\": "<<(i+1)<<", \"share\": \""<<sh<<"\", \"mod\": \""<<mods[i]<<"\"}";
        if(i<n-1) cout<<",";
    }
    cout<<"]}";
}

void reconstruct_secret(string json_input){
    vector<int64> mods;
    vector<int64> rems;

    size_t pos=0;
    while(true){
        size_t share_pos=json_input.find("\"share\":",pos);
        if(share_pos==string::npos) break;

        size_t start_quote=json_input.find("\"",share_pos+8);
        size_t end_quote=json_input.find("\"",start_quote+1);
        string share_str=json_input.substr(start_quote+1,end_quote-start_quote-1);

        size_t mod_pos=json_input.find("\"mod\":",end_quote);
        size_t mod_start_quote=json_input.find("\"",mod_pos+6);
        size_t mod_end_quote=json_input.find("\"",mod_start_quote+1);
        string mod_str=json_input.substr(mod_start_quote+1,mod_end_quote-mod_start_quote-1);

        try{
            rems.push_back(stoll(share_str));
            mods.push_back(stoll(mod_str));
        }catch(...){
            break;
        }

        pos=mod_end_quote;
    }

    int64 secret=CRTsolver::solver(mods,rems);

    cout<<"{";
    if(secret==-1){
        cout<<"\"error\": \"reconstruction failed\"";
    }else{
        cout<<"\"secret\": \""<<secret<<"\"";
    }
    cout<<"}";
}

int main(int argc,char* argv[]){
    if(argc<2){
        return 1;
    }

    string mode=argv[1];

    if(mode=="generate"){
        generate_shares(5,3);
    }else if(mode=="reconstruct"){
        string input;
        if(argc>=3){
            input=argv[2];
        }else{
            stringstream buffer;
            buffer<<cin.rdbuf();
            input=buffer.str();
        }
        reconstruct_secret(input);
    }

    return 0;
}
