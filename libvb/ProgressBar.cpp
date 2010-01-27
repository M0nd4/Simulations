
/// @file ProgressBar.cpp
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

namespace vb {
  ProgressBar::ProgressBar (int length, double pow) : 
    final(length), current(0), nchar(0), power(pow), timer (Timer(1.0)) {
      display();
    }

  void ProgressBar::update (int pos) {
    if (pos<0) pos=0;
    if (pos>final) pos=final;

    current = pos;
    int dirty = 0;
    int new_nchar = (pos*50.0)/final;

    if (timer.check()) {
      timer.reset();
      dirty = 1;
    }

    if (new_nchar > nchar) {
      nchar = new_nchar;
      timer.reset();
      dirty = 1;
    }

    if (dirty) display();
  }

  void ProgressBar::die (void) {
    update(final);
    std::cerr << "\n";
  }

  void ProgressBar::display (void) {
    std::ostringstream bar;

    bar << "\r[";
    for (int i=0; i<nchar; ++i)
      bar << "=";
    for (int i=nchar; i<50; ++i)
      bar << " ";
    bar << "]";
    
    // std::cerr << " (" << current << "/" << final << ")";

    if (timer.t_elapsed() > 0) {
      double done = pow(current,power);
      double todo = pow(final,power);
      int eta = int (timer.t_elapsed() * (todo/done - 1.0));

      bar << " ETA: ";

      if (eta < 60) bar << eta << " second(s)"; else {
        eta /= 60;
        if (eta < 60) bar << eta << " minute(s)"; else {
          eta /= 60;
          if (eta < 24) bar << eta << " hour(s)"; else {
            eta /= 24;
            if (eta < 365) bar << eta << " day(s)"; else {
              eta /= 365;
              bar << eta << " year(s)";
            }
          }
        }
      }

      bar << "     ";
    }

    std::cerr << bar.str();
  }
}
