#include <vb/Cluster.h>

namespace vb {
	bool Cluster::fits (coo z) const {
		return (z.x >= ul.x) && (z.y >= ul.y) && (z.x < ul.x + w) && (z.y < ul.y + w);
	}

	void Cluster::ensure_sub () {
		if (sub.size()>0) return;
		int ww = w/3;
		for (int y=0; y<3; ++y) for (int x=0; x<3; ++x) sub.push_back (Cluster (ul + coo(x*ww,y*ww), ww));
		if (np == w*w) for (auto & c : sub) c.np = ww*ww;
	}

	void Cluster::grow () {
		if (np > 0) {
			std::vector <Cluster> bak; bak.swap(sub);
			ul.x -= w; ul.y -= w; w *= 3; ensure_sub();
			Cluster &c = sub[4]; tile.swap(c.tile); c.np = np; c.sub.swap(bak);
		} else {
			ul.x -= w; ul.y -= w; w *= 3;
		}
	};

	bool Cluster::at (coo z) const {
		if ((np == 0) || (!fits(z))) return false;
		if (np == w*w) return true;
		if (w == bs) { int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y; return tile[xy]; }
		else { int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww; return sub[i+3*j].at(z); }
	};

	void Cluster::put (coo z, bool b, bool quick) {
		if (np == w*w*b) return;
		if (!quick) { if (at(z) == b) return; while (!fits(z)) grow(); }
		if (w == bs) {
			if (!tile.size()) tile.resize (w*w,!b);
			int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y;
			np += (b?1:-1); tile [xy] = b;
			if (np == (b ? w*w : 0)) std::vector<bool>().swap(tile);
		} else {
			int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
			ensure_sub(); np += (b?1:-1); sub[i+3*j].put(z,b,true);
			if (np == (b ? w*w : 0)) std::vector<Cluster>().swap(sub);
		}
	}

	void Cluster::insert (coo z) { put (z,true); }

	void Cluster::remove (coo z) { put (z,false); }

	void Cluster::validate () {
		if ((np==0) || (np==w*w)) { assert (!tile.size()); assert (!sub.size()); }
		if (sub.size()) {
			int s=0; for (auto & c : sub) { c.validate(); s += c.np; } assert (s==np);
		}
	}

	void Cluster::dump (std::string pre) {
		validate ();
		if (np == 0) { if (pre=="") std::cerr << "EMPTY" << std::endl; return; }
		if (np == w*w) { std::cerr << pre << "FULL in " << ul << "[" << w << "]" << std::endl; return; }

		std::cerr << pre << np << " in " << ul << "[" << w << "]" << std::endl;
		if ((np > 0) && (np < w*w)) {
			for (auto & c : sub) c.dump (pre + "    ");
		}
	}

	int Cluster::bs = 32;
}
