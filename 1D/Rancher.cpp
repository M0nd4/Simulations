/*
  Simulation de la "marche aléatoire qui évite son enveloppe convexe passée"
  avec condition initiale donnée par un cône ; avec dessin des enveloppes successives

  Output vectoriel en PDF (ou PS)

  gcc -DMEXP=19937 /usr/include/SFMT/SFMT.c rancher.c -o rancher -lm

  USAGE : rancher pente nb inter filename [x]

  pente (float) tangente du demi-angle initial
  nb steps (int)
  inter (int) : inverse de la fréquence d'affichage des enveloppes
  filename : nom (sans extension !) du fichier de sortie
  x : si paramètre présent, output en .ps, sinon en .pdf
*/

#include <vb/PRNG.h>

using namespace std;
using namespace vb;

int n_sides;

class point {
public:
  double x;
  double y;
  char z; // 0 si point à l'infini, 1 si point du plan (cf. plan projectif)
};

// L'enveloppe convexe est décrite par une liste doublement chaînée
// (père & fils), qui la parcourt dans le sens positif (i.e. centre de
// courbure à gauche)

class maillon {
public:
  int n; // numéro du point
  maillon *prev;
  maillon *suiv;
};

// Angle (O,A), (O,B) entre -pi et pi. Suppose que O n'est pas à
// l'infini
double angle(struct point O, struct point A, struct point B) {
  double vxa,vya,vxb,vyb,u,sina,cosa;

  if (A.z) vxa=A.x-O.x, vya=A.y-O.y;
  else vxa=A.x, vya=A.y;
  u=sqrt(vxa*vxa+vya*vya);
  vxa/=u; vya/=u;

  if (B.z) vxb=B.x-O.x, vyb=B.y-O.y;
  else vxb=B.x, vyb=B.y;
  u=sqrt(vxb*vxb+vyb*vyb);
  vxb/=u; vyb/=u;

  sina=vxa*vyb-vxb*vya;
  cosa=vxa*vxb+vya*vyb;

  // Pour éviter les pb d'arrondi avant d'appliquer acos :
  if (cosa>1) cosa=1;
  if (cosa<-1) cosa=-1;

  return (sina>=0) ? acos(cosa) : -acos(cosa);
}

// Génère un pas aléatoire à partir de position (ni début ni fin) et le
// place dans newpoint (traj est le tableau des coordonnées des points)
void rand_point(struct maillon *position, vector<point> &traj, struct point *newpoint) {
  double x,y,xp,yp,u,xs,ys,vxp,vyp,vxs,vys,costheta,sintheta,alpha,sinalpha,cosalpha;
  char zp,zs;

  x=traj[(*position).n].x;
  y=traj[(*position).n].y;
  xp=traj[(*((*position).prev)).n].x;
  yp=traj[(*((*position).prev)).n].y;
  zp=traj[(*((*position).prev)).n].z;

  if (zp) // si prev n'est pas à l'infini
    vxp=xp-x, vyp=yp-y;
  else
    vxp=xp, vyp=yp;
  u=sqrt(vxp*vxp+vyp*vyp);
  vxp/=u;
  vyp/=u;

  alpha=prng.uniform_real()*(2*M_PI+angle(traj[(*position).n],traj[(*((*position).prev)).n],traj[(*((*position).suiv)).n]));

  sinalpha=sin(alpha);
  cosalpha=cos(alpha);
  (*newpoint).x=x+vxp*cosalpha-vyp*sinalpha;
  (*newpoint).y=y+vxp*sinalpha+vyp*cosalpha;
  (*newpoint).z=1;
}

// Libère (en mémoire) les maillons strictement compris entre début et
// fin
void libere_maillons(struct maillon *debut, struct maillon *fin) {
  struct maillon *next, *apres;

  next=(*debut).suiv;
  while(next!=fin) {
    apres=(*next).suiv;
    free(next);
    n_sides--;
    next=apres;
  }
}

// Ajoute le point position à l'enveloppe convexe débutant par debut
// (traj est le tableau des coordonnées des points) (le paramètre fin
// est en fait inutile)
void insere_maillon(struct maillon *position, struct maillon *debut, struct maillon *fin, vector<point> & traj) {
  struct maillon *next, *maillonmax, *maillonmin;
  float pente,minpente,maxpente;

  next=debut;

  minpente=+INFINITY; // M_PI devrait suffire
  maxpente=-INFINITY; // -M_PI devrait suffire

  // le min par défaut doit être fin et le max debut (si confondus, par exemple...)

  // nb : optimisation possible vu les monotonies dues à la convexité

  while(next!=NULL) {
    pente=angle(traj[(*position).n],traj[(*debut).n],traj[(*next).n]);
    if(pente<=minpente) {
      minpente=pente;
      maillonmin=next;
    }
    next=(*next).suiv;
  }

  next=maillonmin;

  while(next!=NULL) {
    pente=angle(traj[(*position).n],traj[(*debut).n],traj[(*next).n]);
    if(pente>=maxpente) {
      maxpente=pente;
      maillonmax=next;
    }
    next=(*next).prev;
  }

  // Desallocation des chaînons intermédiaires
  libere_maillons(maillonmax,maillonmin);

  // Introduction de position dans la chaîne, entre maillonmin et maillonmax
  (*position).prev=maillonmax;
  (*position).suiv=maillonmin;
  (*maillonmax).suiv=position;
  (*maillonmin).prev=position;
}

void dessine_enveloppe(FILE *fichier, struct maillon *debut, vector<point> & traj) {
  struct maillon *next;

  next=debut;

  fprintf(fichier,"%f %f %f setrgbcolor\n",prng.uniform_real(),prng.uniform_real(),prng.uniform_real());

  fprintf(fichier,"newpath\n");
  if((traj[(*debut).n]).z) // point de départ dans le plan affine
    fprintf(fichier,"%f %f moveto\n",traj[(*debut).n].x,traj[(*debut).n].y);
  else
    fprintf(fichier,"%f %f moveto\n",(traj[(*((*debut).suiv)).n]).x+(traj[(*debut).n]).x,(traj[(*((*debut).suiv)).n]).y+(traj[(*debut).n]).y);

  while((next=(*next).suiv)!=NULL) {
    if((traj[(*next).n]).z) // point de départ dans le plan affine
      fprintf(fichier,"%f %f lineto\n",traj[(*next).n].x,traj[(*next).n].y);
    else
      fprintf(fichier,"%f %f lineto\n",(traj[(*((*next).prev)).n]).x+(traj[(*next).n]).x,(traj[(*((*next).prev)).n]).y+(traj[(*next).n]).y);
  }
  fprintf(fichier,"stroke\n");
}

int main(int argc, char *argv[]) {
  FILE *fichier, *fichier_final;

  time_t t0, t1;

  double minx=+INFINITY,maxx=-INFINITY,miny=+INFINITY,maxy=-INFINITY;
  double x,y;
  int i;

  n_sides = 0;

  struct maillon debut, fin; // début et fin de la liste chaînée de l'enveloppe
  struct maillon *position, *position2; // position en cours et suivante

  if (argc<5) {
    printf("Usage : rancher pente nb inter filename [x]\nProduit le fichier filename.pdf (ou .ps si paramètre 'x' présent)\n");
    exit(1);
  }

  char filename[255];
  sprintf(filename,"%s.ps",argv[4]);

  fichier=tmpfile();

  int nb=15000; // longueur de la trajectoire
  sscanf(argv[2],"%d",&nb);

  int inter=10; // inverse de la fréquence d'affichage des enveloppes
  sscanf(argv[3],"%d",&inter);

  vector<point> traj(nb);
  // struct point *traj; // la trajectoire de la marche
  // traj=(struct point *)malloc(nb*sizeof(struct point));

  // Description de la config initiale
  i=3; // nombre de points au début

  // Point de départ en (0,0)
  traj[2].x=0;
  traj[2].y=0;
  traj[2].z=1;
  position=(struct maillon *)malloc(sizeof(struct maillon)); // Point de départ
  n_sides++;
  (*position).n=2;
  (*position).prev=&debut;
  (*position).suiv=&fin;

  double longueur=(double)nb*2;

  double pente;
  sscanf(argv[1],"%lf",&pente);

  // début vertical
  traj[0].x=-longueur;
  traj[0].y=-pente*longueur;
  traj[0].z=0;
  debut.n=0;
  debut.suiv=position;
  debut.prev=NULL;

  // fin horizontale
  traj[1].x=-longueur;
  traj[1].y=pente*longueur;
  traj[1].z=0;
  fin.n=1;
  fin.prev=position;
  fin.suiv=NULL;

  int seed=1234567;

  time(&t0);

  for(;i<nb;i++) {
    // Tire le nouveau point, le place dans traj[i]
    rand_point(position,traj,&(traj[i]));

    if(traj[i].x>maxx) maxx=traj[i].x;
    if(traj[i].x<minx) minx=traj[i].x;
    if(traj[i].y>maxy) maxy=traj[i].y;
    if(traj[i].y<miny) miny=traj[i].y;

    // Il devient la nouvelle position
    position=(struct maillon *)malloc(sizeof(struct maillon));
    n_sides++;
    printf ("%d\n", n_sides);
    (*position).n=i;

    // Reste à définir prev et suiv en recalculant l'enveloppe convexe
    insere_maillon(position, &debut, &fin, traj);
    if(!(i%inter))
      dessine_enveloppe(fichier, &debut, traj);
    else
      prng.uniform_real(),prng.uniform_real(),prng.uniform_real();
  }
  fprintf(fichier, "stroke");

  // Maintenant que l'on connaît les limites du dessin, on peut écrire l'en-tête, dans fichier_final
  rewind(fichier);

  fichier_final=fopen(filename,"w");

  minx-=(maxx-minx)*0.05; // Pour montrer la situation initiale (cône)

  fprintf(fichier_final,"%%!\n%%%%DocumentMedia: A4 595 842 80 white ( )\n%%%%Orientation: Portrait\n");
  float scale,scalex,scaley,transx,transy;
  scalex=842./(maxx-minx);
  scaley=595./(maxy-miny);
  scale=(scalex<scaley)?scalex:scaley;
  transx=(595-(maxy-miny)*scale)/2+maxy*scale;
  transy=(842-(maxx-minx)*scale)/2-minx*scale;
  fprintf(fichier_final,"%f %f translate\n", transx, transy);
  fprintf(fichier_final,"%f %f scale\n", scale, scale);
  fprintf(fichier_final,"90 rotate\n");

  // Copie de fichier vers fichier_final
  char line[256];
  while(fgets(line, 256, fichier)!=NULL)
    fputs(line, fichier_final);

  fclose(fichier);
  fclose(fichier_final);

  if(argc==5) {
    char commande[256];
    sprintf(commande,"ps2pdf %s",filename);
    system(commande);

    sprintf(commande,"rm %s",filename);
    system(commande);
  }

  time(&t1);
  printf("Temps écoulé : %.0lf s.\n", difftime(t1,t0));

  exit(0);
}
