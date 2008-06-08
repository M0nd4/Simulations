
# This is a simple wrapper around SCons for convenience
#
# Change the lines below (or use the command line) accordingly : PREFIX 
# is the installation prefix, GUI is either 'fltk' (try to use fltk if 
# present) or 'none' (don't even try).

PREFIX = /usr/local
GUI = fltk

all: tools/scons.py
	python tools/scons.py GUI=$(GUI) prefix=$(PREFIX)

clean:
	rm -rf build

distclean: clean
	rm -rf .scon* config.log simulations.conf
	cd tools; rm -rf scons-LICENSE scons-README scons-local-0.98.4 scons*.py

tools/scons.py:
	cd tools; tar zxvf scons-local-0.98.4.tar.gz