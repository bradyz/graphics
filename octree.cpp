#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>
#include <time.h>

#include "octree.h"
#include "Sphere.h"
#include "RigidBody.h"
#include "BoundingBox.h"

using namespace std;

OctTreeNode::OctTreeNode (const vector<RigidBody*>& newObjects,
                          BoundingBox bbox, int level) :
  box(bbox) {
  for (int i = 0; i < 8; ++i)
    cells[i] = NULL;

  cout << newObjects.size() << " " << level << endl;

  if (newObjects.size() <= 3 || level > 5)
    return;

  float dx = (box.maxVals[0] - box.minVals[0]) / 2.0f;
  float dy = (box.maxVals[1] - box.minVals[1]) / 2.0f;
  float dz = (box.maxVals[2] - box.minVals[2]) / 2.0f;

  // must be uniform
  assert(abs(dx - dy) < 1e-5 && abs(dy - dz) < 1e-5);

  vector<RigidBody*> items[8];
  BoundingBox boxes[8];

  int idx = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        glm::vec3 tmpMin = box.minVals + glm::vec3(i * dx, j * dx, k * dx);
        boxes[idx++] = BoundingBox(tmpMin, tmpMin + dx);
      }
    }
  }

  for (RigidBody* obj: newObjects) {
    for (int i = 0; i < 8; ++i) {
      if (boxes[i].intersects(obj->position)) {
        items[i].push_back(obj);
      }
    }
  }

  for (int i = 0; i < 8; ++i) {
    if (items[i].size() > 0)
      cells[i] = new OctTreeNode(items[i], boxes[i], level+1); 
  }
}

void OctTreeNode::getAllBoxes (vector<BoundingBox>& allBoxes) const {
  float dx = (box.maxVals[0] - box.minVals[0]) / 2.0f;

  int idx = 0;
  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        glm::vec3 tmpMin = box.minVals + glm::vec3(i * dx, j * dx, k * dx);
        allBoxes.push_back(BoundingBox(tmpMin, tmpMin + dx));

        if (cells[idx] != NULL)
          cells[idx]->getAllBoxes(allBoxes);

        ++idx;
      }
    }
  }
}

Two mt;

struct Data {
    double r;
    vector<double> c;
    int id;
} md;

inline double maxx(double a, double b){
    return a > b ? a : b;
}

inline double minn(double a, double b){
    return a < b ? a : b;
}

vector<Two> ans;
set<Two> setans;
const int MAXLEAF = 50;
const int MAXLEVEL = 20;

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

vector<Two> getCollisions (vector<Sphere>& spheres) {
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
