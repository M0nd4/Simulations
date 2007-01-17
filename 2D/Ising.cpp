
#include <math.h>
#include <time.h>
#include <vb.h>

using namespace vb;

int main(int argc, char *argv[])
{
  long i,x,y,xy,f1,f2,nstep;
  long p[5];
  char title[80];

  /* arguments -> beta et n */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : Ising <beta> <n>\n");
    fprintf(stderr, "   (beta_critical = log(1+sqrt(2)) = 0.88137359...)\n");
    exit(1);
  }

  double beta=atof(argv[1]);
  long n=atoi(argv[2]);

  sprintf(title,"An Ising configuration (beta=%6f)",beta);
  Image img(n,n,1,title);

  /* conditions initiales / au bord */

#define LEFT 0
#define RIGHT 1

  if (beta > log(1+sqrt(2.0))) { /* Basse temp. */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
	img(x,y) = (x<(n>>1)?0:1);
      }
    }
  } else { /* Haute temp. */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
	img(x,y) = lrand48()%2;
      }
    }
  }

  for (i=0;i<(n>>1);i++) { img(i,0)=LEFT; img(i,n-1)=LEFT; }
  for (i=0;i<n;i++) { img(0,i)=LEFT; }
  for (i=(n>>1);i<n;i++) { img(i,0)=RIGHT; img(i,n-1)=RIGHT; }
  for (i=0;i<n;i++) { img(n-1,i)=RIGHT; }

  img.onscreen();

  /* Initialisations */

  srand48(time(0));
  p[0]=0;
  p[1]=(long)(exp(-beta) * (double)RAND_MAX);
  p[2]=(long)(exp(-2*beta) * (double)RAND_MAX);
  p[3]=(long)(exp(-3*beta) * (double)RAND_MAX);
  p[4]=(long)(exp(-4*beta) * (double)RAND_MAX);

  /* Simulation */

  nstep = (int) ( 10 + n * 0.01 / fabs(beta-log(1.0+sqrt(2.0))) );

  for (i=0;i<nstep;i++) { /* Ouais bof */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
	f1 = 0; f2 = 0; xy = x + n*y;
	
	if (img(xy) == img(xy-1)) { f2++; } else { f1++; } 
	if (img(xy) == img(xy-n)) { f2++; } else { f1++; } 
	if (img(xy) == img(xy+1)) { f2++; } else { f1++; } 
	if (img(xy) == img(xy+n)) { f2++; } else { f1++; } 
	
	if ( (f2<=f1) || ((lrand48()%RAND_MAX) < p[f2-f1]) ) {
	  img.putpoint(x,y,(1-img(xy)));
	}
      }
    }
  }
  
  std::cout << img;

  return 0;
}
