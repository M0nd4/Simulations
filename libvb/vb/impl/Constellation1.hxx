#pragma once
#include <vb/Constellation1.h>
#include <vb/NumberTheory.h>
#include <vb/Toroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation1<T>::Constellation1 (const Hypermap & M, const Hub & H) {
		Hypermap M2 (M); M2.dessin(); p = { I_<T>(), T(0) };
		do {
			M2.split_edges(); Toroidal S (M2,H); S.pack(); S.output_pdf();
			unsigned N = M.sigma.size();
			b.clear(); for (auto c : M.sigma.cycles())	b.push_back( { to_cpx<T>(S.V[S.E[c[0]].src].z),    	c.size() } );
			w.clear(); for (auto c : M.alpha.cycles())	w.push_back( { to_cpx<T>(S.V[S.E[c[0]+N].src].z),  	c.size() } );
			f.clear(); for (auto c : M.phi.cycles())  	f.push_back( { to_cpx<T>(S.V[S.E[c[0]+3*N].src].z),	c.size() } );
			dim = b.size() + w.size() + f.size() + p.size();
			p[0] = to_cpx<T>(S.m); shift(-b[0].z); normalize();
		} while (findn() > T(1e-6));
	}

	template <typename T> template <typename U> Constellation1<T>::Constellation1 (const Constellation1<U> & C) : Constellation<T>(C) {
		from_points();
	}

	template <typename T> void Constellation1<T>::from_points () {
		E = Elliptic<T> { q_<T>(p[0]) };
		cplx szp(0); for (auto zd : b) szp += T(zd.d)*zd.z; for (auto zd : f) szp -= T(zd.d)*zd.z;
		dy = round(T(imag(szp)/imag(p[0]))); dx = round(T(real(szp-T(dy)*p[0])));
		for (auto & zd : f) {
			int ddx = round(double(dx)/zd.d); if (ddx != 0) { zd.z += T(ddx); dx -= int(zd.d)*ddx; }
			int ddy = round(double(dy)/zd.d); if (ddy != 0) { zd.z += T(ddy)*p[0]; dy -= int(zd.d)*ddy; }
		}
	}

	template <typename T> void Constellation1<T>::normalize () {
		cplx sf(0); int sdf(0); for (auto zd : f) { sdf += zd.d; sf += zd.z * T(zd.d); }
		for (auto & zd : b) zd.z -= sf/T(sdf);
		for (auto & zd : w) zd.z -= sf/T(sdf);
		for (auto & zd : f) zd.z -= sf/T(sdf);
		p[1] = cplx(0); cplx avg (0); unsigned d=0;
		for (auto zd : w) { d += zd.d; avg += logder(zd.z,0) * T(zd.d); }
		p[1] = - avg/T(d);
	}

	template <typename T> void Constellation1<T>::shift (cplx z) {
		for (auto & zd : b) zd.z += z;
		for (auto & zd : w) zd.z += z;
		for (auto & zd : f) zd.z += z;
		from_points();
	}

	template <typename T> auto Constellation1<T>::logderp (cplx z, int k) const -> cplx {
		if (k==0) return log(E.sigma(z));
		if (k==1) return E.zeta(z);
		if (k==2) return E.wp(z);
		if (k==3) return E.wp_z(z);
		assert (!"Derivatives of higher order not implemented!"); return cplx(std::nan("Not implemented"));
	}

	template <typename T> auto Constellation1<T>::logderp_z (cplx z, int k) const -> cplx {
		if (k==0) return E.zeta(z);
		if (k==1) return - E.wp(z);
		if (k==2) return E.wp_z(z);
		assert (!"Derivatives of higher order not implemented!"); return cplx(std::nan("Not implemented"));
	}

	template <typename T> auto Constellation1<T>::logderp_q (cplx z, int k) const -> cplx {
		if (k==0) return E.sigma_q(z) / E.sigma(z);
		if (k==1) return E.zeta_q(z);
		if (k==2) return E.wp_q(z);
		assert (!"Derivatives of higher order not implemented!"); return cplx(std::nan("Not implemented"));
	}

	template <typename T> auto Constellation1<T>::logderp_t (cplx z, int k) const -> cplx {
		return to_cpx<T>(0,pi_<T>()) * E.q * logderp_q(z,k);
	}

	template <typename T> auto Constellation1<T>::logder (cplx z, int k) const -> cplx {
		cplx out (logderp (z-b[0].z+T(dx)+tau()*T(dy), k) - logderp (z-b[0].z, k));
		for (auto zd : b) out += logderp (z-zd.z, k) * T(zd.d);
		for (auto zd : f) out -= logderp (z-zd.z, k) * T(zd.d);
		if (k==0) { out += p[1]; out -= to_cpx<T>(0,T(2)*pi_<T>()) * round(real(out/to_cpx<T>(0,T(2)*pi_<T>()))); }
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_z (cplx z, int k) const -> cplx {
		cplx out (logderp_z (z-b[0].z+T(dx)+tau()*T(dy), k) - logderp_z (z-b[0].z, k));
		for (auto zd : b) out += logderp_z (z-zd.z, k) * T(zd.d);
		for (auto zd : f) out -= logderp_z (z-zd.z, k) * T(zd.d);
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_t (cplx z, int k) const -> cplx {
		cplx out (logderp_t (z-b[0].z+T(dx)+tau()*T(dy), k) + T(dy) * logderp_z (z-b[0].z+T(dx)+tau()*T(dy), k) - logderp_t (z-b[0].z, k));
		for (auto zd : b) out += logderp_t (z-zd.z, k) * T(zd.d);
		for (auto zd : f) out -= logderp_t (z-zd.z, k) * T(zd.d);
		return out;
	}

	template <typename T> auto Constellation1<T>::operator() (cplx z) const -> cplx {
		return exp(logder(z,0));
	}

	template <typename T> auto Constellation1<T>::reduce (cplx z) const -> cplx {
		while (imag(z) < - imag(tau())/T(2))                      	z += tau();
		while (imag(z) > imag(tau())/T(2))                        	z -= tau();
		while (real(z) < real(tau())*imag(z)/imag(tau()) - T(1)/2)	z += T(1);
		while (real(z) > real(tau())*imag(z)/imag(tau()) + T(1)/2)	z -= T(1);
		return z;
	}

	template <typename T> void Constellation1<T>::readvec (const Vector<cplx> & xy) {
		unsigned i=0;
		for (auto & zd : b) zd.z = xy[i++];
		for (auto & zd : w) zd.z = xy[i++];
		for (auto & zd : f) zd.z = xy[i++];
		for (auto & z  : p) z    = xy[i++];
		from_points();
	}

	template <typename T> auto Constellation1<T>::vec () const -> Vector<cplx> {
		Vector<cplx> bw (dim); unsigned i=0;
		for (const auto & zd : b) { bw[i++] = zd.z; }
		for (const auto & zd : w) { bw[i++] = zd.z; }
		for (const auto & zd : f) { bw[i++] = zd.z; }
		for (const auto & z  : p) { bw[i++] = z; }
		return bw;
	}

	template <typename T> auto Constellation1<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (dim); int k=0;
		for (auto zd : w) for (unsigned j=0; j<zd.d; ++j) out[k++] = logder(zd.z,j);
		cplx sz (T(-dx)+cplx(T(-dy)*tau()));
		for (auto zd : b) sz += T(zd.d) * zd.z;
		for (auto zd : f) sz -= T(zd.d) * zd.z;
		out[k++] = sz; out[k++] = b[0].z;
		return out;
	}

	template <typename T> auto Constellation1<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(dim,dim,cplx(0));
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<w[ii].d; ++id) { j=0; // f_i is logder(w[ii],id)
			out(i,j) = - logderp_z (w[ii].z-b[0].z+T(dx)+tau()*T(dy), id);
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) -= T(b[jj].d - (jj==0?1:0)) * logderp_z (w[ii].z-b[jj].z,id);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = (ii==jj) ? logder_z (w[ii].z,id) : cplx(0);
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = T(f[jj].d) * logderp_z (w[ii].z-f[jj].z, id);
			out(i,j++) = logder_t (w[ii].z,id);
			out(i,j++) = cplx(id==0 ? 1 : 0);
			assert (j==dim);
		++i; } { j=0; // f_i is sum(z*dz) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = T(b[jj].d);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = -T(f[jj].d);
			out(i,j++) = - T(dy);
			out(i,j++) = cplx(0);
			assert (j==dim);
		++i; } { j=0; // f_i is -sum(f*df) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = jj==0 ? T(1) : T(0);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = 0;
			out(i,j++) = cplx(0);
			out(i,j++) = cplx(0);
			assert (j==dim);
		++i; }
		assert (i==dim);
		return out;
	}

	template <typename T> auto Constellation1<T>::jacnum  () -> Matrix<cplx> {
		Vector<cplx> x = vec(), c = vcost(); Matrix<cplx> out (dim,dim);
		T eps (.00001);
		for (unsigned j=0; j<x.size(); ++j) {
			x[j] += eps; readvec(x); Vector<cplx> dc = vcost() - c; x[j] -= eps;
			for (unsigned i=0; i<c.size(); ++i) out(i,j) = dc[i] / eps;
		}
		return out;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-10); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "Modulus tau  = " << C.tau() << std::endl;
		os << "Invariant j  = " << C.E.j() << std::endl;
		os << "Invariant g2 = " << C.E.g2() << std::endl;
		os << "Invariant g3 = " << C.E.g3() << std::endl;
		os << "log(lambda)  = " << C.p[1] << std::endl;
		os << "Corr. shift  = " << C.dx << " " << C.dy << std::endl;
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.b[i].d << "\t" << C.b[i].z << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.w[i].d << "\t" << C.w[i].z << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.f[i].d << "\t" << C.f[i].z << std::endl;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Constellation1<real_t> & C) {
		using T = real_t;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,to_int(lerr)/2-12); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		int eps = nd-5;

		os << "log(lambda)  = " << C.p[1] << std::endl;
		os << "tau          = " << C.p[0] << std::endl;
		{ auto P = guess_r(C.p[0],eps); if (P) os << "\t\troot of " << *P << std::endl; }
		os << "invariant j  = " << C.E.j() << std::endl;
		{ auto P = guess_r(C.E.j(),eps); if (P) os << "\t\troot of " << *P << std::endl; }
		os << "invariant g2 = " << C.E.g2() << std::endl;
		{ auto P = guess_r(C.E.g2(),eps); if (P) os << "\t\troot of " << *P << std::endl; }
		os << "invariant g3 = " << C.E.g3() << std::endl;
		{ auto P = guess_r(C.E.g3(),eps); if (P) os << "\t\troot of " << *P << std::endl; }
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (auto & zd : C.b) {
			os << "| " << zd.d << "\t" << zd.z << std::endl;
			auto P = guess_r (zd.z,eps);
			if (P) os << "|\t\troot of " << *P << std::endl;
		}
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (auto & zd : C.w) {
			os << "| " << zd.d << "\t" << zd.z << std::endl;
			auto P = guess_r (zd.z,eps);
			if (P) os << "|\t\troot of " << *P << std::endl;
		}
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (auto & zd : C.f) {
			os << "| " << zd.d << "\t" << zd.z << std::endl;
			auto P = guess_r (zd.z,eps);
			if (P) os << "|\t\troot of " << *P << std::endl;
		}
		return os;
	}

	template <typename T> auto Constellation1<T>::bounds () const -> std::pair<cplx,cplx> {
		T xmin = std::min(T(0),real(tau())), xmax = std::max(T(1),real(T(1)+tau())), ymin = T(0), ymax = imag(tau());
		return { to_cpx<T> (xmin,ymin), to_cpx<T> (xmax,ymax) };
	}
}
