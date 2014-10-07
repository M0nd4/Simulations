#pragma once /// @file
#include <vector>
#include <iostream>

namespace vb {
	using Cycles = std::vector<std::vector<unsigned>>;

	class Passport : public std::vector<std::pair<unsigned,unsigned>> {};

	class Permutation : public std::vector<unsigned> {
	public:
		Permutation (unsigned n = 0)         	: std::vector<unsigned> (n)	{ for (unsigned i=0; i<n; ++i) at(i)=i; }
		Permutation (std::vector<unsigned> s)	: std::vector<unsigned> (s)	{ }
		Permutation (Cycles & c);

		bool is_identity () const;

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $this * o = o \circ this$.
		Permutation conjugate	(const Permutation & s) const;

		Cycles               	cycles ()   	const;
		std::vector<unsigned>	signature ()	const;
		Passport             	passport () 	const;

		std::vector<unsigned>	l;
	};

	bool connected (const Permutation & s, const Permutation & a);

	std::ostream & operator<< (std::ostream &os, const Passport &P);
	std::ostream & operator<< (std::ostream &os, const Permutation &P);
}
