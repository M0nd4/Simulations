#include <iostream>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/graph_utility.hpp>
#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace boost; using namespace std; using namespace vb;

using Traits         	= adjacency_list_traits <listS,vecS,directedS>;
using edge_descriptor	= Traits::edge_descriptor;
using Graph          	= adjacency_list <listS,vecS,directedS,no_property,
                     				property < edge_capacity_t, long,
                     				property < edge_residual_capacity_t, long,
                     				property < edge_reverse_t, edge_descriptor > > > >;

void add_one (Graph &g, int i, int j) {
	edge_descriptor e1,e2;
	bool t;

	tie(e1,t) = add_edge(i,j,g);
	tie(e2,t) = add_edge(j,i,g);

	put (edge_reverse, g, e1, e2);
	put (edge_reverse, g, e2, e1);
}

int main(int argc, char **argv) {
	H.init ("Percolation arm exponents", argc,argv, "n=100,t=1,p=.5");
	int n = H['n'];
	int n_iter = H['t'];
	double p = H['p'];

	int stats[5];
	for (int i=0;i<5;++i) stats[i]=0;

	// Create the graph once and for all

	Graph g (n*n+1);

	for (int x=0; x<n; ++x) {
		for (int y=0; y<n; ++y) {
			int i=x+n*y;
			if (x<n-1) add_one (g,i,i+1);
			if (y<n-1) add_one (g,i,i+n);
		}
	}

	// Wired boundary conditions

	for (int i=0; i<n; ++i) {
		add_one(g,i,n*n);
		add_one(g,n*i,n*n);
		add_one(g,(n-1)+n*i,n*n);
		add_one(g,(n-1)*n+i,n*n);
	}

	// Prepare for quick edge access

	property_map<Graph,edge_capacity_t>::type cap = get (edge_capacity,g);
	Graph::edge_iterator e,e_final;

	for (tie(e,e_final)=edges(g); e!=e_final; ++e) {
		cap[*e]=0;
		cap[get(edge_reverse,g,*e)]=0;
	}

	edge_descriptor *all_edges = new edge_descriptor[2*(n+1)*n];
	edge_descriptor *rev_edges = new edge_descriptor[2*(n+1)*n];

	{
		int i=0;
		for (tie(e,e_final)=edges(g); e!=e_final; ++e) {
			if (cap[*e]==0) {
				all_edges[i] = *e;
				cap[all_edges[i]] = 1;

				rev_edges[i] = get (edge_reverse,g,*e);
				cap[rev_edges[i]] = 1;

				++i;
			}
		}
	}

	// Then simulate

	{ ProgressBar PB (n_iter);
		for (int iter=1;iter<=n_iter;++iter) {
			for (int i=0; i<2*n*(n+1); ++i) {
				int o = prng.bernoulli(p);
				cap[all_edges[i]] = o;
				cap[rev_edges[i]] = o;
			}

			long flow = edmonds_karp_max_flow (g,(n>>1)*(n+1),n*n);
			stats[flow]++;

			PB.set (iter);
		}
	}

	cout << n << " | " << n_iter;
	for (int i=1; i<=4; ++i) {
		int bla=0;
		for (int j=i; j<=4; ++j) bla += stats[j];
		cout << " | " << bla << " " << (double)bla / (double)n_iter;
	}
	cout << endl;
}
