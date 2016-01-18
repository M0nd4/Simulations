#include <vb/Hypermap_lib.h>
#include <fstream>

namespace vb {
	HLib::HLib () {
		json j;
		std::ifstream("hypermaps.json") >> j;
		for (auto e = j.begin(); e != j.end(); ++e) {
			emplace (e.key() + "_from_json", e.value());
		}

		// genus 0, hypermaps

		emplace ("star_1",	Hypermap { Cycles {{0}}, 	Cycles {{0}},         	Cycles {{0}} 	} );
		emplace ("star_2",	Hypermap { {{0,1}},      	{{0},{1}},            	{{0,1}}      	} );
		emplace ("star_3",	Hypermap { {{0,1,2}},    	{{0},{1},{2}},        	{{2,1,0}}    	} );
		emplace ("star_4",	Hypermap { {{0,1,2,3}},  	{{0},{1},{2},{3}},    	{{3,2,1,0}}  	} );
		emplace ("star_5",	Hypermap { {{0,1,2,3,4}},	{{0},{1},{2},{3},{4}},	{{4,3,2,1,0}}	} );

		emplace ("line_4",	Hypermap { {{0,1},{2}},	{{0},{1,2}},	{{0,1,2}}} );

		emplace ("tripod_l", 	Hypermap { {{2,1,0},{3}},        	{{2},{1},{0,3}},          	{{2,0,3,1}}      	} );	// LZ p.87 fig. 2.8
		emplace ("tripod_ll",	Hypermap { {{0,1,2},{3,4},{5,6}},	{{0},{1,3},{2,5},{4},{6}},	{{0,2,6,5,1,4,3}}	} );	// LZ p.88 fig. 2.9
		emplace ("tripod_L", 	Hypermap { {{4,1,2},{3,0},{5,6}},	{{4,3},{1},{2},{0,5},{6}},	{{4,0,6,5,3,2,1}}	} );	// LZ p.89 fig. 2.10

		emplace ("tree_h",	Hypermap { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} } );

		emplace ("E8",	Hypermap { {{0},{1,2,3},{4,5},{6}},	{{0,1},{3},{2,4},{5,6}}, {{0,3,2,5,6,4,1}} } );

		emplace ("Z_fig4",	Hypermap {	{{0,1,2}, {3,4}, {5}, {6,7,8}},
		                  	          	{{0,3}, {1,8,2}, {4,5,6}, {7}},
		                  	          	{{0,4,8}, {1}, {2,7,6,5,3}} } );

		emplace ("subd",	Hypermap {	{{0},{1,3,2}},
		                	          	{{0,3,1},{2}},
		                	          	{{0,2,3},{1}} } );

		emplace ("flower",	Hypermap {	{{0,1,2},{3,4},{5,6}},
		                  	          	{{0,6,4},{1,3},{2,5}},
		                  	          	{{0,3},{1,4,5},{2,6}} } );

		emplace ("legall",	Hypermap {	{{0,3,2,1},{4,5},{6,11,10,9,8,7},{12,13}},
		                  	          	{{0,4,6},{1,11,5},{2,13,10},{3,7,9,12},{8}},
		                  	          	{{0,7},{1,4},{2,11},{3,13},{5,6},{8,9},{10,12}} } );

		emplace ("legall2",	Hypermap {	{{0,1,2},{3,4,5},{6,7,8,9},{10,11}},
		                   	          	{{0,3,6},{1,9,11,7,5},{2,4},{8,10}},
		                   	          	{{0,9},{1,4},{2,3},{5,6},{7,10},{8,11}} } );

		emplace ("BBBK",	Hypermap {	{{1,7,11,2},{3,8},{4,5},{6,10},{9,12,0}},
		                	          	{{1,3,12,4},{5,9},{6,7},{10,0,11},{2,8}},
		                	          	{{1,5,0,6},{7,10},{2,3},{8,11,12},{4,9}} } );

		emplace ("autm22",	Hypermap {	{{0,1,2,3,4,5,6,7,8,9,10},{11,12,13,14,15,16,17,18,19,20,21}},
		                  	          	{{0,8,2,1},{3,7,16,20},{4,19,18,5},{6,17},{9,10},{11,21,15,12},{13,14}},
		                  	          	{{0},{1},{4},{9},{11},{13},{18},{21},{2,7},{3,19},{5,17},{6,16},{8,10},{12,14},{15,20}} } );

		// genus 0, maps

		emplace ("m_star_3",  	Hypermap { {{1},{3},{5},{0,4,2}},            	{{0,1},{2,3},{4,5}},            	{{0,1,2,3,4,5}}        	} );
		emplace ("m_tripod_l",	Hypermap { {{1},{5},{7},{3,4},{2,6,0}},      	{{2,1},{0,3},{4,5},{6,7}},      	{{2,1,0,4,5,3,6,7}}    	} );
		emplace ("m_tree_h",  	Hypermap { {{1},{5},{7},{9},{2,8,0},{4,3,6}},	{{2,1},{0,3},{4,5},{6,7},{8,9}},	{{2,1,0,4,5,6,7,3,8,9}}	} );

		emplace ("m_triangle",	Hypermap {  {{0,1},{2,3},{4,5}}, {{0,2},{3,4},{1,5}}, {{0,3,5},{1,4,2}} } );

		emplace ("m_simple",	Hypermap { {{0,2,3},{1}}, {{0,1},{2,3}}, {{0,1,3},{2}} } );	// LZ p. 107 fig. 2.20

		emplace ("m_Z_fig5",	Hypermap {	{{0,7,8,6},{1,2,5},{3,4},{9,10},{11,12,13}},
		                    	          	{{0,1},{2,3},{4,6},{5,13},{7,12},{8,9},{10,11}},
		                    	          	{{0,5,12},{1,6,3},{2,4,8,10,13},{7,11,9}} } );

		emplace ("m_subd",		Hypermap {	{{0,5,17,23,18,6},{1,9,19,20,10,2},{3,13,21,22,14,4},{7,8},{11,12},{15,16}},
		                  		          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23}},
		                  		          	{{0,2,4},{1,6,8},{3,10,12},{5,14,16},{7,18,9},{11,20,13},{15,22,17},{19,23,21}} } );

		emplace ("m_flower",	Hypermap {	{{0,5,17,6},{1,9,10,2},{3,13,14,4},{7,19,27,28,20,8},{11,21,31,32,22,12},
		                    	          		{15,23,35,24,18,16},{25,41,36,26},{29,37,38,30},{33,39,40,34}},
		                    	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},
		                    	          		{22,23},{24,25},{26,27},{28,29},{30,31},{32,33},{34,35},{36,37},{38,39},{40,41}},
		                    	          	{{0,2,4},{1,6,8},{3,10,12},{5,14,16},{7,17,18},{9,20,11},{13,22,15},
		                    	          		{19,24,26},{21,28,30},{23,32,34},{25,35,40},{27,36,29},{31,38,33},{37,41,39}} } );

		emplace ("m_legall",	Hypermap {	{{0,27,8,7},{1,2,14},{3,4},{5,6,13},{9,16,10},{11,21,22,24,15,12},{17,18},{19,26,25,20},{23}},
		                    	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23},{24,25},{26,27}},
		                    	          	{{0,14,24,26},{1,7,5,3},{2,4,13,15},{6,8,10,12},{9,27,19,17},{11,16,18,20},{21,25,22,23}} } );

		emplace ("m_legall2",	Hypermap {	{{0,2,4},{1,8,10},{3,23,16,14,13},{5,6},{7,12,9},{11,15,21,22},{17,18},{19,20}},
		                     	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23}},
		                     	          	{{0,10,22,3},{1,4,6,9},{2,13,7,5},{8,12,14,11},{15,16,18,20},{17,23,21,19}} } );

		emplace ("m_tetrahedron",	Hypermap {	{{0,2,4},{1,11,6},{3,7,8},{5,9,10}},
		                         	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11}},
		                         	          	{{0,6,3},{1,4,10},{2,8,5},{7,11,9}} } );

		emplace ("m_cube",	Hypermap {	{{0,7,8},{1,10,2},{3,12,4},{5,14,6},{9,23,16},{11,17,18},{13,19,20},{15,21,22}},
		                  	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23}},
		                  	          	{{0,2,4,6},{1,8,16,11},{3,10,18,13},{5,12,20,15},{7,14,22,9},{17,23,21,19}} } );

		emplace ("m_dodecahedron",	Hypermap {	{{0,9,10},{1,12,2},{3,14,4},{5,16,6},{8,7,18},{11,39,20},{13,23,24},{15,27,28},{17,31,32},{19,35,36},
		                          	          		{21,40,22},{25,42,26},{29,44,30},{33,46,34},{38,37,48},{41,59,50},{43,51,52},{45,53,54},{47,55,56},{49,57,58}},
		                          	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},
		                          	          		{20,21},{22,23},{24,25},{26,27},{28,29},{30,31},{32,33},{34,35},{36,37},{38,39},
		                          	          		{40,41},{42,43},{44,45},{46,47},{48,49},{50,51},{52,53},{54,55},{56,57},{58,59}},
		                          	          	{{0,2,4,6,8},{1,10,20,22,13},{3,12,24,26,15},{5,14,28,30,17},{7,16,32,34,19},{9,18,36,38,11},
		                          	          		{21,39,48,58,41},{23,40,50,43,25},{27,42,52,45,29},{31,44,54,47,33},{35,46,56,49,37},{51,59,57,55,53}} } );

		emplace ("house", Hypermap {	{{0},{1,2,22},{3,10,4},{5,6},{7,8},{9,11,12},{13,14,24},{15,20,16},{21},
		                            		{17,19,18},{23,26,28},{25,33,27},{29,30},{31,32,34},{35,38,36},{37},{39}},
		                            	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},
		                            		{20,21},{22,23},{24,25},{26,27},{28,29},{30,31},{32,33},{34,35},{36,37},{38,39}},
		                            	{{0,22,28,30,34,36,37,38,39,35,32,25,14,16,18,17,20,21,15,13,11,3,1},
		                            		{19},{5,10,9,7},{2,4,6,8,12,24,27,23},{26,33,31,29}}} );

		{ Hypermap M = at("house"); M.dessin(); emplace ("house_d", M); }
		{ Hypermap M = at("house_d"); M.dessin(); emplace ("house_dd", M); }

		// genus 1, lattices

		emplace ("lat_tri",	Hypermap {	{{0,1,2,3,4,5}},
		                   	          	{{0,3},{1,4},{2,5}},
		                   	          	{{0,2,4},{1,3,5}} } );

		emplace ("lat_tri_2",	Hypermap {	{{0,1,2,3,4,5},{6,7}},
		                     	          	{{0,6},{1,4},{2,5},{3,7}},
		                     	          	{{0,7,2,4},{1,3,6,5}} } );

		emplace ("lat_tri_3",	Hypermap {	{{0,6,9,5,2,1,7,8,4,3},{10,11}},
		                     	          	{{0,1},{2,3},{4,6},{5,7},{8,10},{9,11}},
		                     	          	{{0,2,4},{1,3,5},{6,8,11},{7,9,10}} } );

		emplace ("lat_tri_bi",	Hypermap {	{{0,5,11,8,4,3},{1,7,10,9,6,2}},
		                      	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11}},
		                      	          	{{0,2,4},{1,3,6},{5,8,10},{7,9,11}} } );

		emplace ("lat_tri_67",	Hypermap {	{{0,1,2,3,4,5,6,7,8,9},{10,11,12,13,14},{15,16,17}},
		                      	          	{{0,5},{1,14},{2,17},{3,7},{4,10},{6,11},{8,16},{9,12},{13,15}},
		                      	          	{{0,4,14},{1,13,17},{2,16,7},{3,6,10},{5,9,11},{8,15,12}} } );

		emplace ("lat_tri_67b",	Hypermap {	{{0,1,2,3,4,5,6,7,8,9,10,11},{12,13,14,15},{16,17,18,19},{20,21,22,23},{24,25,26,27,28,29}},
		                       	          	{{0,6},{1,29},{2,16},{3,12},{4,8},{5,24},{7,25},{9,15},{10,22},{11,26},{13,19},{14,23},{17,28},{18,20},{21,27}},
		                       	          	{{0,5,29},{1,28,16},{2,19,12},{3,15,8},{4,7,24},{6,11,25},{9,14,22},{10,21,26},{13,18,23},{17,27,20}} } );

		emplace ("lat_8862",	Hypermap {	{{0,1,2,3,4,5},{6,7,8,9,10,11,12,13},{14,15,16,17,18,19,20,21},{22,23}},
		                    	          	{{0,6},{1,20},{2,8},{3,14},{4,12},{5,16},{7,15},{9,19},{10,22},{11,17},{13,21},{18,23}},
		                    	          	{{0,13,20},{1,19,8},{2,7,14},{3,21,12},{4,11,16},{5,15,6},{9,18,22},{10,23,17}} } );

		emplace ("lat_csquare",	Hypermap {	{{0,1,2,3,4,5,6,7},{11,10,9,8}},
		                       	          	{{0,4},{1,11},{2,6},{3,10},{5,9},{7,8}},
		                       	          	{{0,3,11},{1,8,6},{2,5,10},{4,7,9}} } );

		emplace ("lat_C5",	Hypermap {	{{0,29,15,16,6,12,5},{1,2,19,27},{7,25,18,20,8},{9,22,28,26,24,17,10,13},{3,4,11,14,23,21}},
		                  	          	{{0,1},{2,3},{4,5},{6,7},{8,9},{10,11},{12,13},{14,15},{16,17},{18,19},{20,21},{22,23},{24,25},{26,27},{28,29}},
		                  	          	{{11,17,15},{4,12,10},{6,8,13},{20,23,9},{1,5,3},{18,2,21},{16,24,7},{19,25,26},{0,27,28},{22,14,29}} } );

		emplace ("lat_SV",	Hypermap {	{{1,2,3,4,5,6,7},{8,9,10,11,12},{13,14,15,16,17},{18,19,20,21,22,23,0}},
		                  	          	{{0,4},{1,10},{2,22},{3,15},{5,8},{6,19},{7,13},{9,23},{11,17},{12,20},{14,18},{16,21}},
		                  	          	{{0,3,14},{1,9,22},{2,21,15},{4,23,8},{5,12,19},{6,18,13},{7,17,10},{11,16,20}} } );

		emplace ("lat_KS5",	Hypermap {	{{0,2},{1,3,4}},
		                   	          	{{0,1,3},{2,4}},
		                   	          	{{0,1,2,3,4}} } );

		emplace ("lat_KS5t",	Hypermap {	{{0,1,2,3},{4,5,6,7},{8,9,10,11,12,13},{14,15,16,17,18,19},{20,21,22,23,24,25,26,27,28,29}},
		                    	          	{{0,6},{1,29},{2,19},{3,25},{4,10},{5,20},{7,24},{8,17},{9,21},{11,23},{12,15},{13,27},{14,28},{16,22},{18,26}},
		                    	          	{{0,5,29},{1,28,19},{2,18,25},{3,24,6},{4,9,20},{7,23,10},{8,16,21},{11,22,15},{12,14,27},{13,26,17}} } );
	}

	Hypermap H_artem (int n) {
		std::vector<unsigned> sigma(6*n), alpha(6*n), phi(6*n);
		for (int i=0; i<n; ++i) {
			sigma[i]=2*n+i; sigma[n+i]=5*n+i+1; sigma[2*n+i]=n+i-1; sigma[3*n+i]=3*n+i+1; sigma[4*n+i]=4*n+i-1; sigma[5*n+i]=i;
			alpha[i]=n+i; alpha[n+i]=i; alpha[2*n+i]=3*n+i; alpha[3*n+i]=2*n+i; alpha[4*n+i]=5*n+i; alpha[5*n+i]=4*n+i;
			phi[i]=2*n+i+1; phi[n+i]=5*n+i; phi[2*n+i]=3*n+i-1; phi[3*n+i]=i; phi[4*n+i]=n+i-1; phi[5*n+i]=4*n+i+1;
		}
		sigma[2*n-1]=3*n; sigma[2*n]=5*n-1; sigma[4*n-1]=5*n; sigma[4*n]=2*n-1;
		phi[n-1]=4*n; phi[2*n]=2*n-1; phi[3*n]=0; phi[4*n]=4*n-1; phi[6*n-1]=2*n;
		return Hypermap (sigma, alpha, phi);
	}

	Hypermap H_genus0 (int n) {
		std::vector<unsigned> sigma(6*n), alpha(6*n), phi(6*n);
		for (int i=0; i<n; ++i) { int j = (i+1)%n, k = (i+n-1)%n;
			sigma[i]=j; sigma[n+i]=3*n+k; sigma[2*n+i]=n+i; sigma[3*n+i]=4*n+j; sigma[4*n+i]=2*n+i; sigma[5*n+i]=5*n+k;
			alpha[i]=n+i; alpha[n+i]=i; alpha[2*n+i]=3*n+i; alpha[3*n+i]=2*n+i; alpha[4*n+i]=5*n+i; alpha[5*n+i]=4*n+i;
			phi[i]=2*n+i; phi[n+i]=k; phi[2*n+i]=n+j; phi[3*n+i]=4*n+i; phi[4*n+i]=5*n+j; phi[5*n+i]=3*n+k;
		}
		return Hypermap (sigma, alpha, phi);
	}

	Hypermap H_genus1 (int n) {
		std::vector<unsigned> sigma(6*n), alpha(6*n), phi(6*n);
		for (int i=0; i<n; ++i) { int j = (i+1)%n, k = (i+n-1)%n;
			sigma[6*i]=6*i+1; sigma[6*i+1]=6*i+2; sigma[6*i+2]=6*i+3; sigma[6*i+3]=6*i+4; sigma[6*i+4]=6*i+5; sigma[6*i+5]=6*i;
			alpha[6*i]=6*j+3; alpha[6*i+1]=6*j+4; alpha[6*i+2]=6*i+5; alpha[6*i+3]=6*k; alpha[6*i+4]=6*k+1; alpha[6*i+5]=6*i+2;
			phi[6*i]=6*j+2; phi[6*i+1]=6*j+3; phi[6*i+2]=6*i+4; phi[6*i+3]=6*k+5; phi[6*i+4]=6*k; phi[6*i+5]=6*i+1;
		}
		return Hypermap (sigma, alpha, phi);
	}
}
