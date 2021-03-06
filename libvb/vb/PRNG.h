#pragma once /// \file
#define BOOST_DISABLE_ASSERTS 1
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <random>
#include <sstream>
#include <vector>

namespace vb {
	class PRNG : public boost::mt19937_64 {
	public:
		PRNG (unsigned long s=0) : boost::mt19937_64 (s ? s : std::random_device()()) {}

		bool  	bernoulli   	(double p=.5)               	{ return (boost::bernoulli_distribution<>  	(p))        	(*this); }
		int   	uniform_int 	(int mmax)                  	{ return (boost::uniform_int<>             	(0, mmax-1))	(*this); }
		int   	geometric   	(double p=.5)               	{ return (boost::geometric_distribution<>  	(p))        	(*this); }
		int   	poisson     	(double lambda=1)           	{ return (boost::poisson_distribution<>    	(lambda))   	(*this); }
		double	uniform_real	(double min=0, double max=1)	{ return (boost::uniform_real<>            	(min, max)) 	(*this); }
		double	exponential 	(double lambda=1)           	{ return (boost::exponential_distribution<>	(lambda))   	(*this); }
		double	gaussian    	(double m=0, double sigma=1)	{ return (boost::normal_distribution<>     	(m,sigma))  	(*this); }

		int	discrete (const std::vector<double> & p) {
		   	double U = uniform_real(); int i=0;
		   	while (U>p[i]) { U -= p[i]; ++i; }
		   	return i;
		}

		std::string	state ()                    	{ return boost::lexical_cast<std::string> (*this); }
		void       	state (const std::string &s)	{ std::istringstream is (s); is >> (*this); }
	};

	extern PRNG prng;
}
