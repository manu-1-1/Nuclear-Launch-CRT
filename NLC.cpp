#include<iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
using namespace std;
using int64=long long;
using int128 = __int128_t;

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
        x = y1;
        y = x1-(a/b)*y1;
        return g;
    }

    static int64 mod_inv(int64 a, int64 m){
        int64 x, y;
        int64 g = gcd((a%m+m)%m, m, x, y);
        if (g != 1) return -1;
        x = (x%m+m) % m;
        return x;
    }
};

class CRTsolver{
public:
    static int64 solver(const vector<int64>& mods, const vector<int64>& rems){
        int128 prod=1;
        for (auto m:mods)prod *= (int128)m;
        int128 result = 0;
        for (size_t i = 0; i < mods.size(); ++i){
            int128 m_i = mods[i];
            int128 r_i = rems[i];
            int128 p = prod / m_i;
            int64 inv = CryptoUtils::mod_inv((int64)(p%m_i), mods[i]);
            if (inv == -1) {
                cout<<"Error: Could not find modular inverse"<<endl;
                return -1;
            }
            result += r_i*(int128)inv*p;
        }
        return (int64)(result%prod);
    }
};

class user{
protected:
    int id;
    int64 mod;
    int64 share;

public:
    user(int i){
        id=i;
        mod=0;
        share=0;
    }
    virtual void eS(){
        cout<<"Participant "<<id<<" entering share..."<<endl;
    }
    int getId(){
        return id;
    }
    int64 getMod(){
        return mod;
    }
    int64 getShare(){
        return share;
    }
};

class Minister : public user{
public:
    Minister(int i) : user(i){}
    void setShare(int64 val, int64 m){
        share = val;
        mod=m;
    }
    void setShare(int64 val){
        share = val;
    }
    int verify(int64 input){
        if(input==share){
            return 1;
        }else{
            return 0;
        }
    }

    void eS() override{
        int64 val;
        cout<<"Minister "<<id<<", enter your share value: ";
        cin>>val;
        if (verify(val)){
            cout<<"Accepted from Minister "<<id<<endl;
        }else{
            cout<<"Rejected from Minister "<<id<<endl;
        }
    }
    friend class CRTsolver;
};

class SS{
private:
    const int n;
    const int k;
    int128 secret;
    vector<Minister> ministers;

public:
    SS(int total, int threshold) : n(total), k(threshold) {
        for(int i=1;i<=n;i++){
            ministers.push_back(i);
        }
    }

    void create(){
        mt19937_64 rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());
        vector<int64> mods;
        int64 p = 101;
        while((int)mods.size() < n){
            p=Pg::np(p);
            mods.push_back(p);
            ++p;
        }
        vector<int64> sorted_mods = mods;
        sort(sorted_mods.begin(), sorted_mods.end());

        int128 ub= 1;
        for(int i = 0; i < k; i++) {
            ub *= (int128)sorted_mods[i];
        }

        int128 lb= 1;
        for(int i = n-k+1; i < n; i++) {
            lb *= (int128)sorted_mods[i];
        }
        uniform_int_distribution<long long> dist(1, (long long)(ub-1));
        secret = dist(rng);
        for(int i=0;i<n;i++){
            int64 sh = (int64)(secret % (int128)mods[i]);
            ministers[i].setShare(sh, mods[i]);
        }

        cout<<"\n[System] Secret (hidden in real system): "<<(int64)secret<<endl;
        cout<<"\nDistributed shares\n";
        for(int i=0;i<n;i++){
            cout<<"Minister "<<ministers[i].getId()<<": m="<<ministers[i].getMod()<<"  share="<<ministers[i].getShare()<<"\n";
        }
        cout<<endl;
    }

    void launch(){
        int users;
        cout<<"\nHow many ministers will enter their shares today? ";
        cin>>users;
        if (users < k){
            cout<<"Not enough participants. Launch Aborted!\n";
            return;
        }
        if (users > n){
            cout<<"Participants > total ministers. Launch Aborted!\n";
            return;
        }
        vector<int> c(n,0);
        vector<int64> usedmods;
        vector<int64> usedrems;
        int collect = 0;
        while(collect < users){
            int i;
            cout<<"\nEnter minister index (1-"<<n<<"): ";
            cin>>i;
            if(i<1 || i>n){
                cout << "Invalid index. Try again.\n";
                continue;
            }
            if(c[i-1]){
                cout<<"Minister already submitted. Pick another.\n";
                continue;
            }

            cout<<"Enter share value: ";
            int64 val;
            cin>>val;
            if(!ministers[i-1].verify(val)){
                cout<<"\nIncorrect share by Minister "<<i<<" LAUNCH ABORTED.\n";
                return;
            }
            cout<<"Accepted from Minister "<<i<<endl;
            c[i-1] = 1;
            usedmods.push_back(ministers[i-1].getMod());
            usedrems.push_back(val);
            ++collect;
        }

        int64 r = CRTsolver::solver(usedmods, usedrems);
        if (r == -1){
            cout<<"\nReconstruction failed. Launch Aborted!\n";
            return;
        }
        cout<<"\nReconstructed = "<<r<< endl;
        if(r == (int64)secret){
            cout<<"Launch Authorized!\n";
        }else{
            cout<<"Reconstruction mismatch, Launch Aborted!\n";
        }
    }
};

int main(){
    cout<<"CRT Mignotte Secret-Sharing Nuclear Launch\n";
    SS s(5,3);
    s.create();
    s.launch();
    return 0;
}
