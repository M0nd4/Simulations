#pragma once
#include <vb/Minimizer.h>

namespace vb {
	template <typename T> Minimizer<T>::Minimizer (unsigned int n_) :
		n(n_), x(n), gx(n), old_x(n), old_gx(n), er(1.0), ler(0) {}

	template <typename T> Minimizer<T>::Minimizer (unsigned int n_,
		std::function < T (const Vector<T>&,Vector<T>&) > fg_) :
		Minimizer (n_) { fg=fg_; }

	template <typename T> Minimizer<T>::Minimizer (unsigned int n_,
		std::function < T (const Vector<T>&) > f_, std::function < Vector<T> (const Vector<T>&) > g_) :
		Minimizer (n_) { f=f_; g=g_; }

	template <typename T> T Minimizer<T>::compute (const Vector<T> &x_) {
		if ((!x_.empty()) && (&x != &x_)) x=x_;
		if (fg)	{ fx = fg(x,gx); } else	{ fx = f(x); gx = g(x); }
		return fx;
	}

	template <typename T> void Minimizer<T>::line_search (const Vector<T> &d) {
		old_x.swap(x); old_fx=fx; old_gx.swap(gx);

		T qq_0 = T(.8) * inner_prod (old_gx,d);
		T dir = (qq_0>T(0) ? T(-1) : T(1));
		T t_l = T(0), t_r = T(0), t = dir;
		T y;

		bool refining = false;

		while (true) {
			x = old_x + t*d; compute();
			y = old_fx + T(.3) * t * qq_0;

			if ((fx<=y) && (dir*inner_prod (gx,d) >= dir*qq_0)) break;
			if (fx>y) { t_r=t; refining = true; } else t_l = t;
			if (refining) t = (t_r+t_l)/T(2); else t *= T(2);
			if (t-t_l+T(1) == T(1)) break;
		}
	}

	template <typename T> T Minimizer<T>::minimize_grad (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		while (fx < old_fx) line_search (Vector<T>(gx));
		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		Vector<T> dx,dg,Wdg;
		T dgdx,u;

		Vector<T> diag = W0;
		if (diag.size() == 0) diag = Vector<T> (x0.size(), T(1.0));
		Matrix<T> W(x0.size(),x0.size());
		for (unsigned int i=0; i<x0.size(); ++i) W(i,i) = diag[i];

		while (fx < old_fx) {
			line_search(prod(W,gx));

			dx = x - old_x;
			dg = gx - old_gx;
			Wdg = prod(W,dg);
			dgdx = inner_prod(dg,dx);
			dx /= dgdx;
			u = dgdx + inner_prod(dg,Wdg);

			W += outer_prod (u*dx-Wdg,dx) - outer_prod (dx,Wdg);
		}

		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_fr (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		Vector<T> d(n);
		Vector<T> old_d(n);
		bool first = true;

		while (fx < old_fx) {
			old_d = d; d = -gx;
			if (!first) {
				T c = inner_prod(gx,gx) / inner_prod(old_gx,old_gx);
				d += c * old_d;
			}
			line_search(d);
			first=false;
		}

		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_pr (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		Vector<T> d(n);
		Vector<T> old_d(n);
		Vector<T> y(n);

		bool first = true;

		while (fx < old_fx) {
			old_d.swap(d);
			d = -gx;

			if (!first) {
				T c1 = inner_prod(old_gx,old_gx);

				old_gx -= gx;

				old_d *= inner_prod(old_gx,gx) / c1;
				d -= old_d;
			}

			line_search(d);
			first=false;
		}

		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_qn (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+T(1);
		old_gx = gx;

		Vector<T> d(n);
		Vector<T> old_d(n);
		Vector<T> y(n);

		bool first = true;

		while (fx < old_fx) {
			old_d.swap(d);
			d = -gx;

			if (!first) {
				y = gx - old_gx;
				d += old_d * inner_prod(y,gx) / inner_prod(y,old_d);
			}

			line_search(d);
			first=false;

			if (cb) cb(x,fx);
		}

		return fx;
	}
}
