#include <vb/QuadTree.h>

namespace vb {
	std::vector <QuadTree *> QuadTree::store;

	QuadTree::QuadTree (coo UL, coo BR, int M) :
		n(0), ul(UL), br(BR), center((ul+br)/2), iul(br), ibr(ul), m(M) {}

	QuadTree::~QuadTree () {
		if (n>m) for (int i=0; i<4; ++i) delete store[ch+i];
	}

	int QuadTree::index (coo z) const {
		if (z.y<center.y)	{ if (z.x<center.x) return 0; else return 1; }
		else             	{ if (z.x<center.x) return 2; else return 3; }
	}

	void QuadTree::insert (coo z) {
		iul.x = std::min(iul.x,z.x); iul.y = std::min(iul.y,z.y);
		ibr.x = std::max(ibr.x,z.x); ibr.y = std::max(ibr.y,z.y);
		if (n==m) split();
		if (n<m) pts.push_back(z); else store[ch+index(z)] -> insert (z);
		++n;
	}

	void QuadTree::split () {
		ch = store.size();
		store.push_back (new QuadTree (ul,                	center,            	m));
		store.push_back (new QuadTree (coo(center.x,ul.y),	coo(br.x,center.y),	m));
		store.push_back (new QuadTree (coo(ul.x,center.y),	coo(center.x,br.y),	m));
		store.push_back (new QuadTree (center,            	br,                	m));
		for (auto & z : pts) store[ch+index(z)] -> insert (z);
		std::vector<coo>().swap(pts);
	}

	int QuadTree::idist (coo z) const { return std::min ({z.x-ul.x, 	z.y-ul.y, 	br.x-z.x, 	br.y-z.y}); 	}
	int QuadTree::odist (coo z) const { return std::max ({z.x-ibr.x,	z.y-ibr.y,	iul.x-z.x,	iul.y-z.y});	}

	void QuadTree::nn (coo z, QuadIndex & qi) const {
		if (n<=m) for (auto & w : pts) {
			int newnorm = sup (z-w);
			if (newnorm < qi.d) { qi.d = newnorm; qi.z = w; }
		} else {
			int i0 = index(z); if (store[ch+i0] -> n) store[ch+i0] -> nn (z,qi);
			for (int i=0; i<4; ++i) if (i != i0) {
				QuadTree *q = store[ch+i];
				if ((q -> n) && (q -> odist(z) < qi.d)) q -> nn (z,qi);
			}
		}
	}

	void QuadTree::paint (Image & img, coo ul, int w) {
		if (w==1) { img.at(ul) = (n>0) ? GREEN : BLACK; return; }
		if (n>m) {
			store[ch+0] -> paint (img, ul, w/2);
			store[ch+1] -> paint (img, ul+coo(w/2,0), w/2);
			store[ch+2] -> paint (img, ul+coo(0,w/2), w/2);
			store[ch+3] -> paint (img, ul+coo(w/2,w/2), w/2);
			return;
		}
		for (int i=0; i<w; ++i) { img.at(ul+coo(i,0)) = Color(128,0,0); img.at(ul+coo(0,i)) = Color(128,0,0); }
	}
}
