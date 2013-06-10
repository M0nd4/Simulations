#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/common.h> // fmt

using namespace vb;

int *fifox,*fifoy;

void cnx (Image *img, int ox, int oy, Color in, Color out)
{
  int imin,imax;
  int i,j,k;

  imin=0; imax=0;
  fifox[0]=ox; fifoy[0]=oy;
  img -> putpoint (ox,oy,in);

  while (imin<=imax) {
    i=fifox[imin];
    j=fifoy[imin];
    k=i+j*img->w();
    imin++;
    if ((i<img->w()-1)&&((*img).at(i+1,j)!=in)&&((*img).at(i+1,j)!=out)) {
      fifox[++imax]=i+1;
      fifoy[imax]=j;
      img -> putpoint (i+1,j,in);
    }
    if ((i>0)&&((*img).at(i-1,j)!=in)&&((*img).at(i-1,j)!=out)) {
      fifox[++imax]=i-1;
      fifoy[imax]=j;
      img -> putpoint (i-1,j,in);
    }
    if ((j<img->h()-1)&&((*img).at(i,j+1)!=in)&&((*img).at(i,j+1)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j+1;
      img -> putpoint (i,j+1,in);
    }
    if ((j>0)&&((*img).at(i,j-1)!=in)&&((*img).at(i,j-1)!=out)) {
      fifox[++imax]=i;
      fifoy[imax]=j-1;
      img -> putpoint (i,j-1,in);
    }
  }
}

int main(int argc, char ** argv)
{
  Image *img;
  int n,i;
  double p;

  /* arguments -> p et n */

  CL_Parser CLP (argc,argv,"n=500,p=.5");
  n = CLP('n');
  p = CLP('p');

  img=new Image(2*n,2*n, str(fmt("Site-percolation cluster (p=%)")%p));

  /* Site-percolation --- 1=dedans, 2=dehors */

  for (i=0;i<4*n*n;i++) {
    if ( prng.bernoulli(p) )
      img -> putpoint (i,0,1);
    else
      img -> putpoint (i,0,2);
  }

  /* 3=cluster de l'origine */

  fifox = new int [4*img->w()*img->h()];
  fifoy = new int [4*img->w()*img->h()];
  cnx(img,n,n,3,2);

  /* "Hull" du cluster */

  for (i=0;i<2*n;i++) if ((*img).at(i,0)!=Color(3))                            cnx(img,i,0,0,3);
  for (i=0;i<2*n;i++) if ((*img).at(0,i)!=Color(3))                 cnx(img,0,i,0,3);
  for (i=0;i<2*n;i++) if ((*img).at(img->w()*(i+1)-1,0)!=Color(3))           cnx(img,img->w()-1,i,0,3);
  for (i=0;i<2*n;i++) if ((*img).at(img->w()*(img->h()-1)+i,0)!=Color(3)) cnx(img,i,img->h()-1,0,3);

  for (i=0;i<4*n*n;i++) if ((*img).at(i,0)==Color(2)) img->putpoint(i,0,1);

  /* affichage du resultat */

  return 0;
}
