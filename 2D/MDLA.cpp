#include <vb/Image.h>

#define EMPTY  Color(0)
#define AWAY   Color(50)
#define ALIVE  Color(0,200,0)
#define DEAD   Color(255,0,0)

using namespace vb;

int main (int argc, char ** argv) {
    H.init ("Aggregation of exclusion walkers", argc, argv, "n=250,p=.5,g,a=1.0,t=0.0,s");
    int    n = H['n']; // Half of board size
    double p = H['p']; // Initial particle density
    bool   g = H['g']; // Dynamic discovery of environment (ghosts)
    double t = H['t']; // Snapshot interval for movies
    double a = H['a']; // Contagion probability

    Image img (2*n,2*n);

    for (Color & c : img) if (g) c = AWAY; else c = prng.bernoulli(p) ? ALIVE : EMPTY;
    img.put (coo(n,n),DEAD); img.show();
    if (t>0) img.snapshot_setup ("MDLA",t);

    while (true) {
        coo z = img.rand(), nz = z + dz[prng()%4];
        if (!img.contains(nz)) continue;
        if (img.at(z) == img.at(nz)) continue;

        bool flag = false;
        if (img.at(z) == AWAY)  {
            if (prng.bernoulli(a) || (img.at(nz) == DEAD)) { flag = true; img.put (z,(prng.bernoulli(p)?ALIVE:EMPTY)); }
            else continue;
        }
        if (img.at(nz) == AWAY)  {
            if (prng.bernoulli(a) || (img.at(z) == DEAD)) { flag = true; img.put (nz,(prng.bernoulli(p)?ALIVE:EMPTY)); }
            else continue;
        }

        if (img.at(z) == img.at(nz)) continue;
        if (img.at(z) != ALIVE) { coo t=z; z=nz; nz=t; }
        if (img.at(z) != ALIVE) continue;

        if (img.at(nz) == EMPTY) { if (prng.bernoulli(a) || flag) { img.put (z,EMPTY); img.put (nz,ALIVE); } }
        else if (img.at(nz) == DEAD) { img.put (z,DEAD); if ((z.x==0)||(z.x==2*n-1)||(z.y==0)||(z.y==2*n-1)) break; }
    }
}
