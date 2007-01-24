
/// @file vb.h
/// Include everything from the libvb library.

#include <vb/config.h>

#include <vb/Image.h>
#include <vb/CoarseImage.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Point.h>

#include <string>

/** The namespace where libvb puts everything it defines. */

namespace vb {

  /** Makes an EPS file out of a path in Z^2.
   *
   * The path is stored as an array of chars, taking values between 0
   * and 3. The path is clipped automatically.
   *
   * @param p The path data.
   * @param l The length of the path.
   * @param title The title of the created image.
   */

  int printout_path (char *p, long l, std::string title);
}

/** @mainpage Documentation for the `libvb' library.
 *
 * @section sec_intro Introduction
 *
 * The aim of this library is to provide an easy way for mathematicians
 * (and maybe others) to display 2D graphics for their simulations and
 * to produce EPS files for easy inclusion in a LaTeX document. The main
 * design goal is that usage should be as simple as possible; as such,
 * the library is not very efficient (no fancy OpenGL or even hardware
 * acceleration) but the interface is straightforward. Among the useful
 * features:
 *
 * - vb::Image: the main class provided by the library. It centralizes
 *   most of the functionality, can be displayed on the screen by
 *   calling vb::Image::onscreen() and transformed into an EPS file via
 *   an output stream (std::cout << img).
 *
 * - Auto-update: the display routine (namely vb::Image::putpoint())
 *   will try to update the screen representation of the image on the
 *   screen about 25 times per second. It works best if the computation
 *   time is roughly the same for all the points displayed. This is
 *   extremely practical.
 *
 * - vb::Image::tessellate(): a quick way to fill an image according to a
 *   coloring function (which gives the gray level as a function of
 *   coordinates) by a dyadic decomposition.
 *
 * - A helper class vb::CL_Parser to access a simple but often
 *   sufficient subset of getopt. Supports default values for parameters
 *   and a personalized help message. I use it myself even for programs
 *   without a display.
 *
 * - vb::CoarseImage: a simple way to store and display very large
 *   images that consist of large black and white zones (typically
 *   first-passage percolation clusters).
 *
 * And once again: this is all supposed to be very easy to use. If it
 * isn't, please let me know (vbeffara@ens-lyon.fr) ! See the sample
 * code included in the documentation for an easy way to get started.
 *
 * @section sec_install Installation
 *
 * There is no separate package for the library (though I might produce
 * one at some point), so you will have to download all my simulations.
 * On the other hand, it will give you some sample code, which cannot
 * hurt. See http://www.umpa.ens-lyon.fr/~vbeffara/simu.php for details.
 * Once you have it, it is a simple matter of the usual ./configure;
 * make; make install.
 *
 * The main dependency is on the SDL library (http://www.libsdl.org). While
 * technically libvb will work without it, in the sense that the programs
 * will run, either the display will use the standard X11 libraries, if
 * available and with less features, or you will get no display at all.
 * The EPS output of course works in all cases.
 */
