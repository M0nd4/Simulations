/** Classes to build nice figures.
 *
 * The main output is a .asy file (Asymptote format), later there might
 * be others, especially EPS would be nice.
 */

#ifndef __VB_FIGURE_H
#define __VB_FIGURE_H

#include <iostream>
#include <sstream>
#include <complex>
#include <list>
#include <stdexcept>

#include <vb/types.h>

namespace vb {

  /// Base class for the elements of a figure.

  class Shape {
    public:
      /// Empty destructor to make the compiler happy.
      virtual ~Shape() {}

      /// Write ASY code for the shape.
      virtual std::ostream &printASY (std::ostream &os) {
        not_impl ("Asymptote"); return os;
      }

      /// Write EPS code for the shape.
      virtual std::ostream &printEPS (std::ostream &os) {
        not_impl ("EPS"); return os;
      }

      /// Write MP code for the shape.
      virtual std::ostream &printMP (std::ostream &os) {
        not_impl ("MetaPost"); return os;
      }

    private:
      /// Complain that some output is not implemented.
      void not_impl (std::string s) {
        std::ostringstream os;
        os << s << " output not implemented by class " << typeid(*this).name() << std::flush;
        throw (std::runtime_error(os.str()));
      }
  };

  /// Line segment.

  class Segment : public Shape {
    public:
      /// Constructor from two complex numbers.
      Segment (cpx zz1, cpx zz2) : z1(zz1), z2(zz2) {}

      /// Implementation of ASY output.
      virtual std::ostream &printASY (std::ostream &os) {
        return os << "draw (" << z1 << "--" << z2 << ");" << std::endl;
      }

    private:
      cpx z1; ///< First endpoint.
      cpx z2; ///< Second endpoint.
  };

  /// Circle.

  class Circle : public Shape {
    public:
      /// Constructor from center and radius.
      Circle (cpx zz, real rr) : z(zz), r(rr) {}

      /// Implementation of ASY output.
      virtual std::ostream &printASY (std::ostream &os) {
        return os << "draw (circle(" << z << "," << r << "));" << std::endl;
      }

    private:
      cpx z;  ///< The center.
      real r; ///< The radius.
  };

  /** The main Figure class.
   *
   * Right now it's a list of shapes with an output routine.
   */

  class Figure {
    public:
      /// Add a segment to the figure.
      Figure &segment (cpx z1, cpx z2) {
        return add (new Segment (z1,z2));
      }

      /// Add a circle to the figure.
      Figure &circle (cpx z, real r) {
        return add (new Circle (z,r));
      }

      /// Output as an ASY file.
      std::ostream &printASY (std::ostream &os) {
        os << "unitsize(100);" << std::endl;

        std::list<Shape*>::iterator i;
        for (i = contents.begin(); i != contents.end(); ++i) {
          (*i)->printASY(os);
        }
        return os;
      }

    private:
      std::list<Shape*> contents;  ///< The elements of the figure.

      /// Add an element to the figure.
      Figure &add (Shape *S) {
        contents.push_back(S);
        return (*this);
      }
  };
}

#endif
