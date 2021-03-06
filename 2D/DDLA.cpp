#include <vb/Point.h>

using namespace vb; using namespace std;

class DDLA : public CoarseImage {
public:
	DDLA (int n_) : CoarseImage (n_, n_, H['f'] ? pow(n_,.33) : 1), f(H['f']), n(n_), cursum(0), p(H['p']) {
		put(0,1);
		pq << Point(1,prng.exponential()/p) << Point(J,prng.exponential()/(1-p));
	};

	bool ok (coo z) {	if ( (z.x >= n) || (z.y >= n) || (z.x+z.y > cursum) ) return 1;
	                 	if (at(z)) return 0; else return ok (z+dz[prng.bernoulli(p)]); }

	void fill (coo z) {	if ( (z.x > 0) && (z.y < n-1) && at(z-1+J) && !at(z-1) ) { put(z-1,1); fill (z-1); }
	                   	if ( (z.y > 0) && (z.x < n-1) && at(z+1-J) && !at(z-J) ) { put(z-J,1); fill (z-J); } }

	void run () {
		while (true) {
			Point pt (pq);

			if ((pt.x == n-1) || (pt.y == n-1)) return;
			if (!at(pt)) {
				double curtime = pt.t;
				if (ok(pt)) {
					put(pt,1); cursum = max (cursum, pt.x+pt.y); if (f) fill (pt);
					if ( (pt.x<n-1) && (at(pt+1)==0) ) pq << Point(pt+1,curtime+prng.exponential()/p);
					if ( (pt.y<n-1) && (at(pt+J)==0) ) pq << Point(pt+J,curtime+prng.exponential()/(1-p));
				} else {
					pq << Point(pt,curtime+prng.exponential());
				}
			}
		}
	}

	bool f;
	long n,cursum;
	double p;
	PointQueue pq;
};

int main (int argc, char ** argv) {
	H.init ("Directed DLA",argc,argv,"n=750,p=.5,f");
	DDLA img (H['n']); img.show(); img.run(); img.output();
}
