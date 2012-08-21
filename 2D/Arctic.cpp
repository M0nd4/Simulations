
#include <vb/Bitmap.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

#define ZZ 0
#define V1 1
#define V2 2
#define H1 3
#define H2 4

#define C_ZZ Color(0,0,0)
#define C_V1 Color(0,255,0)
#define C_V2 Color(0,0,255)
#define C_H1 Color(255,0,0)
#define C_H2 Color(255,255,0)

Color colors[5] = { C_ZZ, C_V1, C_V2, C_H1, C_H2 };

class Site {
public:
  unsigned char s;
  Site (unsigned char _s) : s(_s) { };
  void operator= (unsigned char _s) { s=_s; }
  operator int () { return s; }
  operator Color () { return colors[s]; }
};

class World : public Bitmap<Site> {
public:
  World (CL_Parser &CLP) : Bitmap<Site> (CLP('n'),CLP('n'),"Domino tilings in the plane") {
    if (string(CLP('o')) == "aztec") {
      for (int x=0; x<w()/2; ++x)
        for (int y=h()/2-1-x; y<h()/2+1+x; ++y)
          at(x,y)=V1;
      for (int x=w()/2; x<w(); ++x)
        for (int y=x-w()/2; y<h()+w()/2-x; ++y)
          at(x,y)=V2;
    } else {
      for (int x=0; x<w(); ++x) {
        for (int y=0; y<h(); ++y) {
          if (y%2) at(x,y)=H1; else at(x,y)=H2;
        }
      }
    }
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=600,o=box");
  World w(CLP);
  w.show();
  w.pause();

  w.snapshot_setup("arctic",5.0);

  while (true) {
    int x = prng.uniform_int(w.w()-1);
    int y = prng.uniform_int(w.h()-1);
    if ((x+y)%2) {
      if ( (w.at(x,y)==H1) && (w.at(x+1,y)==H1) && (w.at(x,y+1)==H2) && (w.at(x+1,y+1)==H2) ) {
        w.at(x,y)=V1; w.at(x,y+1)=V1; w.at(x+1,y)=V2; w.at(x+1,y+1)=V2; w.step();
      } else if ( (w.at(x,y)==V1) && (w.at(x,y+1)==V1) && (w.at(x+1,y)==V2) && (w.at(x+1,y+1)==V2) ) {
        w.at(x,y)=H1; w.at(x+1,y)=H1; w.at(x,y+1)=H2; w.at(x+1,y+1)=H2; w.step();
      }
    } else {
      if ( (w.at(x,y)==H2) && (w.at(x+1,y)==H2) && (w.at(x,y+1)==H1) && (w.at(x+1,y+1)==H1) ) {
        w.at(x,y)=V2; w.at(x,y+1)=V2; w.at(x+1,y)=V1; w.at(x+1,y+1)=V1; w.step();
      } else if ( (w.at(x,y)==V2) && (w.at(x,y+1)==V2) && (w.at(x+1,y)==V1) && (w.at(x+1,y+1)==V1) ) {
        w.at(x,y)=H2; w.at(x+1,y)=H2; w.at(x,y+1)=H1; w.at(x+1,y+1)=H1; w.step();
      }
    }
  }
}