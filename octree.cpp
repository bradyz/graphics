#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include "Sphere.h"

using namespace std;

struct Data {
    double r;
    vector<double> c;
    int id;
} md;

struct Two {
    int a, b;
    bool operator < (const Two &other) const {
        return a < other.a or (a==other.a and b < other.b);
    }
} mt;

inline double maxx(double a, double b){
    return a > b ? a : b;
}

inline double minn(double a, double b){
    return a < b ? a : b;
}

vector<Two> ans;
set<Two> setans;
const int MAXLEAF = 50;
const int MAXLEVEL = 20, MINEDGE = .0000001;

bool collide(Data one, Data two){
    double sum = 0;
    for(int e = 0; e<3; e++){
        double save = (one.c[e] - two.c[e]);
        sum = sum + save*save;
    }
    if(sum <= (one.r+two.r)*(one.r+two.r)){
        return true;
    }
    return false;
}

void doBruteForce(vector<Data> spheres){
     for(int i=0; i<spheres.size(); i++){
        for(int e=i+1; e<spheres.size(); e++){
            if(collide(spheres[i], spheres[e])){
                mt.a = spheres[i].id;
                mt.b = spheres[e].id;
                setans.insert(mt);
            }
        }
    }
}


void check(vector<Data> spheres, vector<double> minc, vector<double> maxc, int level){
    if(spheres.size() == 0) return;
    //cout<<minc[0]<<" "<<minc[1]<<" and "<<maxc[0]<<" "<<maxc[1]<<" SIZE "<<spheres.size()<<endl;
    
    /*double minedge = 1e100;
    for(int i=0; i<2; i++){
        minedge = minn(minedge, maxc[i]-minc[i]);
    }*/

    if(spheres.size() < MAXLEAF or level==MAXLEVEL){
        doBruteForce(spheres);
        return ;
    }
    vector<double> median;
    for(int e=0; e<3; e++)
        median.push_back((maxc[e]-minc[e])/2);
    
    vector<int> index;
    for(int e=0; e<3; e++) index.push_back(0);


    vector<Data> rec[8];
    vector<double> minc2[8], maxc2[8];

    int many = 0;
    for(int x = 0; x<=1; x++){
        index[0] = x;
        for(int y = 0; y<=1; y++){
            index[1] = y;
            for(int z = 0; z<=1; z++){
                index[2] = z;

                minc2[many].clear();
                maxc2[many].clear();
                rec[many].clear();

                for(int e = 0; e<3; e++){
                    minc2[many].push_back(minc[e] + index[e] * median[e]);
                    maxc2[many].push_back(minc2[many][e] + median[e]);
                }

                for(int i=0; i<spheres.size(); i++){
                    bool touches = true;
                    for(int e = 0; e<3; e++){
                        if(spheres[i].c[e] - spheres[i].r > maxc2[many][e])
                            touches=false;
                        if(spheres[i].c[e] + spheres[i].r < minc2[many][e])
                            touches = false;
                    }
                    if(touches){
                        rec[many].push_back(spheres[i]);
                    }
                }
                ++many;
                //check(sphe, minc2, maxc2, level+1);
            }
        }
    }
    double manyops = 0;
    int dif=0;
    for(int i=0; i<8; i++){
        if(rec[i].size() > 0) ++dif;
        manyops = manyops + rec[i].size()*rec[i].size();
    }
    long long bruteops = spheres.size()*spheres.size();
    if(manyops > bruteops and bruteops < 10000 and dif > 1){
        //cout<<"dobrute"<<endl;
        doBruteForce(spheres);
        return;
    }
    else{
        //cout<<"pray"<<endl;
        for(int i=0; i<8; i++){
            check(rec[i], minc2[i], maxc2[i], level+1);
        }
    }
}

vector<Two> sloww(vector<Data> spheres){
    setans.clear();
    
    for(int i=0; i<spheres.size(); i++){
        spheres[i].id = i;
    }

    doBruteForce(spheres);

    ans.clear();
    for(set<Two>::iterator it = setans.begin(); it != setans.end(); it++){
        ans.push_back(*it);
    }
    return ans;
}

vector<Two> intersections(vector<Data> spheres){
    setans.clear();
    if(spheres.size() == 0) return ans;

    for(int i=0; i<spheres.size(); i++){
        spheres[i].id = i;
    }

    vector<double> minc;
    vector<double> maxc;
    for(int i=0; i<3; i++){
        minc.push_back(1e100);
        maxc.push_back(1e100 * -1);
    }
    // get bounding box of all spheres
    for(int i=0; i<spheres.size(); i++){
        for(int e=0; e<3; e++){
            minc[e] = minn(minc[e], spheres[i].c[e]);
            maxc[e] = maxx(maxc[e], spheres[i].c[e]);
        }
    }
    
    check(spheres, minc, maxc, 0);

    ans.clear();
    for(set<Two>::iterator it = setans.begin(); it != setans.end(); it++){
        ans.push_back(*it);
    }
    return ans;
}

vector<Two> getCollisions(vector<Sphere> spheres){
    vector<Data> temp;
    for(int i=0; i<spheres.size(); i++){
        md.r = spheres[i].radius;
        for(int e=0; e<3; e++){
            md.c[e] = spheres[i].position[e];
        }
        temp.push_back(md);
    }
    return intersections(temp);
}

double randd(int ma){
    double s = 1.0 * rand() / 1000;
    int h = s;
    return s - h + h%ma;
}
/*
int main(){
    /*double r = 2;
    int ctos;
    cin>>ctos;
    vector<double> es;
    vector<Data> inp;
    for(int i=0; i<3; i++) es.push_back(0);
    for(int i=1; i<=ctos; i++){
        int a, b;
        cin>>a>>b;
        md.r = r;
        es[0]=a;
        es[1]=b;
        md.c = es;
        inp.push_back(md);
    }
    cout<<"Collisions:\n";
    vector<Two> s = intersections(inp);
    for(int i=0; i<s.size(); i++){
        cout<<s[i].a<<" and "<<s[i].b<<endl;
    }


    //srand (time(NULL));
    int ctos;
    cin>>ctos;
    vector<double> es;
    for(int i=0; i<3; i++) es.push_back(0);
    vector<Data> inp;
    for(int i=1; i<=ctos; i++){
        md.r = randd(1);
        es[0]=randd(20);
        es[1]=randd(20);
        //es[2]=randd(20);
        md.c = es;
   //     cout<<md.r<<": "<<es[0]<<", "<<es[1]<<", "<<es[2]<<endl;
        inp.push_back(md);
    }

    cout<<"starting fast"<<endl;
    vector<Two> fast = intersections(inp);
    cout<<"ok\nstarting slow"<<endl;
    vector<Two> slow = sloww(inp);
    cout<<"ok"<<endl;
    cout<<fast.size()<<" vs "<<slow.size()<<endl;
    cout<<"..."<<endl;
    if(fast.size() != slow.size()){
        for(int i=0; i<fast.size(); i++){
            cout<<fast[i].a<<" - "<<fast[i].b<<endl;
        }
        cout<<"is not"<<endl;
        for(int i=0; i<slow.size(); i++){
            cout<<slow[i].a<<" - "<<slow[i].b<<endl;
        }

        cout<<"Wrong Answer"<<endl;
        return 0;
    }
    for(int i=0; i<fast.size(); i++){
        if(fast[i].a != slow[i].a or fast[i].b != slow[i].b){
            cout<<fast[i].a<<", "<<fast[i].b<<" not equal to "<<slow[i].a<<" "<<slow[i].b<<endl;
            cout<<"Wrong Answer"<<endl;
            return 0;
        }
    }
    cout<<"Accepted"<<endl;
    return 0;
}*/
