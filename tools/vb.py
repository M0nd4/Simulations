#!/usr/bin/env python
# coding=iso-8859-1

"""Utility classes to go with libvb."""

__version__ = "0.1"
__revision__ = "2"
__author__ = "Vincent Beffara <vbeffara@ens-lyon.fr>"
__copyright__ = "(c) 2008 VB. GNU GPL v3."

import sys, os, re, random
from subprocess import *

from numpy import *
from scipy.optimize import leastsq

class mc_data (object):
    def __init__ (self, file=None):
        self.data = {}
        self.comments = []
        self.dirty = False
        self.file = file

        if self.file:
            if not os.path.isfile (self.file):
                open (self.file, "w")
            self.read (self.file)

    def __del__ (self):
        if self.dirty:
            self.save()

    def parse (self, l):
        (k,n,v) = l.strip().split (" | ", 2)

        n = int(n)
        key = tuple(float(s) for s in k.split())
        vs = [int(s.split()[0]) for s in v.split(" | ")]

        if key not in self.data:
            self.data[key] = [ 0, [0 for i in vs] ]

        self.data[key][0] += n
        for i in range(len(vs)):
            self.data[key][1][i] += vs[i]

        self.dirty = True
        return key

    def read (self, file):
        for l in open(file):
            if l[0] == "#":
                self.comments.append (l)
            else:
                self.parse (l)

    def dump (self, file=None):
        if file:
            f = open (file+".tmp","w")
        else:
            f = sys.stdout

        for l in self.comments:
            f.write (l)

        outstrings = {}
        for key in self.data:
            k = " ".join(str(i) for i in key)
            n = self.data[key][0]
            v = []

            for x in self.data[key][1]:
                r = float(x)/n
                v.append ("%d %f %f" % (x, r, sqrt(r*(1-r)/n)))

            outstrings[key] = (k,str(n),v)

        max_k = max(len(k) for (k,n,vs) in outstrings.values())
        max_n = max(len(n) for (k,n,vs) in outstrings.values())
        max_v = max(len(v) for (k,n,vs) in outstrings.values() for v in vs)

        for key in sorted(self.data):
            k,n,v = outstrings[key]

            k = k.ljust(max_k)
            n = n.rjust(max_n)
            v = [s.rjust(max_v) for s in v]

            f.write ("%s | %s | %s\n" % (k,n," | ".join(v)))

        if file:
            f.close()
            os.rename (file+".tmp", file)

    def save (self):
        if self.file:
            self.dump (self.file)
            self.dirty = False

class mc_auto (mc_data):
    def __init__ (self, file):
        mc_data.__init__ (self,file)

        self.command = ""
        self.fitstr = ""

        for l in self.comments:
            if l[1] == "!":
                self.command = l[2:].strip()
            if l[1] == "?":
                self.fitstr = l[2:].strip()

        if not self.fitstr:
            self.fitstr = "a*x+b"

        self.fitstr = self.fitstr.replace ('a','v[0]')
        self.fitstr = self.fitstr.replace ('b','v[1]')
        self.fitstr = self.fitstr.replace ('c','v[2]')
        self.fitstr = self.fitstr.replace ('d','v[3]')

        self.fitstr = self.fitstr.replace ('x','x[0]')
        self.fitstr = self.fitstr.replace ('y','x[1]')
        self.fitstr = self.fitstr.replace ('z','x[2]')

        self.f = eval ("lambda v,x: " + self.fitstr, {})

        self.v = []
        for i in range(4):
            if ("v[%d]" % i) in self.fitstr:
                self.v.append (.495876945)

        self.n = array ([self.data[key][0] for key in self.data])
        self.x = array (self.data.keys())
        self.y = array ([float(self.data[key][1][0]) for key in self.data]) / self.n
        self.z = sqrt (self.y*(1.0-self.y)/self.n)

        self.fx = 0
        self.above = 0
        self.below = 0

        self.fit()

    def e (self, v):
        ff = array ([self.f (v,x) for x in self.x])
        return (ff-self.y)/self.z

    def fit (self):
        self.v, success = leastsq (self.e, self.v)

        self.fx = array ([self.f (self.v,x) for x in self.x])
        self.above = self.fx - self.y + 2*self.z
        self.below = self.fx - self.y - 2*self.z

        print self.v

        return success

    def run (self,key):
        keydict = dict ( (str(i+1),key[i]) for i in range(len(key)) )
        cmdstr = re.sub ('\$([0-9]*)', '%(\\1)s', self.command)
        cmd = (cmdstr % keydict).split()

        P = Popen (cmd, stdout=PIPE, stderr=open("/dev/null","w"))
        return P.communicate()[0]

    def run_once (self, key):
        self.parse (self.run (key))
        self.save()

        i = self.data.keys().index(key)

        self.n[i] = self.data[key][0]
        self.y[i] = float(self.data[key][1][0]) / self.n[i]
        self.z[i] = sqrt (self.y[i] * (1.0-self.y[i]) / self.n[i])

        self.fit()

    def err (self):
        sqerr = sum(i*i for i in self.above) + sum(i*i for i in self.below)

        return sqrt (0.5 * sqerr / len(self.x))

    def next (self):
        if self.above.max() + self.below.min() > 0:
            m = self.above.argmax()
        else:
            m = self.below.argmin()

        return self.data.keys()[m]

    def loop (self):
        while True:
            m = self.next()
            #print self.v, self.err(), "=>", " ".join(str(i) for i in m)
            self.run_once (m)
