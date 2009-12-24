/// @file MatrixStorage.h
/// Definitions of various kinds of matrix storage.

#ifndef __VB_MATRIXSTORAGE_H
#define __VB_MATRIXSTORAGE_H

#include <vb/Vector.h>

namespace vb {

  /** Abstract base class for a general matrix storage class.
   *
   * All matrix-matrix operations return a pointer to a (possibly 
   * different) MatrixStorage subclass, it is the responsibility of the 
   * surrounding vb::Matrix to juggle between them. Not sure that this 
   * is such a wonderful way of doing it, but it works pretty well and 
   * is transparent to the user.
   *
   * @todo Add in-place versions of as many things as possible.
   */

  class MatrixStorage {
    public:
      unsigned int width;  ///< The width of the matrix;
      unsigned int height; ///< The height of the matrix.

      /** Standard constructor.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage (unsigned int h, unsigned int w) : width(w), height(h) {}

      /// Standard virtual destructor.

      virtual ~MatrixStorage () {}

      /// Produce a copy of the object and return it.

      virtual MatrixStorage *copy () =0;

      /// Compute each of the entry, converting to MatrixStorage_Plain.

      virtual MatrixStorage *compute () =0;

      /** Return the entry at the given coordinates.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      virtual double at (unsigned int i, unsigned int j) const =0;

      /** Set the value of a particular matrix entry.
       *
       * @param i The line of the entry.
       * @param j The line of the entry.
       * @param t The new value of the entry.
       */

      virtual MatrixStorage *put (unsigned int i, unsigned int j, double t) =0;

      /** Add a MatrixStorage to the current one.
       *
       * NB There is no add(const vb::Matrix) because it is not the 
       * point ... MatrixStorage does not know about Matrix. It's 
       * simpler this way.
       *
       * @param M The MatrixStorage to add.
       */

      virtual MatrixStorage *add (MatrixStorage *M) =0;

      /** Subtract a MatrixStorage from the current one.
       *
       * NB There is no sub(const vb::Matrix) because it is not the 
       * point ... MatrixStorage does not know about Matrix. It's 
       * simpler this way.
       *
       * @param M The MatrixStorage to subtract.
       */

      virtual MatrixStorage *sub (MatrixStorage *M) =0;

      /** Right-multiply the current matrix by another one.
       *
       * @param M The (right) multiplicator.
       */

      virtual MatrixStorage *mul_right (MatrixStorage *M) =0;

      /** Make a rank-one update of the matrix.
       *
       * This adds the rank-one matrix A^t.B to the current matrix.
       *
       * @param A The column vector of the rank-one matrix.
       * @param B The line vector of the rank-one matrix.
       */

      virtual MatrixStorage *rank1update (const Vector &A, const Vector &B) =0;

      /** Map a vector by the current matrix.
       *
       * @param X The vector to map.
       */

      virtual Vector map_right (const Vector &X) =0;
  };

  /** Plain old matrix, storing each entry separately.
   *
   * Internally it is just a vector of lines, themselves being vectors. 
   * This is so that scalar_product() can be used for matrix 
   * multiplication.
   */

  class MatrixStorage_Plain : public MatrixStorage {
    public:
      /// The lines of the matrix.

      std::vector<Vector> lines;

      /** The standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage_Plain (unsigned int h, unsigned int w) : MatrixStorage (h,w), lines (std::vector<Vector> (h,Vector(w))) { }

      /// The standard destructor.
      virtual ~MatrixStorage_Plain () {}

      virtual MatrixStorage *copy () {
        return new MatrixStorage_Plain (*this);
      }

      virtual MatrixStorage *compute () { return this; }

      virtual double at (unsigned int i, unsigned int j) const {
        return lines[i][j];
      }

      virtual MatrixStorage *put (unsigned int i, unsigned int j, double t) {
        lines[i][j] = t; return this;
      }

      /// Add another matrix to this one.
      virtual MatrixStorage *add (MatrixStorage *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += M->at(i,j);
        return this;
      }

      /// Subtract another matrix from this one.
      virtual MatrixStorage *sub (MatrixStorage *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] -= M->at(i,j);
        return this;
      }

      /// Right-multiply this matrix with another one.
      virtual MatrixStorage *mul_right (MatrixStorage *M) {
        MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, M->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<M->width; ++j) {
            tmp->lines[i][j] = lines[i][0] * M->at(0,j);
            for (unsigned int k=1; k<this->width; ++k) 
              tmp->lines[i][j] += lines[i][k] * M->at(k,j);
          }
        }
        return tmp;
      }

      virtual MatrixStorage *rank1update (const Vector &A, const Vector &B) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += A[i]*B[j];
        return this;
      }

      virtual Vector map_right (const Vector &X) {
        Vector Y(this->height);
        for (unsigned int i=0; i<this->height; ++i)
          Y[i] = scalar_product (lines[i],X);
        return Y;
      }
  };

  /** Clever storage as a diagonal matrix plus rank-1 perturbations.
   *
   * It is extremely useful for BFGS algorithms and the like; 
   * rank1update() works in linear time (copy of the arguments) instead 
   * of quadratic, except whenever the matrix upgrades itself to 
   * MatrixStorage_Plain.
   *
   * @todo Generalize it as any matrix storage plus rank-1 updates ?
   */

  class MatrixStorage_DiagSmallRank : public MatrixStorage {
    public:
      /// The diagonal of the unperturbed matrix.

      Vector diag;

      /// The list of rank-one updates performed to the matrix.

      std::vector < std::pair<Vector,Vector> > updates;

      /** Standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w) : MatrixStorage (h,w) {
        diag = Vector (h);
        for (unsigned int i=0; i<h; ++i) diag[i]=0;
      }

      /** Constructor from a diagonal vector.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       * @param d The diagonal of the matrix.
       */

      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w, const Vector &d) : MatrixStorage (h,w), diag(d) { }

      /// The standard destructor.
      virtual ~MatrixStorage_DiagSmallRank () {}

      virtual MatrixStorage *copy () {
        return new MatrixStorage_DiagSmallRank (*this);
      }

      MatrixStorage *compute () {
        MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, this->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<updates.size(); ++j)
            tmp->lines[i] += updates[j].first[i] * updates[j].second;
        }
        for (unsigned int i=0; i<(this->height<this->width?this->height:this->width); ++i)
          tmp->lines[i][i] += diag[i];
        return tmp;
      }

      virtual double at (unsigned int i, unsigned int j) const {
        double tmp = 0;
        if (i==j) tmp += diag[i];
        for (unsigned int k=0; k<updates.size(); ++k)
          tmp += updates[k].first[i] * updates[k].second[j];
        return tmp;
      }

      virtual MatrixStorage *put (unsigned int i, unsigned int j, double t) {
        MatrixStorage *tmp = this->compute();
        tmp->put (i,j,t);
        return tmp;
      }

      /// Add another matrix to this one.
      virtual MatrixStorage *add (MatrixStorage *M) {
        MatrixStorage *tmp = this->compute();
        tmp->add(M);
        return tmp;
      }

      /// Subtract another matrix from this one.
      virtual MatrixStorage *sub (MatrixStorage *M) {
        MatrixStorage *tmp = this->compute();
        tmp->sub(M);
        return tmp;
      }

      /// Right-multiply this matrix with another one.
      virtual MatrixStorage *mul_right (MatrixStorage *M) {
        MatrixStorage *tmp = this->compute();
        tmp->mul_right(M);
        return tmp;
      }

      virtual MatrixStorage *rank1update (const Vector &A, const Vector &B) {
        updates.push_back (std::pair<Vector,Vector> (A,B));
        if (updates.size() > this->height/10) {
          return this->compute();
        } else {
          return this;
        }
      }

      virtual Vector map_right (const Vector &X) {
        Vector tmp(this->width);
        for (unsigned int i=0; i<(this->width<this->height?this->width:this->height); ++i) tmp[i] = diag[i]*X[i];
        for (unsigned int i=0; i<updates.size(); ++i) tmp += updates[i].first * scalar_product(updates[i].second,X);
        return tmp;
      }
  };
}

#endif
