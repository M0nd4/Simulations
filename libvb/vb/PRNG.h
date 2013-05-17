#pragma once /// \file
#include <vb/common.h>

namespace vb {
	template <typename G> class PRNG_base : public G {
	public:
		PRNG_base (unsigned long s=0) { if (s) G::seed(s); }

		// For some reason default parameters are much slower than independent implementation ...
		double bernoulli   	(double p)              	{ return (boost::bernoulli_distribution<>  	(p))        	(*this); }
		double bernoulli   	()                      	{ return (boost::bernoulli_distribution<>  	(.5))       	(*this); }
		double exponential 	(double lambda)         	{ return (boost::exponential_distribution<>	(lambda))   	(*this); }
		double exponential 	()                      	{ return (boost::exponential_distribution<>	(1.0))      	(*this); }
		double gaussian    	(double m, double sigma)	{ return (boost::normal_distribution<>     	(m,sigma))  	(*this); }
		double gaussian    	(double m)              	{ return (boost::normal_distribution<>     	(m,1.0))    	(*this); }
		double gaussian    	()                      	{ return (boost::normal_distribution<>     	(0.0,1.0))  	(*this); }
		double geometric   	(double p)              	{ return (boost::geometric_distribution<>  	(p))        	(*this); }
		double geometric   	()                      	{ return (boost::geometric_distribution<>  	(.5))       	(*this); }
		double poisson     	(double lambda)         	{ return (boost::poisson_distribution<>    	(lambda))   	(*this); }
		double poisson     	()                      	{ return (boost::poisson_distribution<>    	(1.0))      	(*this); }
		double uniform_real	(double min, double max)	{ return (boost::uniform_real<>            	(min, max)) 	(*this); }
		double uniform_real	(double max)            	{ return (boost::uniform_real<>            	(0.0, max)) 	(*this); }
		double uniform_real	()                      	{ return (boost::uniform_real<>            	(0.0, 1.0)) 	(*this); }
		int    uniform_int 	(int mmax)              	{ return (boost::uniform_int<>             	(0, mmax-1))	(*this); }
	};

	typedef PRNG_base <boost::mt19937> PRNG;
	extern PRNG prng;

	class PRNG_Rewindable {
	public:
		PRNG_Rewindable (long aa=13, long bb=257, long mm=2147483647) {
			max = mm; a = aa; b = bb;
			long long  t_a=a, t_b=max, t_u=1, t_v=0, t_s=0, t_t=1;

			while (t_b != 0) {
				long long  t_q = t_a/t_b;
				long long  t_r = t_a - t_b*t_q;
				t_a = t_b; t_b = t_r;

				long long  r1 = t_u - t_q*t_s;
				t_u = t_s; t_s = r1;

				long long  r2 = t_v - t_q*t_t;
				t_v = t_t; t_t = r2;
			}

			r_a = t_u;
			r_b = (long) ( (- (long long)b * (long long)r_a ) % (long long)max );

			struct timeval tv;
			time_t curtime;

			gettimeofday (&tv, NULL);
			curtime = tv.tv_sec;
			rdmbuf = tv.tv_usec;
		}

		void iterate (long long a, long long b, long long n) {
			while (n>0) {
				if (n%2) rdmbuf = (a*rdmbuf+b) % max;
				b = ((a+1)*b) % max;
				a = (a*a) % max;
				n >>= 1;
			}
		}

		void rewind (long time1, long time2) { iterate (r_a, r_b, (long long)time1 * (long long)time2); }

		unsigned long operator() () { rdmbuf = (a*rdmbuf+b)%max; return (unsigned long) rdmbuf; }

		unsigned long max;

	private:
		long a,b,r_a,r_b;
		long long rdmbuf;
	};
}
