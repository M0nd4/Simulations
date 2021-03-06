// S-E+F = 2, 3F=2E, S=2+F/2, F=2d, S=2+d
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/Pairings.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

Stream <Hypermap> triangulations (unsigned n) {
	Cycles phic; for (unsigned i=0; i<n/3; ++i) phic.push_back ({i,i+n/3,i+2*n/3}); Permutation phi (phic);
	int np = n/6; vector<unsigned> a (n/2 - np,2);

	return Stream<Hypermap> ([a,phi,n,np](Sink<Hypermap> & yield) {
		std::vector<Hypermap> hs;
		for (auto alph : permutations(a)) {
			Permutation alpha (n); for (unsigned i=0; i<n-2*np; ++i) alpha[i] = alph[i];
			for (int i=0; i<np; ++i) { alpha[n-2*np+2*i] = n-2*np+2*i+1; alpha[n-2*np+2*i+1] = n-2*np+2*i; }
			if (!connected(phi,alpha)) continue;
			Permutation sigma = (alpha*phi).inverse();
			Hypermap h(sigma,alpha,phi);
			h.normalize(); bool done=0;
			for (auto & hh : hs) if (h==hh) { done=1; break; }
			if (!done) { hs.push_back(h); yield(h); }
		}
	});
}

int main (int argc, char ** argv) {
	H.init ("Spheroidal enumeration", argc, argv, "s=3,m=228,d=2,g=0,v,o,b");
	unsigned s=H['s'], g=0, a=6*(s-2), d=H['d'];
	assert (a>0);

	int nb=0;
	for (Hypermap & M : triangulations(a)) {
		if (M.genus() != g)                                                          	continue;
		bool good=true; for (auto & c : M.sigma.cycles()) if (c.size()<d) good=false;	if (!good)	continue;

		cout << ++nb << ": " << M << endl;

		if (H['v'] || H['o'] || H['b']) {
			ostringstream os; os << "Spheroidal enumeration (s=" << s << ", d=" << d << ", i=" << nb << ")"; H.title = os.str();
			Constellation0<double> C {M,H}; C.belyi();
			if (H['b']) cout << endl << C << endl;
			if (H['v'] || H['o']) {
				auto bd = C.bounds();
				Coloring CC (bd.first,bd.second,800,[&](cpx z){ return HSV ((imag(C(z))>0)?0:.5, .8, .8); });
				CC.scale(1.5); CC.show();
				if (H['o']) CC.output(); else { while (CC.visible()) { CC.update(); Fl::wait(); } };
				CC.hide();
			}
		}
	}
}
