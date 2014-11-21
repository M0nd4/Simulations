#pragma once
#include <vb/Constellation1.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Toroidal.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation1<T>::Constellation1 (Hypermap M, Hub H, int n) {
		Hypermap M2 (M); M2.dessin(); for (int i=0; i<n; ++i) M2.split_edges();
		Toroidal S (M2,H); S.pack(); std::cerr << std::endl; S.output_pdf();

		unsigned N = M.sigma.size(); d=0;
		for (auto c : M.sigma.cycles())	{ b.push_back(cplx(S.V[S.E[c[0]].src].z));    	bd.push_back(c.size()); d+=bd.back(); }
		for (auto c : M.alpha.cycles())	{ w.push_back(cplx(S.V[S.E[c[0]+N].src].z));  	wd.push_back(c.size()); }
		for (auto c : M.phi.cycles())  	{ f.push_back(cplx(S.V[S.E[c[0]+3*N].src].z));	fd.push_back(c.size()); }
		tau = S.m;
		from_points(); normalize();
	}

	template <typename T> Constellation1<T>::Constellation1 () {}

	template <typename T> void Constellation1<T>::from_points () {
		q = q_(tau); et1 = eta1_(q); th1p0 = theta1_z(cplx(0),q);
		mt3t4p2 = - pow(theta4_(std::complex<T>(0),q),2) * pow(theta3_(std::complex<T>(0),q),2) * pow(pi_<T>(),2);
		mt3t4p2_q = - T(2) * theta4_q(cplx(0),q) * theta4_(std::complex<T>(0),q) * pow(theta3_(std::complex<T>(0),q),2) * pow(pi_<T>(),2)
			- pow(theta4_(std::complex<T>(0),q),2) * T(2) * theta3_q(cplx(0),q) * theta3_(std::complex<T>(0),q) * pow(pi_<T>(),2);
		cplx sz(0); for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i])*b[i];
		cplx sp(0); for (unsigned i=0; i<f.size(); ++i) sp += T(fd[i])*f[i];
		dy = round(double(T(imag(sz-sp)/imag(tau)))); dx = round(double(T(real(sz-sp-T(dy)*tau))));
	}

	template <typename T> void Constellation1<T>::normalize () {
		cplx sf(0); int sdf(0); for (unsigned i=0; i<f.size(); ++i) { sdf+=fd[i]; sf += f[i] * T(fd[i]); }
		for (auto & z : b) z -= sf/T(sdf); for (auto & z : w) z -= sf/T(sdf); for (auto & z : f) z -= sf/T(sdf);
		ll = cplx(0); cplx avg (0); unsigned d=0;
		for (unsigned i=0; i<w.size(); ++i) { d += wd[i]; avg += logder(w[i],0) * T(wd[i]); }
		ll = - avg/T(d);
	}

	template <typename T> auto Constellation1<T>::my_lsigma  	(cplx z) const -> cplx { return log(sigma_(z,q)); }
	template <typename T> auto Constellation1<T>::my_lsigma_z	(cplx z) const -> cplx { return my_zeta(z); }
	template <typename T> auto Constellation1<T>::my_lsigma_t	(cplx z) const -> cplx { return q_t(tau) * sigma_q(z,q) / sigma_(z,q); }

	template <typename T> auto Constellation1<T>::my_zeta  	(cplx z) const -> cplx { return zeta_(z,q); }
	template <typename T> auto Constellation1<T>::my_zeta_z	(cplx z) const -> cplx { return - my_wp(z); }
	template <typename T> auto Constellation1<T>::my_zeta_t	(cplx z) const -> cplx { return q_t(tau) * zeta_q(z,q); }

	template <typename T> auto Constellation1<T>::my_wp  	(cplx z) const -> cplx { return wp_(z,q); }
	template <typename T> auto Constellation1<T>::my_wp_z	(cplx z) const -> cplx { return wp_z(z,q); }
	template <typename T> auto Constellation1<T>::my_wp_t	(cplx z) const -> cplx { return q_t(tau) * wp_q(z,q); }

	template <typename T> auto Constellation1<T>::logderp (cplx z, int k) const -> cplx {
		if (k==0) return my_lsigma (z);	// 0th : sum(log(sigma)) = log(prod(sigma))
		if (k==1) return my_zeta (z);  	// 1st : sum(sigma'/sigma) = sum(zeta)
		if (k==2) return my_wp (z);    	// 2nd : sum(zeta') = - sum(wp)
		if (k==3) return my_wp_z (z);  	// 3rd : - sum(wp')
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logderp_z (cplx z, int k) const -> cplx {
		if (k==0) return my_lsigma_z (z);	// 0th : sum(log(sigma)) = log(prod(sigma))
		if (k==1) return my_zeta_z (z);  	// 1st : sum(sigma'/sigma) = sum(zeta)
		if (k==2) return my_wp_z (z);    	// 2nd : sum(zeta') = - sum(wp)
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logderp_t (cplx z, int k) const -> cplx {
		if (k==0) return my_lsigma_t (z);	// 0th : sum(log(sigma)) = log(prod(sigma))
		if (k==1) return my_zeta_t (z);  	// 1st : sum(sigma'/sigma) = sum(zeta)
		if (k==2) return my_wp_t (z);    	// 2nd : sum(zeta') = - sum(wp)
		assert (!"Derivatives of higher order not implemented!");
	}

	template <typename T> auto Constellation1<T>::logder (cplx z, int k) const -> cplx {
		cplx out (logderp (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp (z-f[i], k) * T(fd[i]);
		if (k==0) { out += ll; out -= cplx(0,T(2)*pi_<T>()) * T(round(real(out/cplx(0,T(2)*pi_<T>())))); }
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_z (cplx z, int k) const -> cplx {
		cplx out (logderp_z (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp_z (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp_z (z-f[i], k) * T(fd[i]);
		return out;
	}

	template <typename T> auto Constellation1<T>::logder_t (cplx z, int k) const -> cplx {
		cplx out (logderp_t (z-b[0]+T(dx)+tau*T(dy), k) + T(dy) * logderp_z (z-b[0]+T(dx)+tau*T(dy), k));
		for (unsigned i=0; i<b.size(); ++i) out += logderp_t (z-b[i], k) * T(bd[i] - (i==0?1:0));
		for (unsigned i=0; i<f.size(); ++i) out -= logderp_t (z-f[i], k) * T(fd[i]);
		return out;
	}

	template <typename T> auto Constellation1<T>::operator() (cplx z) const -> cplx { return exp(logder(z,0)); }

	template <typename T> void Constellation1<T>::readvec (const Vector<cplx> & xy) {
		unsigned i=0;
		for (auto & z : b) z = xy[i++];
		for (auto & z : w) z = xy[i++];
		for (auto & z : f) z = xy[i++];
		tau = xy[i++]; ll = xy[i++];
		from_points();
	}

	template <typename T> auto Constellation1<T>::vec (const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t, const cplx & l) const -> Vector<cplx> {
		Vector<cplx> bw (b.size()+w.size()+f.size()+2); unsigned i=0;
		for (auto z : b) { bw[i++] = z; }
		for (auto z : w) { bw[i++] = z; }
		for (auto z : f) { bw[i++] = z; }
		bw[i++] = t; bw[i++] = l;
		return bw;
	}

	template <typename T> auto Constellation1<T>::vcost() const -> Vector<cplx> {
		Vector<cplx> out (d+2); int k=0;
		for (unsigned i=0; i<w.size(); ++i) for (unsigned j=0; j<wd[i]; ++j) out[k++] = logder(w[i],j);
		cplx sz (T(-dx)+cplx(T(-dy)*tau)); for (unsigned i=0; i<b.size(); ++i) sz += T(bd[i]) * b[i]; out[k++] = sz;
		cplx sf (0); for (unsigned i=0; i<f.size(); ++i) sf -= T(fd[i]) * f[i]; out[k++] = sf;
		return out;
	}

	template <typename T> T Constellation1<T>::cost () const	{ T out(0); for (auto z : vcost()) out += norm(z); return out; }

	template <typename T> auto Constellation1<T>::jacvcost () const -> Matrix<cplx> { // m_ij = \partial_j(f_i)
		Matrix<cplx> out(d+2,d+2,cplx(0));
		unsigned i=0,j=0; for (unsigned ii=0; ii<w.size(); ++ii) for (unsigned id=0; id<wd[ii]; ++id) { j=0; // f_i is logder(w[ii],id)
			out(i,j) = - logderp_z (w[ii]-b[0]+T(dx)+tau*T(dy), id);
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) -= T(bd[jj] - (jj==0?1:0)) * logderp_z (w[ii]-b[jj],id);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = (ii==jj) ? logder_z (w[ii],id) : cplx(0);
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = T(fd[jj]) * logderp_z (w[ii]-f[jj], id);
			out(i,j++) = logder_t (w[ii],id);
			out(i,j++) = cplx(id==0 ? 1 : 0);
			assert (j==unsigned(d+2));
		++i; } { j=0; // f_i is sum(z*dz) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = T(bd[jj]);
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = 0;
			out(i,j++) = - T(dy);
			out(i,j++) = cplx(0);
			assert (j==unsigned(d+2));
		++i; } { j=0; // f_i is -sum(f*df) recentered
			for (unsigned jj=0; jj<b.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<w.size(); ++jj)	out(i,j++) = 0;
			for (unsigned jj=0; jj<f.size(); ++jj)	out(i,j++) = -T(fd[jj]);
			out(i,j++) = cplx(0);
			out(i,j++) = cplx(0);
			assert (j==unsigned(d+2));
		++i; }
		assert (i==unsigned(d+2));
		return out;
	}

	template <typename T> auto Constellation1<T>::jacnum  () -> Matrix<cplx> {
		Vector<cplx> x = vec (b,w,f,tau,ll), c = vcost(); Matrix<cplx> out (d+2,d+2);
		T eps (.00001);
		for (unsigned j=0; j<x.size(); ++j) {
			x[j] += eps; readvec(x); Vector<cplx> dc = vcost() - c; x[j] -= eps;
			for (unsigned i=0; i<c.size(); ++i) out(i,j) = dc[i] / eps;
		}
		return out;
	}

	template <typename T> void Constellation1<T>::find () {
		Vector<cplx> bw = vec(b,w,f,tau,ll);
		T c = cost(), eps = std::min(T(.1),c), nc = c;

		while (eps>1e-100) {
			std::cerr << "\r" << c << " (" << eps << ") [" << tau << "]          ";
			bool flag = false;
			for (auto & z : bw) {	z += eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= eps; }
			                     	z -= eps; readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += eps; }
			                     	z += cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z -= cplx(0,eps); }
			                     	z -= cplx(0,eps); readvec(bw); nc = cost(); if (nc<c) { c=nc; flag=true; } else { z += cplx(0,eps); } }
			if (!flag) eps /= 1.618; else eps *= 1.1;
		}
		std::cerr << std::endl;
	}

	template <typename T> T Constellation1_fg (const Vector<T> & xy, Vector<T> & df, void * c) {
		Constellation1<T> * C = (Constellation1<T> *) c; return C->fg(xy,df);
	}

	template <typename T> void Constellation1_cb (const Vector<T> &, T f, void *) {
		static T er (-1); if ((f<er)||(er<T(0))) { std::cerr << f << "          \r"; er = f; }
	}

	template <typename T> void Constellation1<T>::findn () {
		Vector<cplx> x = vec(b,w,f,tau,ll); Matrix<cplx> IJ (x.size(),x.size());
		T c = cost(), old_c = c + T(1);
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; auto old_x = x;
			inv(jacvcost(),IJ); x -= prod(IJ,vcost());
			readvec(x); c = cost();
			if (c > old_c) readvec(old_x);
		}
		std::cerr << std::endl;
	}

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C) {
		Constellation1<U> CC;
		CC.bd = C.bd; CC.fd = C.fd; CC.wd = C.wd; CC.ll = C.ll; CC.tau = C.tau; CC.d = C.d;
		for (auto z : C.b) CC.b.push_back(z); for (auto z : C.f) CC.f.push_back(z); for (auto z : C.w) CC.w.push_back(z);
		CC.from_points();
		return CC;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C) {
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-7); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "tau = " << C.tau << std::endl;
		os << "log(lambda) = " << C.ll << std::endl;
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Constellation1<gmp100> & C) {
		using T = gmp100;
		T err (C.cost()); T lerr (-log10(err)); int nd = std::max (5,int(lerr)/2-12); if (err==T(0)) nd=10;
		os << std::setprecision(nd) << std::fixed;
		os << "tau = " << C.tau << std::endl;
		os << "log(lambda) = " << C.ll << std::endl;
		os << std::endl;
		os << "Keeping " << nd << " digits." << std::endl;
		os << std::endl;
		os << "Black vertices / zeros: " << std::endl;
		for (unsigned i=0; i<C.b.size(); ++i) {
			os << "| " << C.bd[i] << "\t" << C.b[i] << std::endl;
			Polynomial<cpxint> P = guess (C.b[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "White vertices / ones: " << std::endl;
		for (unsigned i=0; i<C.w.size(); ++i) {
			os << "| " << C.wd[i] << "\t" << C.w[i] << std::endl;
			Polynomial<cpxint> P = guess (C.w[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		os << std::endl;
		os << "Red vertices / poles: " << std::endl;
		for (unsigned i=0; i<C.f.size(); ++i) {
			os << "| " << C.fd[i] << "\t" << C.f[i] << std::endl;
			Polynomial<cpxint> P = guess (C.f[i],T(pow(T(.1),nd)));
			if (P.degree()>0) os << "|\t\troot of " << P << std::endl;
		}
		return os;
	}

	template <typename T> Image * Constellation1<T>::draw (unsigned l) const {
		T xmin(0), xmax(0), ymin(0), ymax(0);
		for (auto z : b) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : f) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		for (auto z : w) { xmin=std::min(xmin,real(z)); xmax=std::max(xmax,real(z)); ymin=std::min(ymin,imag(z)); ymax=std::max(ymax,imag(z)); }
		cplx center { (xmin+xmax)/T(2), (ymin+ymax)/T(2) }; T scale = T(.75) * std::max(xmax-xmin,ymax-ymin);

		Image * img = new Image (l,l,"Constellation1"); img->show();
		for (unsigned j=0; j<l; ++j) for (unsigned i=0; i<l; ++i) {
			cplx z {T(i),T(j)}; z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z;
			img->put(coo(i,j), imag((*this)(z))>0 ? Color(200,255,255) : Color(200,200,255));
		}
		for (auto z : b) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), BLACK); }
		for (auto z : w) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), WHITE); }
		for (auto z : f) { z = (z-center)/scale; z = (z-cplx{-1,1})*T(l/2); double x=real(z), y=imag(z); img->put (coo(x,-y), RED); }
		return img;
	};
}
