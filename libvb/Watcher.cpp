
#include <vb/Watcher.h>

namespace vb {
  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  Watcher::Watcher () : AutoWindow (400,0,"Watcher") {
    task = global_clock.add (100, Watcher_cerr, this);
  }

  Watcher::~Watcher () {
    global_clock.remove(task);
  }

  void Watcher::add (Value_base *v) {
    l.push_back (v);
    size (w(), h()+30);
    begin();
    new Fl_Button (0,h()-30, 150,30, v->name.c_str());
    o.push_back (new Fl_Output (150,h()-30, 250,30));
    end();
  }

  void Watcher::draw () {
    for (int i = 0; i < l.size(); ++i) {
      std::ostringstream os; os << l[i];
      o[i]->value(os.str().c_str());
    }
    AutoWindow::draw();
  }

  std::ostream & operator<< (std::ostream &os, const Watcher &W) {
    for (int i = 0; i < W.l.size(); ++i) {
      os << " | ";
      if (W.l[i]->name != "") os << W.l[i]->name << " = ";
      os << W.l[i];
    }
    return os << " |" << std::endl;
  }

  void Watcher_cerr (void * W) { std::cerr << *((Watcher*)W); }
}
