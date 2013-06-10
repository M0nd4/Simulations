#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/common.h> // fmt

#define EC fprintf(stderr,"<%d>\n",__LINE__);

#define EMPTY  Color(0)
#define AWAY   Color(50)
#define ALIVE  Color(0,200,0)
#define DEAD   Color(255,0,0)

using namespace vb;

int main(int argc, char ** argv) {
  Image *img;              /* L image */
  char d;                  /* La chaine */

  long x,y,nx,ny,done,ndraw,ngrey;

  CL_Parser CLP (argc, argv, "n=250,p=.5,g,a=1.0,t=0.0,s");
  int    n = CLP('n');
  double p = CLP('p');
  bool   g = CLP('g');
  double a = CLP('a');
  double t = CLP('t');
  bool   s = CLP('s');

  /* Initialisations */

  img = new Image(2*n,2*n, str(fmt("A MDLA in density %f")%p));
  if (t>0) img->snapshot_setup ("MDLA",t);

  ngrey = 0;

  if (g) {
    for (x=0;x<2*n;++x)
      for (y=0;y<2*n;++y)
        img->putpoint (x,y,AWAY);
  } else {
    for (x=0;x<2*n;x++) {
      for (y=0;y<2*n;y++) {
        if (prng.bernoulli(p)) {
          img->putpoint (x,y,ALIVE);
          if ((x!=n)||(y!=n)) ngrey++;
        } else
          img->putpoint (x,y,EMPTY);
      }
    }
  }
  img->putpoint (n,n,DEAD);

  img->show();

  /* Simulation */

  done=0; ndraw=0;

  bool running = true;
  while (running) {
    bool flag = false;
    x = prng() % (2*n);
    y = prng() % (2*n);

    d = prng() &3;
    nx = x + dx[(int)d]; if ((nx<0)||(nx>=2*n)) continue;
    ny = y + dy[(int)d]; if ((ny<0)||(ny>=2*n)) continue;

    if ((*img).at(x,y) == (*img).at(nx,ny)) continue;

    if ((*img).at(x,y) == AWAY)  {
      if (prng.bernoulli(a) || ((*img).at(nx,ny) == DEAD)) { flag = true; img->putpoint (x,y,(prng.bernoulli(p)?ALIVE:EMPTY)); }
      else continue;
    }
    if ((*img).at(nx,ny) == AWAY)  {
      if (prng.bernoulli(a) || ((*img).at(x,y) == DEAD)) { flag = true; img->putpoint (nx,ny,(prng.bernoulli(p)?ALIVE:EMPTY)); }
      else continue;
    }

    if ((*img).at(x,y) == (*img).at(nx,ny)) continue;
    if ((*img).at(x,y) != ALIVE) { int t=x; x=nx; nx=t;  t=y; y=ny; ny=t; }
    if ((*img).at(x,y) != ALIVE) continue;

    if ((*img).at(nx,ny) == EMPTY) {
      if (prng.bernoulli(a) || flag) {
        img->putpoint (x,y,EMPTY);
        img->putpoint (nx,ny,ALIVE);
      }
    } else if ((*img).at(nx,ny) == DEAD) {
      img->putpoint (x,y,DEAD);
      if ((s)&&((x==0)||(x==2*n-1)||(y==0)||(y==2*n-1))) running = false;
      --ngrey;
    }
  }

  return 0;
}
