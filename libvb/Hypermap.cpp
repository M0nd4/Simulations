#include <vb/Hypermap.h>
#include <cassert>
#include <sstream>

namespace vb {
	bool Hypermap::validate () const {
		if (sigma.size() != alpha.size()) return false;
		if (sigma.size() != phi.size()) return false;
		return (sigma*alpha*phi).is_identity();
	}

	void Hypermap::from_hypermap () {
		Cycles sc = sigma.cycles();
		int nb = sc.size(); V.resize(nb); E.resize(6*nb);
		for (unsigned i=0; i<E.size(); ++i) E[i].i=i;
		for (unsigned i=0; i<V.size(); ++i) V[i].i=i;
		for (auto & v : V) {
			v.z = NAN; v.bone=false; v.adj.clear();
			for (int e : sc[v.i]) { E[e].a = NAN; E[e].src = v.i; }
		}
		for (auto & v : V) for (int e : sc[v.i]) { v.adj.push_back(E[alpha[e]].src); }
	}

	int Hypermap::euler () const { return sigma.cycles().size() + alpha.cycles().size() - sigma.size() + phi.cycles().size(); }

	unsigned	Hypermap::genus () const { return 1-euler()/2; }

	bool Hypermap::is_graph () const {
		for (const auto & v : alpha.cycles()) if (v.size() != 2) return false;
		return true;
	}

	bool Hypermap::is_triangulation () const {
		if (!(is_graph())) return false;
		for (const auto & f : phi.cycles()) if (f.size() != 3) return false;
		return true;
	}

	bool Hypermap::is_simple (unsigned d) const {
		for (const auto & s : sigma.cycles())	if (s.size()<=d) return false;
		for (const auto & f : phi.cycles())  	if (f.size()<=d) return false;
		return true;
	}

	void Hypermap::split_edges () {
		unsigned N = sigma.size();
		Cycles sigma_c, alpha_c, phi_c;
		sigma_c = sigma.cycles();

		for (unsigned a=0; a<N; ++a) {
			unsigned b=alpha[a], c=phi[a], f=sigma[a], x=phi[b];
			if (a<b) sigma_c.emplace_back (Permutation({a+N,x+3*N,b+2*N,b+N,c+3*N,a+2*N}));
			alpha_c.emplace_back (Permutation({a,a+N}));
			alpha_c.emplace_back (Permutation({a+2*N,a+3*N}));
			phi_c.emplace_back (Permutation({a,a+2*N,f+N}));
		}
		for (const auto & F : phi.cycles()) {
			std::vector<unsigned> FF = F;
			for (unsigned &i : FF) i += 3*N;
			phi_c.push_back (FF);
		}

		sigma = sigma_c; alpha = alpha_c; phi = phi_c;
		initial.resize (sigma.size(),0);
		for (unsigned i=0; i<N; ++i) {
			initial[alpha[i]] = (initial[i]&1 ? 1 : 0);
			if ((initial[i]&2) && (initial[alpha[sigma[sigma[sigma[alpha[i]]]]]]&2)) initial[alpha[i]] |= 4;
		}
	}

	void Hypermap::flip (unsigned e) {
		unsigned b=sigma[e], a=alpha[b], c=sigma[a], d=alpha[c], f=alpha[e], g=phi[f], h=alpha[g], i=phi[g], j=alpha[i];
		if (alpha[phi[alpha[phi[e]]]]==e) return;
		if (phi[alpha[phi[alpha[e]]]]==e) return;
		if ((e==sigma[e])||(f==sigma[f])) return;
		sigma[a]=e; sigma[e]=c; sigma[d]=j; sigma[g]=b; sigma[i]=f; sigma[f]=h;
		phi[a]=g; phi[g]=f; phi[f]=a; phi[d]=e; phi[e]=i; phi[i]=d;
		alpha = (phi * sigma).inverse();
	}

	Permutation Hypermap::rebasing (unsigned i) const {
		unsigned n=alpha.size(), m=0; std::vector<unsigned> s1(n,n), s2(n,n);
		auto go = [&] (unsigned i) { while (s1[i]==n) { s1[i]=m; s2[m]=i; ++m; i=alpha[i]; } };
		go(i); for (int x : s2) go(phi[x]); return s1;
	}

	Permutation Hypermap::rebasing () const {
		Permutation s = rebasing(0), a = alpha.conjugate(s), p = phi.conjugate(s);
		for (unsigned i=1; i<alpha.size(); ++i) {
			Permutation s2 = rebasing(i), a2 = alpha.conjugate(s2), p2 = phi.conjugate(s2);
			if ((a2<a) || ((a2==a) && (p2<p))) { s=s2; a=a2; p=p2; }
		}
		return s;
	}

	void Hypermap::normalize () { relabel(rebasing()); }

	void Hypermap::mirror () { alpha = sigma*phi; sigma = sigma.inverse(); phi = phi.inverse(); }

	void Hypermap::dual () { sigma = sigma.inverse(); alpha = alpha.inverse(); phi = phi.inverse(); swap(sigma,phi); }

	void Hypermap::simplify1 () {
		int n = sigma.size();
		bool finished = false;
		while (!finished) {
			finished = true;
			for (auto v : sigma.cycles()) {
				if (v.size() != 1) continue;
				finished = false;

				int jj=v[0], ii=alpha[jj]; Permutation p(n);
				if (jj==n-2) { p[ii] = n-2; p[n-2] = n-1; p[n-1] = ii; } else { p[ii]=n-2; p[n-2]=ii; p[jj]=n-1; p[n-1]=jj; }
				relabel(p);

				unsigned i=phi[n-1], j=sigma[n-2], k=alpha[j];
				sigma[i]=j; phi[k]=i; initial[j] |= initial[n-1]; initial[i] |= initial[n-2];
				sigma.resize(n-2); alpha.resize(n-2); phi.resize(n-2); initial.resize(n-2); n-=2;
				assert(validate());

				break;
			}
		}
	}

	void Hypermap::simplify2 () {
		int n = sigma.size();
		bool finished = false;
		while (!finished) {
			finished = true;
			for (auto v : sigma.cycles()) {
				if (v.size() != 2) continue;
				finished = false;

				int i=std::min(v[0],v[1]), j=std::max(v[0],v[1]); Permutation p(n);
				if (j==n-2) { p[i] = n-2; p[n-2] = n-1; p[n-1] = i; } else { p[i]=n-2; p[n-2]=i; p[j]=n-1; p[n-1]=j; }
				relabel(p);

				int ii=alpha[n-2], jj=alpha[n-1]; alpha[ii]=jj; alpha[jj]=ii;
				initial[ii] |= initial[n-2]; initial[jj] |= initial[n-1];
				sigma.resize(n-2); alpha.resize(n-2); phi.resize(n-2); initial.resize(n-2); n-=2;
				phi = (sigma*alpha).inverse();

				break;
			}
		}
	}

	void Hypermap::simplify (int d) {
		while (!is_simple(d)) {
			if (d>=1) { simplify1(); dual(); simplify1(); dual(); }
			if (d>=2) { simplify2(); dual(); simplify2(); dual(); }
		}
	}

	void Hypermap::relabel (const Permutation & p) {
		sigma = sigma.conjugate(p); alpha = alpha.conjugate(p); phi = phi.conjugate(p);
		std::vector<unsigned> b (sigma.size()); for (unsigned i=0; i<sigma.size(); ++i) b[p[i]] = initial[i]; initial=b;
	}

	void Hypermap::dessin () {
		Cycles new_a, new_f; unsigned n=sigma.size(); initial.resize(6*n); Permutation alpha1 = alpha.inverse();
		for (unsigned i=0; i<n; ++i) {
			new_a.emplace_back(Permutation({i,i+n}));
			new_a.emplace_back(Permutation({i+2*n,i+3*n}));
			new_a.emplace_back(Permutation({i+4*n,i+5*n}));
			new_f.emplace_back(Permutation({i,i+2*n,i+4*n}));
			new_f.emplace_back(Permutation({alpha1[i]+n,phi[i]+5*n,i+3*n}));
			initial[i]=3; initial[i+n]=5; initial[i+2*n]=4; initial[i+3*n]=8; initial[i+4*n]=8; initial[i+5*n]=2;
		}
		alpha = new_a; phi = new_f; sigma = (alpha*phi).inverse();
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os	<< "Hypermap < " << H.sigma.cycles().size() << " black, "
		  	<< H.alpha.cycles().size() << " white, " << H.sigma.size() << " half-edges, "
		  	<< H.phi.cycles().size() << " faces, genus " << H.genus() << " >" << std::endl;
		os << "  sigma: " << H.sigma << std::endl;
		os << "  alpha: " << H.alpha << std::endl;
		os << "    phi: " << H.phi << std::endl;
		return os;
	}

	double Hypermap::alpha_xyz (double x, double y, double z) const { return acos ( (x*(x+y+z) - y*z) / ((x+y)*(x+z)) ); }

	double Hypermap::ccn (int n) const {
		static std::vector<double> p;
		for (int i=p.size(); i<=n; ++i) p.push_back (sqrt(2/(1-cos(2*M_PI/i)))-1);
		return p[n];
	}

	double acpa_step (const Hypermap & M, const std::vector<double> & in, std::vector<double> & out, std::vector<double> & er) {
		out = in; double se = 0;
		for (unsigned i=0; i<out.size(); ++i) {
			if (M.V[i].fixed) continue;
			auto & adj = M.V[i].adj; int n = adj.size(); if (n==2) continue;
			double s = M.alpha_xyz (out[i],out[adj[0]],out[adj[n-1]]);
			for (int j=0; j<n-1; ++j) s += M.alpha_xyz (out[i],out[adj[j]],out[adj[j+1]]);
			er[i] = s-2*M_PI; se += fabs(er[i]);
			double c = cos(s/n); double nr = M.ccn(n) * (1-c + sqrt(2*(1-c))) / (1+c); out[i] *= 1.1*nr - .1;
		}
		return se;
	}

	void Hypermap::acpa () {
		int n = V.size();
		std::vector<double> r (n,0), old_r (n,0), e (n,1), old_e (n,0), l (n,0), ll (n,0);

		int   	m  	= pow(n,.25);	// Steps between looks
		double	eps	= .1;        	// Dampening of the decay estimator
		double	mvm	= .01;       	// Threshold to trigger the jump
		double	tgt	= 1e-8;      	// Target per-vertex error

		for (int i=0; i<n; ++i) if (V[i].adj.size() > 2) { r[i] = V[i].r; }
		for (int t=1 ;; ++t) {
			old_e = e; old_r=r; double se = 0;
			for (int i=0; i<m; ++i) se = acpa_step(*this,r,r,e);
			if (se/n<tgt) break;
			double mv=0;
			for (int i=0; i<n; ++i) {
				if (old_e[i]==0) continue;
				double x = (old_e[i]-e[i])/old_e[i]; // if (x<0) x=0;
				l[i] 	= (1-eps) * l[i] 	+ eps * x;
				ll[i]	= (1-eps) * ll[i]	+ eps * x*x;
				if (l[i]==0) continue;
				double v = (ll[i]-l[i]*l[i])/(ll[i]);
				if (v>mv) mv=v;
			}
			std::cerr << t << " " << se/n << " " << mv << "            \r";
			if (mv<mvm) {
				for (unsigned i=0; i<V.size(); ++i) {
					if (old_e[i]!=0) {
						double nr = (r[i] - (1-l[i])*old_r[i])/l[i];
						if (nr>0) r[i] = nr;
					}
				}
			}
		}
		for (unsigned i=0; i<V.size(); ++i) V[i].r = r[i];
	}

	Stream <Hypermap> hypermaps (const std::vector<unsigned> & s, const std::vector<unsigned> & a, const std::vector<unsigned> & p) {
		Cycles cs; int i=0;
		for (int l : s) { std::vector<unsigned> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
		Permutation sigma (cs);
		return Stream<Hypermap> ([sigma,a,p](Sink<Hypermap> & yield) {
			std::vector<Hypermap> hs;
			for (const auto & alpha : permutations(a)) {
				if (!connected(sigma,alpha)) continue;
				Permutation phi = (sigma*alpha).inverse(); if (phi.signature() != p) continue;
				Hypermap h(sigma,alpha,phi);
				h.normalize(); bool done=false;
				for (auto & hh : hs) if (h==hh) { done=true; break; }
				if (!done) { hs.push_back(h); yield(h); }
			}
		});
	}
}

YAML::Node YAML::convert<vb::Hypermap>::encode (const vb::Hypermap & h) {
    Node node;
    node["sigma"] = h.sigma.cycles();
    node["alpha"] = h.alpha.cycles();
    node["phi"]   = h.phi.cycles();
    return node;
}

bool YAML::convert<vb::Hypermap>::decode (const Node & node, vb::Hypermap & h) {
	auto sigma = node["sigma"].as<vb::Cycles>();
	auto alpha = node["alpha"].as<vb::Cycles>();
	auto phi = node["phi"].as<vb::Cycles>();
	h = vb::Hypermap (sigma, alpha, phi);
    return true;
}
