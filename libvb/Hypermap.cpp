#include <vb/Hypermap.h>

namespace vb {
	void Permutation::s_to_c () {
		std::vector<int> done (size(), 0);
		for (int i=0; i<size(); ++i) {
			if (done[i]) continue;
			std::vector<int> v (1,i); done[i]=1;
			for (int j=at(i); done[j]==0; j=at(j)) { v.push_back(j); done[j]=1; }
			c.push_back(v);
		}
	}

	void Permutation::c_to_s () {
		int sz=0; for (std::vector<int> v : c) sz += v.size();
		std::vector<int>::operator= (std::vector<int> (sz));
		for (std::vector<int> v : c) {
			for (int i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
	}

	void Hypermap::validate () {
		assert (sigma.size() == alpha.size());
		assert (sigma.size() == phi.size());
		for (int i=0; i<sigma.size(); ++i) assert (phi[alpha[sigma[i]]] == i);
	}

	void Hypermap::output_dot (std::ostream & os) {
		std::vector<int> black(n_edges()), white(n_edges());
		for (int i=0; i<n_black(); ++i) for (int j : sigma.c[i]) black[j]=i;
		for (int i=0; i<n_white(); ++i) for (int j : alpha.c[i]) white[j]=i;
		os << "graph {" << std::endl;
		for (int i=0; i<n_black(); ++i) os << "  b" << i << " [fillcolor=\"grey\"];" << std::endl;
		for (int i=0; i<n_edges(); ++i) os << "  b" << black[i] << " -- w" << white[i] << ";" << std::endl;
		os << "}" << std::endl;
	}

	void Hypermap::output_graph_dot (std::ostream & os) {
		assert (is_graph());
		std::vector<int> black(n_edges()), white(n_edges());
		for (int i=0; i<n_black(); ++i) for (int j : sigma.c[i]) black[j]=i;
		for (int i=0; i<n_white(); ++i) for (int j : alpha.c[i]) white[j]=i;
		os << "graph { graph [dimen=3]" << std::endl;
		for (int i=0; i<n_black(); ++i)
			os << "  " << i << " [shape=point];" << std::endl;
		for (int i=0; i<n_edges(); ++i)
			if (i<alpha[i])
				os << "  " << black[i] << " -- " << black[alpha[i]] << ";" << std::endl;
		os << "}" << std::endl;
	}

	Hypermap Hypermap::split_edges () {
		Hypermap H; H.sigma = sigma;
		int N = n_edges();

		for (int a=0; a<N; ++a) {
			int b=alpha[a], c=phi[a], e=phi[c], f=sigma[a], x=phi[b];
			if (a<b) H.sigma.c.push_back ({a+N,x+3*N,b+2*N,b+N,c+3*N,a+2*N});
			H.alpha.c.push_back ({a,a+N}); H.alpha.c.push_back ({a+2*N,a+3*N});
			H.phi.c.push_back ({a,a+2*N,f+N});
		}
		for (std::vector<int> F : phi.c) {
			std::vector<int> FF = F;
			for (int &i : FF) i += 3*N;
			H.phi.c.push_back (FF);
		}
		H.sigma.c_to_s(); H.alpha.c_to_s(); H.phi.c_to_s();
		return H;
	}

	std::ostream & operator<< (std::ostream &os, Permutation &P) {
		os << "(";
		for (std::vector<int> cc : P.c) { os << " ("; for (int i : cc) os << " " << i; os << " )"; }
		return os << " )" << std::endl;
	}

	std::ostream & operator<< (std::ostream &os, Hypermap &H) {
		os   	<< "Hypermap < "
		     	<< H.n_black() << " black, " << H.n_white() << " white, "
		     	<< H.n_edges() << " half-edges, " << H.n_faces() << " faces, genus " << H.genus() << " >" << std::endl;
		// os	<< "  | Black: " << H.sigma << "  | White: " << H.alpha << "  | Faces: " << H.phi;
		return os;
	}
}
