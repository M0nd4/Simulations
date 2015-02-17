#pragma once
#include <vb/Constellation.h>
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Image.h>

namespace vb {
	template <typename T> class Constellation0 : public Constellation<T> { public:
		using cplx = std::complex<T>;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f; using Constellation<T>::p;
		using Constellation<T>::findn; using Constellation<T>::cost;

		Constellation0 (const Hypermap & M, const Hub & H);
		Constellation0 ();

		cplx	operator()	(cplx z)	const;

		void	draw	(Image & img, bool smooth = true) const;

		void	find 	(T t = T(0));
		void	belyi	();	// does too many things at once

	private:
		cplx	logder   	(cplx z, int k=0)          	const;
		void	linear   	(cplx u, cplx v = cplx(0));	// move the points, recompute P and Q, don't touch l
		void	normalize	();                        	// choose l to make ones ones
		void	make_l_1 	();                        	// rescale to make l equal to 1
		void	make_c_0 	();                        	// shift everybody to make sum(black)=0
		void	make_p_1 	();                        	// try to have reasonable scaling

		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;

		Vector<cplx>	vec    	()                                                                                              	const;
		Vector<T>   	coovec 	(const std::vector<Star<T>> & b, const std::vector<Star<T>> & w, const std::vector<Star<T>> & f)	const;
		void        	readcoo	(const Vector<T> & xy);
		void        	readvec	(const Vector<cplx> & xy);
	};

	template <typename T, typename U> Constellation0<U> cconvert (Constellation0<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation0<T> & C);
}