#pragma once /// @file
#include <vb/Permutation.h>

// Implementation of hypermaps as triples of permutations, following A. Zvonkin, "Belyi Functions: Examples, Properties, and Applications".

namespace vb {
	class Hypermap {
	public:
		int n_black () { return sigma.c.size(); }
		int n_white () { return alpha.c.size(); }
		int n_faces () { return phi.c.size(); }
		int n_edges () { return sigma.size(); }

		bool validate ();
		bool is_graph () { for (auto v : alpha.c) if (v.size() != 2) return false; return true; }
		bool is_triangulation () {
			if (!(is_graph())) return false;
			for (auto f : phi.c) if (f.size() != 3) return false;
			return true;
		}

		int euler () { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () { return 1-euler()/2; }

		void output_dot      	(std::ostream & os);
		void output_graph_dot	(std::ostream & os);

		Hypermap split_edges (); ///< Turn white vertices into black and then complete into a new hypermap.

		void flip (int e, bool fast=false); // fast=true will invalidate the permutation cycles, need to be rebuilt afterwards.

		Permutation sigma, alpha, phi; // black, white, faces
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}