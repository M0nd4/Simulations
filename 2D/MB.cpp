/*
 * Simulation d'une marche simple dans Z^2, tu�e � la sortie d'un
 * carr� de c�t� donn�. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vb.h>

#define EC fprintf(stderr,"<%d>\n",__LINE__);

using namespace vb;

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

int main(int argc, char ** argv)
{
  Image *img;              /* L image */
  char title[80];          /* Titre ... */
  int n;                   /* Taille du carr� */
  char d;                  /* La chaine */
  
  long i,x,y,done;

  /* arguments -> taille du terrain */

  if (argc != 2) {
    fprintf(stderr, "Syntaxe : MB <n>\n");
    exit(1);
  }
  n=atoi(argv[1]);

  /* Initialisations */

  srand48(time(0));

  sprintf(title,"A Brownian path of size %d",n);
  img = new Image(2*n,2*n,1,title);

  /* Simulation */

  i=0; done=0; x=n; y=n;
  while (!done) {
    d = ((lrand48()%49583)>>3)&3;
    img->putpoint (x,y,1);
    x += dx[(int)d];
    y += dy[(int)d];
    /*  fprintf(stderr,"%8ld\r",++i); */

    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }

  img->cropped = 1;
  std::cout << *img;
  return 0;
}
