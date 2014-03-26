#!/usr/bin/env python
#
#*****************************************************************************
#
#	 Copyright (C) 2013  Daniele Di Proietto <d.diproietto@sssup.it>
#	 
#	 This file is part of nontetris.
#	 
#	 nontetris is free software: you can redistribute it and/or modify
#	 it under the terms of the GNU General Public License as published by
#	 the Free Software Foundation, either version 3 of the License, or
#	 (at your option) any later version.
#	 
#	 nontetris is distributed in the hope that it will be useful,
#	 but WITHOUT ANY WARRANTY; without even the implied warranty of
#	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	 GNU General Public License for more details.
#	 
#	 You should have received a copy of the GNU General Public License
#	 along with nontetris.  If not, see <http://www.gnu.org/licenses/>.
#
#*****************************************************************************
#
# This should work with python2 and python3. Yay!

import json
import pylab
import sys

xmax = -float("inf")
xmin = float("inf")
ymax = -float("inf")
ymin = float("inf")

def drawpolygon(clusters):
	global xmax, xmin, ymax, ymin
	prevpoint = clusters[-1]
	for point in clusters:
		pylab.plot([prevpoint[0], point[0]], [prevpoint[1], point[1]], 'k-')
		prevpoint = point
		xmax = max(xmax, point[0])
		xmin = min(xmin, point[0])
		ymax = max(ymax, point[1])
		ymin = min(ymin, point[1])

if len(sys.argv) < 2 or len(sys.argv) > 3:
	sys.stderr.write("Plot polygons from testcutter\n")
	sys.stderr.write("Usage: " + sys.argv[0] + " <filename> [index]\nUse - for standard input. If no index is specified every test case is displayed\n")
	sys.exit()

if sys.argv[1] == '-':
	tests = json.loads(sys.stdin.read())
else:
	json_file = open(sys.argv[1])
	tests = json.load(json_file)
	

if len(sys.argv) == 3:
	testindexes = [int(sys.argv[2])]
else:
	testindexes = range(0, len(tests))

firsttest = True

for index in testindexes:
	if firsttest:
		firsttest = False
	else:
		pylab.figure()
	test = tests[index]

	pylab.subplot(2, 2, 1)
	pylab.xlabel("original")
	pylab.axhline(test["lineup"])
	pylab.axhline(test["linedown"])

	ymax = max(ymax, test["lineup"])
	ymin = min(ymin, test["lineup"])
	ymax = max(ymax, test["linedown"])
	ymin = min(ymin, test["linedown"])

	drawpolygon(test["orig"])
	ax = pylab.gca()

	pylab.subplot(2, 2, 2, sharex=ax, sharey=ax)
	for pol in test["up"]:
		drawpolygon(pol)
	pylab.xlabel("up")

	pylab.subplot(2, 2, 3, sharex=ax, sharey=ax)
	for pol in test["down"]:
		drawpolygon(pol)
	pylab.xlabel("down")

	pylab.subplot(2, 2, 4, sharex=ax, sharey=ax)
	for pol in test["mid"]:
		drawpolygon(pol)
	pylab.xlabel("mid")

	ax.axis([xmin - 1, xmax + 1, ymax + 1 , ymin -1])

pylab.show()
