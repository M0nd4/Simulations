
#include <math.h>
#include <time.h>
#include <vb.h>

using namespace vb;

int *fifox,*fifoy;

void cnx (Image &img, int ox, int oy, char in, char out)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img(ox,oy)=in; 

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img.pitch;
    imin++;
    if ((i<img.width-1)&&(img(k+1)!=in)&&(img(k+1)!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img(k+1)=in;
    }
    if ((i>0)&&(img(k-1)!=in)&&(img(k-1)!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img(k-1)=in;
    }
    if ((j<img.height-1)&&(img(k+img.pitch)!=in)&&(img(k+img.pitch)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img(k+img.pitch)=in;
    }
    if ((j>0)&&(img(k-img.pitch)!=in)&&(img(k-img.pitch)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img(k-img.pitch)=in;
    }
  }  
}

void bndcnx (Image &img, int ox, int oy)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img(ox,oy) |= 3;

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img.pitch;
    imin++;
    if ((i<img.width-1)&&(img(k)&16)&&!(img(k+1)&2)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img(k+1)|=3;
    }
    if ((j<img.height-1)&&(img(k)&32)&&!(img(k+img.pitch)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img(k+img.pitch)|=3;
    }
    if ((i>0)&&(img(k-1)&16)&&!(img(k-1)&2)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img(k-1)|=3;
    }
    if ((j>0)&&(img(k-img.pitch)&32)&&!(img(k-img.pitch)&2)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img(k-img.pitch)|=3;
    }
  }  
}

int main(int argc, char ** argv)
{
  int n,i;
  char title[80];
  double p;

  /* arguments -> p et n */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : Perco <p> <n>\n");
    exit(1);
  }
  p=atof(argv[1]);
  n=atoi(argv[2]);

  sprintf(title,"Bond-percolation cluster (p=%f)",p);
  Image img (2*n,2*n,2,title);
  srand48(time(0));

  /* Bond-percolation --- 16->est, 32->nord */

  for (int x=0;x<2*n;++x)
    for (int y=0;y<2*n;++y) {
      if ( drand48() < p ) img(x,y)=17; else img(x,y)=1;
      if ( drand48() < p ) img(x,y)+=32;
    }

  /* 3=cluster de l'origine */

  fifox = new int [4*img.width*img.height];
  fifoy = new int [4*img.width*img.height];
  bndcnx(img,n,n);

  /* Oublier le r�seau */

  for (int x=0;x<2*n;++x)
    for (int y=0;y<2*n;++y)
      img(x,y)&=3;

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if (img(i,0)!=3)
			cnx(img,i,0,0,3);
  for (i=0;i<2*n;i++) if (img(0,i)!=3)
			cnx(img,0,i,0,3);
  for (i=0;i<2*n;i++) if (img(2*n-1,i)!=3)
			cnx(img,img.width-1,i,0,3);
  for (i=0;i<2*n;i++) if (img(i,2*n-1)!=3)
			cnx(img,i,img.height-1,0,3);

  /* affichage du resultat */

  std::cout << img;
  return 0;
}
