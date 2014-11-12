#include <vb/NumberTheory.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	vector<gmp100> xs { sqrt(gmp100(2)), 4*atan(gmp100(1)), exp(gmp100(1)), exp(log(gmp100(2))/7), (1+sqrt(gmp100(5)))/2,
		gmp100("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255"),
		gmp100("1.36602540378443864676372317075293618347140262690519031402790348972596650845440001854057309338"),
		gmp100("0.5877852522924731291687059546390727685976524376431459910722724807572784741623519575085040499"),
		gmp100("0.8090169943749474241022934171828190588601545899028814310677243113526302314094512248536036021"),
		gmp100("798303.3673469387755102040816326530612244897959183673469387755102040816326530612244897959183673469"),
		gmp100("1.12266701157648291040117446489401135452719640061624479423083713099312644668872851836902659616"),
		gmp100("0.63827397417446081629048447976042972714028217652392199657870122677085361940416547100605619666") };

	cout << setprecision(100);
	for (auto x : xs) {
		cout << "x = " << x << endl;
		Polynomial<bigint> P = guess (x, gmp100(1e-80));
		if (P.degree()>0) cout << "  root of " << P << endl;
		cout << endl;
	}

	cpx100 z (gmp100("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632"),
		gmp100("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543"));
	cout << "x = " << z << endl;
	Polynomial<cpxint> P = guess (z, gmp100(1e-80));
	if (P.degree()>0) cout << "  root of " << P << endl;
}