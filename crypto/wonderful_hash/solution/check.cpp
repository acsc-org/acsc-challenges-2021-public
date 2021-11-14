#include <bits/stdc++.h>
using namespace std;
vector<pair<unsigned long long, unsigned int>> vt1;
vector<pair<unsigned long long, unsigned int>> vt2;
int main() {
    char x[11];
    unsigned int val;
    unsigned long long t;
    int i;
    FILE *f;
    int length;
    char fname[256];
    
    for(int cnt=0;cnt<256;cnt++) {
        vt1.clear();
        vt2.clear();
	    sprintf(fname, "out1/val_%02d", cnt);
	    f = fopen(fname, "r");
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        length = length / 9;
        for(i=0;i<length;i++){
            fread(x,1, 9, f);
            t = 0;
            val = 0;
            memcpy(&t ,x,5);
            memcpy(&val ,x+5,4);
            vt1.emplace_back(make_pair(t, val));
        }
        sort(vt1.begin(), vt1.end());
	    sprintf(fname, "out2/val_%02d", cnt);
	    f = fopen(fname, "r");
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        length = length / 9;
        for(i=0;i<length;i++){
            fread(x,1, 9 ,f);
            t = 0;
            val = 0;
            memcpy(&t ,x,5);
            memcpy(&val ,x+5,4);
            vt2.emplace_back(make_pair(t, val));
        }
        sort(vt2.begin(), vt2.end());
        int i1 = 0, i2 = 0;
        while(i1<vt1.size() && i2 < vt2.size()) {
          if(vt1[i1].first<vt2[i2].first) i1++;
          else if(vt1[i1].first>vt2[i2].first) i2++;
          else {
            printf("Found!\n");
            printf("%u %u %llu\n", vt1[i1].second, vt2[i2].second, vt1[i1].first);
            i1++;
            i2++;
          }
        }
    }
}

