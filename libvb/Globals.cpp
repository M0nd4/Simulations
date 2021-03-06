/// @file
/// Implementation of useful global variables.

#include <vb/PRNG.h>
#include <vb/Auto.h>
#include <vb/coo.h>

namespace vb {
	PRNG prng;
	std::vector<Task> Auto::tasks;

	const coo dz[8] = { coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1) };
	const coo3 dz3[6] = { coo3(1,0,0), coo3(-1,0,0), coo3(0,1,0), coo3(0,-1,0), coo3(0,0,1), coo3(0,0,-1) };

	template <> int to_int (const cln::cl_I & z) { return cl_I_to_int(z); }
	template <> int to_int (const cln::cl_R & z) { return cl_I_to_int(floor1(z)); }

	template <> complex_t to_cpx <real_t> (const real_t & x, const real_t & y) { return cln::complex (x,y); }
}
