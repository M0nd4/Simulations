
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <vb/Figure.h>

using namespace vb;

double omx = sqrt(3.0);

class Perco_Schramm : public vb::Figure {
public:
  int w, h;
  std::vector<bool> cols;
  cpx pos;
  
  Perco_Schramm (int w_, int h_) : w(w_), h(h_) {
    for (int i=0; i < w/2; ++i)     cols.push_back (true);
    for (int i=0; i < w/2; ++i)     cols.push_back (false);
    for (int i=0; i < (w-1)*h; ++i) cols.push_back (rand()<rand());
  }
  
  void lineto (cpx xy) { segment (pos,xy); pos = xy; }
  
  void hex (cpx xy) {
    pos = xy + cpx(-omx,-1);
    lineto (pos + cpx(0,2));  lineto (pos + cpx(omx,1));   lineto (pos + cpx(omx,-1));
    lineto (pos + cpx(0,-2)); lineto (pos + cpx(-omx,-1)); lineto (pos + cpx(-omx,1));
  }
  
  cpx thepos (int i) { return cpx(omx*(((i/w)%2)+2*(i%w)) , 3*(i/w)); }
  
  void perc () { for (int i=0; i<w*h; ++i) if (cols[i]) hex(thepos(i)); }
  
  int follow (int base, int dir) {
    static int fola[6] = { 1, w, w-1, -1, -w-1, -w };
    static int folb[6] = { 1, w+1, w, -1, -w, -w+1 };
    return ((base/w)%2 ? folb : fola) [dir] + base;
  }
  
  int thenext (int base, int dir) { return follow (base, (dir+1)%6); }
  
  void seg (int base, int dir, int rot) {
    cpx x1y1 = thepos(base);
    cpx x2y2 = thepos(follow(base,dir));
    cpx x3y3 = thepos(follow(base,(dir+rot)%6));
    segment ((x1y1+x2y2)*.5, (x1y1+x2y2+x3y3)*(1.0/3));
  }

  void walk () {
    int base = w/2-1, dir = 0;
    while (((base+1)%w >= 2) && (base/w <= h-2)) {
      seg (base,dir,1);
      if (cols[thenext(base,dir)]) { base = thenext(base,dir); dir = (dir+5)%6; }
      else                         {                           dir = (dir+1)%6; }
      seg (base,dir,5);
    }
  }  
};

int main (int argc, char ** argv) {
  Perco_Schramm RS (60,70);
  RS.walk(); RS.show(); RS.pause();
  return 0;
}
