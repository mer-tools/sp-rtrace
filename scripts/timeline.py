#!/usr/bin/env python

# Copyright (C) 2010 by Nokia Corporation
#
# Contact: Eero Tamminen <eero.tamminen@nokia.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License 
# version 2 as published by the Free Software Foundation. 
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

import sys, string, re, os, getopt, subprocess, operator

class Timestamp:
	"""
	This class provides timestamp utility functions
	"""
	def format(hours):
		"Converts timestamp to text format"
		msecs = hours % 1000		
		hours /= 1000
		seconds = hours % 60
		hours /= 60
		minutes = hours % 60
		hours /= 60  
		return "%02d:%02d:%02d.%03d" % (hours, minutes, seconds, msecs)
	
	def formatOffset(offset):
		"Converts timestamp offset to text format"
		text = Timestamp.format(offset)
		text = string.lstrip(text, ":0")
		text = string.rstrip(text, "0")
		if text[-1] == '.':
			text = string.rstrip(text, ".")
		if text == "" or text[0] == '.':
			text = "0" + text
		return text
	
	format = staticmethod(format)
	formatOffset = staticmethod(formatOffset)
# /class Timestamp


class Context:
	"""
	This class contains context allocation implementation.
	"""
	# context masks 
	MASK_NONE = 0
	MASK_ALL = 0xFFFFFFFF
	
	# the context value/mask
	value = 0
	# the context name
	name = ""

	def __init__(self, value, name):
		self.value = value
		self.name = name

	def isMaskAll(self):
		return self.value == Context.MASK_ALL
	
	def isMaskNone(self):
		return self.value == Context.MASK_NONE
# /class Context


class Event:
	"""
	The allocation/deallocation event
	"""
	class Types:
		UNDEFINED = 0
		ALLOC = 1
		FREE = 2
	# /class Types
		
	timestamp = 0
	timestampOffset = 0
	res_size = 0
	res_id = ""
	type = Types.UNDEFINED
	context = 0
	index = 0
	
	def __init__(self, type, index, context, timestamp, res_id, res_size):
		self.type = type
		self.timestamp = timestamp
		self.res_size = res_size
		self.res_id= res_id
		self.context = context
		self.index = index
		
	def matchContext(self, context):
		"Checks if the event matches the specified context mask"
		if context.isMaskNone():
			if self.context != 0:
				return False
		elif not (context.isMaskAll() or context.value & self.context):
				return False
		return True
# /class Event

class Filter:
	"""
	The base class for event filters.
	"""

	def matchesEvent(self, event):
		return False
	
# /class Filter


class TimeFilter:
	"""
	Time based event filter
	"""
	start = 0
	end = sys.maxint
	
	def __init__(self, start, end):
		if start is not None:
			self.start = start
		if end is not None:
			self.end = end

	def matchesTimestamp(self, timestamp, timestampOffset):
		absStart = self.start < 0 and timestampOffset - self.start or self.start
		absEnd = self.end < 0 and timestampOffset - self.end or self.end 
		print >> sys.stderr, "%s < %s < %s (%s)" % (Timestamp.format(absStart), Timestamp.format(timestamp), 
											Timestamp.format(absEnd), Timestamp.format(timestampOffset))
		if absStart > timestamp:
			return False
		if absEnd < timestamp:
			return False
		return True
# /class TimeFilter
 
class SizeFilter(Filter):
	"""
	Resource size based filter
	"""		
	min = 0
	max = sys.maxint
	
	def __init__(self, min, max):
		if min is not None:
			self.min = min
		if max is not None:
			self.max = max
		
	def matchesEvent(self, event):
		if event.res_size == 0:
			return True
		if self.min > event.res_size:
			return False
		if self.max < event.res_size:
			return False
		return True
# /class SizeFilter		
			
class Tic:
	"""
	This class provides helper functions for time slices used to represent
	time tics and activity slices.
	"""
	value = 0
	format = None
	text = None
	
	def __init__(self, slice, rounded):
		if rounded:
			round = 1
			value = slice
			self.value = 1
			while value != 0:
				self.value = value * round;
				round *= 10
				value /= 10
			#
		else: 
			self.value = slice
			
		decimal = 3
		slice = self.value
		while decimal and slice and slice % 10 == 0:
			decimal -= 1
			slice /= 10	
		self.format = "%%.%df" % decimal
	
	def getText(self):
		if not self.text:
			self.text = self.format % (float(self.value) / 1000)
		return self.text	
	
# /class Tic

class Plotter:
	"""
	The plotter class provides high level API for creating gnuplot configuration file.
	
	The configuration file is created when creating Plotter instance and closed
	when the plot() method is called().
	"""
	
	class Table:
		"""
		The Table class provides gnuplot table emulation with labels
		"""
		
		class Column:
			"""
			The Column class represents a single column in the table.
			"""
			
			class Cell:
				"""
				The Cell class represents a single cell in the column.
				"""
				text = None
				align = None
				
				def __init__(self, text, align = "right"):
					self.text = text
					self.align = align
		
				def write(self, file, row, col, size):
					"Writes cell contents at the specified coordinates"
					if self.align == "right":
						col += size - 1
					if self.align == "center":
						col += size / 2
					file.write("set label \"%s\" at character %d,%d %s\n" % (self.text, col, row, self.align))
				# /class Cell
			
			size = 0
			cells = None
			
			def __init__(self, size):
				self.size = size	
				self.cells = []
				
			def setCell(self, row, text, align):
				if row + 1 > len(self.cells):
					self.cells.extend(None for rows in range(row + 1 - len(self.cells)))
				self.cells[row] = self.Cell(text, align)
			
			def write(self, file, rows, col):
				"Writes column contents at the specified coordinates"
				for row in range(len(self.cells)):
					cell = self.cells[row]
					if cell is None:
						continue
					cell.write(file, rows - row, col, self.size)
				return self.size
		# /class Column
			
		rows = 0
		columns = None
		
		def __init__(self, row, col):
			self.row = row
			self.col = col
			self.columns = []
		
		def addColumn(self, size):
			self.columns.append(self.Column(size))
		
		def setText(self, row, col, text, align = "right"):
			if col > len(self.columns):
				raise NameError("Table.setText: column value(%d) exceeding column count(%d)" % (col, len(self.columns)))
			self.columns[col].setCell(row, text, align)
			if row > self.rows:
				self.rows = row
			
		def write(self, file):
			offset = self.col
			for column in self.columns:
				offset += column.write(file, self.rows + self.row, offset)
	# /class Table
		
	class DataFile:
		"""
		This class represents gnuplot data file
		"""
		
		class Cache:
			x1 = 0
			y1 = 0
			x2 = 0
			y2 = 0
			
			def __init__(self, x1, y1, x2, y2):
				self.x1 = x1
				self.y1 = y1
				self.x2 = x2
				self.y2 = y2
		# /class Cache
		
		_filename = None
		cache = None
		
		def __init__(self, filename, title = None):
			self._filename = filename
			self.cache = []
			if title is not None:
				self.create(title)
			
		def create(self, title = None):
			self.file = open(self._filename, "w")
			if self.file is None:
				raise NameError("Failed to create data file: %s" % filename)
			if title is not None:
				self.file.write("Resource \"%s\"\n" % title)
			
		def write(self, x, y):
			self.file.write("%d %d\n" % (x, y))
			
		def add(self, x1, y1, x2, y2):
			self.cache.append(self.Cache(x1, y1, x2, y2))
			
		def close(self):
			for line in self.cache:
				self.file.write("%d %d " % (line.x1, line.y1))
			self.file.write("\n")
			for line in self.cache:
				self.file.write("%d %d " % (line.x2, line.y2))
			self.file.write("\n")
				
			self.file.close()
			
		def remove(self):
			os.remove(self._filename)
			
		def getFilename(self):
			return self._filename
	# /class DataFile
		
	PNG = 1
	POSTSCRIPT = 2
	CFG_FILENAME = "timeline.cfg"
	
	file = None
	terminal = None
	timestampOffset = 0
	tables = None
	files = None
	stream = None
	graphs = None
	# the last index of line style setting
	lineStyleIndex = 0
		
	def __init__(self, stream, timestamp = 0, terminal = POSTSCRIPT):
		self.file = open(self.CFG_FILENAME, "w")
		self.tables = []
		self.files = []
		self.graphs = []
		if self.file is None:
			print >> sys.stderr, "Failed to create configuration file: %s" % self.CFG_FILENAME
			sys.exit(2)
	
		# set terminal properties
		if terminal == Plotter.PNG:
			self.file.write("set terminal png enhanced size 1024,728\n")
			self.file.write("set fontpath \"/usr/share/fonts/truetype/ttf-liberation\"\n")
		elif terminal == Plotter.POSTSCRIPT:
			self.file.write("set terminal postscript eps enhanced color\n")
		else:
			raise NamedError("Unknown terminal type: %s" % terminal)
		self.terminal = terminal
		self.timestampOffset = timestamp
		self.stream = stream
		
		# write graph legend outside graph
		self.file.write("set key bmargin\n")
		
	def setTitle(self, title):
		"Sets report title"
		self.file.write("set title \"%s\"\n" % title)
		
	def setDataStyle(self, style):	
		"Sets graph line style"
		self.file.write("set style data %s\n" % style)
		
	def setAxisX(self, label, range, format = None):
		"Sets X axis"
		# set label
		self.file.write("set xlabel \"%s\" offset 0,-4\n" % label)
		# set  range
		frange = float(range) / 1000
		self.file.write("set xrange[0.000:%.3f]\n" % (frange))
		# set the X axis tic label format, not used as tics are printed manually
		# file.write("set format x \"+%.3f\"\n")

		# place autotics outside range to avoid interference with manual tics
		self.file.write("set xtics %f,%f\n" % (frange + 1, frange + 1))
		
		# set ticks
		self.file.write("set xtics rotate\n")
		step = Tic(range / 10, True)
		fstep = float(step.value) / 1000
		tic = 0
		while tic <= range - step.value:
			self.file.write("set xtics add (\"%s\\n+%s\" %f)\n" % (Timestamp.format(self.timestampOffset + tic), Timestamp.formatOffset(tic), float(tic) / 1000))
			tic += step.value
		self.file.write("set xtics add (\"%s\\n+%s\" %f)\n" % (Timestamp.format(self.timestampOffset + range), Timestamp.formatOffset(range), frange))
			
			
	def setAxisY(self, label, range, format):
		"Sets Y axis"
		self.file.write("set yrange[0:%d]\n" % (range))
		
		self.file.write("set format y \"%s\"\n" % format)
		self.file.write("set ytics out\n")
		
		self.file.write("set ylabel \"%s\"\n" % label)
	
	
	def setAxisY2(self, label, range, format = None):
		"Sets Y2 axis"
		
		self.file.write("set y2range[0:%d]\n" % (range))
		
		self.file.write("set y2tics out\n")
		self.file.write("set ytics nomirror\n")
		
		self.file.write("set y2label \"%s\"\n" % label)

	def setBMargin(self, value):
		self.file.write("set bmargin %d\n" % value)

	def addGraph(self, file, col, row, title = "\"\"", axis = None, style = None):
		"Adds a file based graph to plot"
		if len(self.graphs):
			data = ","
		else:
			data = ""
		data += "\"%s\" using %s:%s" % (file.getFilename(), col, row)
		if style is not None:
			data += " ls %d" % style
		data += " title %s" % title
		if axis:
			data += " axes %s" % axis
		data += "\\\n"
		self.graphs.append(data)
			
	def plot(self):
		"Sets the tables and plots the data files"
		for table in self.tables:
			table.write(self.file)
			
		self.file.write("plot \\\n")
		for graph in self.graphs:
			self.file.write(graph)
		self.file.write("\n")
		
		self.file.close()
		
		# Convert with gnuplot
		gnuplot = subprocess.Popen(["gnuplot", self.CFG_FILENAME], 0, None, None, self.stream)
		gnuplot.wait()
	
	def setLineStyle(self, type, color):
		self.lineStyleIndex += 1
		self.file.write("set style line %d lt %s lc rgb \"%s\"\n" % (self.lineStyleIndex, type, color))
		return self.lineStyleIndex
	
	# TODO: uncomment	
	#	self.cleanup()
		
	def createTable(self, row, col):
		"""
		Creates virtual table object. The tables can be used to report statistics
		or whatever table formatted information is required. A gnuplot report can
		have multiple tables.
		"""
		table = self.Table(row, col)
		self.tables.append(table)
		return table
	
	def createFile(self, filename, title = None):
		file = self.DataFile(filename, title)
		self.files.append(file)
		return file
		
	def cleanup(self):
		"Deletes configuration and data files"
		os.remove(self.CFG_FILENAME)
		
		for file in self.files:
			file.remove();
			
# /class Plotter	


class Processor:
	"""
	This class stores the allocation/deallocation events for further 
	processing. It servers as base class for specific post-processing
	tasks.
	"""
	
	class Index:
		"""
		This class is used to index resources by their identifiers, which
		should increase event lookups by resource identifiers, required
		for freed resource allocation event removal.
		"""
		event = None
		refCount = 0
		
		def __init__(self, event):
			self.event = event
			self.refCount = 1
	# /class Index
			
			
	events = None
	contexts = None
	timestampOffset = 0
	
	def __init__(self):
		self.events = {}
		self.contexts = []
	
	def registerAlloc(self, index, context, timestamp, res_type, res_id, res_size):
		"Registers resource allocation"
		resource = res_type;
		if resource is None:
			resource = self.events.keys()[0]
		if resource not in self.events:
			print >> sys.stderr, "Unknown resource: %s" % resource
			sys.exit(2)
		self.events[resource].append(Event(Event.Types.ALLOC, index, context, timestamp, res_id, res_size))
		
	def registerFree(self, index, context, timestamp, res_type, res_id):
		"Registers resource deallocation"
		resource = res_type;
		if resource is None:
			resource = self.events.keys()[0]
		if resource not in self.events:
			print >> sys.stderr, "Unknown resource: %s" % resource
			sys.exit(2)
		self.events[resource].append(Event(Event.Types.FREE, index, context, timestamp, res_id, 0))
		
	def registerContext(self, value, name):
		"Registers context declaration"
		self.contexts.append(Context(value, name))
		
	def writeReport(self, plotter):
		"Writes the end report"
		
	def registerResource(self, resource):
		"Registers tracked resource"
		self.events[resource] = []

	def setTimestampOffset(self, offset):
		"Sets the initial timestamp value from where the offsets are calculated"
		self.timestampOffset = offset	
		
	def sort(self):
		for resource in self.events.keys():
			self.events[resource] = sorted(self.events[resource], key = operator.attrgetter("index"))
			self.events[resource] = sorted(self.events[resource], key = operator.attrgetter("timestamp"))
			
	def isEventActive(self, event):
		for filter in Options.filters:
			if not filter.matchesEvent(event):
				return False
		return True
				
# /class Processor		
		

class LifetimeProcessor(Processor):
	"""
	The LifetimeProcessor class generates resource life time
	report. This report contains lines, illustrating resource
	allocation and release times.
	"""
	class Pager:
		"""
		This is a helper class to split huge plot data into multiple files.
		"""
		RECORD_LIMIT = 1000
		
		plotter = None
		resource = None
		resourceIndex = 1
		pageIndex = 1
		file = None
				
		def __init__(self, plotter, resource):
			self.plotter = plotter
			self.resource = resource
			self.createFile()
			
		def addLifeline(self, startTimestamp, endTimestamp, size, style):
			"Adds resource lifeline to the report graph"
			self.file.add(startTimestamp, size, endTimestamp, size)
			self.plotter.addGraph(self.file, "($%d/1000)" % (self.resourceIndex * 2 - 1), "%d" % (self.resourceIndex * 2), "\"\"", None, style)
			self.resourceIndex += 1
			if self.resourceIndex > self.RECORD_LIMIT:
				self.file.close()
				self.pageIndex += 1
				self.resourceIndex = 1
				self.createFile()
				#print >> sys.stderr, "page index: %d" % self.pageIndex
			
		def createFile(self):
			self.file = self.plotter.createFile("%s-%d" % (self.resource, self.pageIndex))
			self.file.create()
			
		def finish(self):
			self.file.close();
			
		def getTotal(self):
			return (self.pageIndex - 1) * self.RECORD_LIMIT + self.resourceIndex
		
	DETAILS_LIMIT = 20
	PAGE_LIMIT = 150
			
	
	def writeReport(self, plotter):
		"Writes the end report"
		xrange = 0
		yrange = 0
		
		lsMain = plotter.setLineStyle(1, "blue")
		totalGraphs = 0

		for resource in self.events.keys():
			# initialize resource statistics data
			events = self.events[resource]
			if events[-1].timestamp > xrange:
				xrange = events[-1].timestamp
		
		# iterate through registered resources
		for resource in self.events.keys():
			# initialize resource statistics data
			events = self.events[resource]
			if events[-1].timestamp > xrange:
				xrange = events[-1].timestamp
			# resource indexing map 
			index = {}

			pager = self.Pager(plotter, resource)			
			
			for event in events:
				if not self.isEventActive(event):
					continue
				if event.type == Event.Types.ALLOC:
					if event.res_id in index:
						index[event.res_id].refCount += 1
						continue
					else:
						# store the event in dictionary for event lookup speedup.
						index[event.res_id] = self.Index(event);
						
				if event.type == Event.Types.FREE:
					if event.res_id not in index:
						continue
					ievent = index[event.res_id]
					ievent.refCount -= 1
					if ievent.refCount > 0:
						continue
					# TODO: plot the timeline
					size = ievent.event.res_size
					if size > yrange:
						yrange = size
					pager.addLifeline(ievent.event.timestamp, event.timestamp, size, lsMain)

					if pager.pageIndex > self.PAGE_LIMIT:
						print >> sys.stderr, "Warning, number of resources exceeding limits, processing could take a lot of time."
					
					del index[event.res_id]

			# plot all unallocated resources					
			for res_id, ievent in index.iteritems():
				pager.addLifeline(ievent.event.timestamp, xrange, ievent.event.res_size, lsMain)	
			
			totalGraphs += pager.getTotal()
			pager.finish();
			
		plotter.setTitle("Resource life-time")
		plotter.setAxisX("time (secs)", xrange)
		plotter.setAxisY("size", yrange, "%.1s%c")
			
		if totalGraphs > self.DETAILS_LIMIT:
			plotter.setDataStyle("lines")
		else:
			plotter.setDataStyle("linespoints")
			
		plotter.plot()
	

class ActivityProcessor(Processor):
	"""
	The ActivityProcessor class generates 'activity' report.
	The activity report contains graphs illustrating the resource
	allocation rate per time slice.
	"""

	class Stats:
		"""
		Resource allocation activity statistics 
		"""
		class Data:
			count = 0
			size = 0
			timestamp = 0
			file = None
		# /class Data

		peakSize = None
		peakAllocs = None
		peakFrees = None
		
		def __init__(self):
			self.peakSize = self.Data()
			self.peakAllocs = self.Data()
			self.peakFrees = self.Data()
	# /class Stats

	def writeReport(self, plotter):
		"Writes the end report"
		xrange = 0
		yrange = 0
		y2range= 0
		# peak statistics list
		stats = {}
		
		# calculate the X axis range and activity time slice
		for resource in self.events.keys():
			events = self.events[resource]
			if events[-1].timestamp > xrange:
				xrange = events[-1].timestamp
		
		
		if Options.slice == 0:
			slice = Tic(xrange / 100, True)
		else:
			slice = Tic(Options.slice, False)
		
		contexts = [Context(Context.MASK_ALL, "all allocations")]
		if len(self.contexts) > 0:
			contexts.append(Context(Context.MASK_NONE, "no contexts"))
			contexts.extend(self.contexts)

		# iterate through registered resources
		for resource in self.events.keys():
			stat = self.Stats()
			stats[resource] = stat

			# initialize peak statistics data files
			stat.peakSize.file = plotter.createFile("%s-size-peak.dat" % resource)
			stat.peakAllocs.file = plotter.createFile("%s-allocs-peak.dat" % resource)
			stat.peakFrees.file = plotter.createFile("%s-frees-peak.dat" % resource)

			# initialize resource statistics data
			events = self.events[resource]
			
			lastTimestamp = events[-1].timestamp
			step = slice.value / 2
			if step == 0:
				step = 0.001
			
			# iterate through available contexts
			for context in contexts:
				# create the data files for the resource-context filter
				fileRate = plotter.createFile("%s-size-%x.dat" % (resource, context.value), "%s (rate:%s)" % (resource, context.name))
				fileAllocs = plotter.createFile("%s-allocs-%x.dat" % (resource, context.value), "%s (allocs:%s)" % (resource, context.name))
				fileFrees = plotter.createFile("%s-frees-%x.data" % (resource, context.value), "%s (frees:%s)" % (resource, context.name))
				 
				# resource indexing map 
				index = {}
				# total allocation size per time slice
				total = 0
				# allocation number for time slice
				allocs = 0
				# deallocation number for time slice
				frees = 0
				# allocation events per time slice
				sliceEvents = []
				# flag indicating if the filter has any data
				hasData = False
				 
				timestamp = 0
				it = iter(events)
				event = it.next()
				while True:
					try:
						while event.timestamp <= timestamp:
							if event.type == Event.Types.ALLOC:
								if event.res_id in index:
									index[event.res_id].refCount += 1
								else:
									# store the event in dictionary for event lookup speedup.
									index[event.res_id] = self.Index(event);
									#
									total += event.res_size
									allocs += 1
									sliceEvents.append(event)
									
							if event.type == Event.Types.FREE:
								if event.res_id in index:
									ievent = index[event.res_id]
									ievent.refCount -= 1
									if ievent.refCount <= 0:
										del index[event.res_id]
									frees +=1
									sliceEvents.append(event)
									
							event = it.next()
						
					except StopIteration:
						pass

					# remove allocation events outside time slice
					while len(sliceEvents) > 0 and sliceEvents[0].timestamp < event.timestamp - slice.value:
						oldEvent = sliceEvents.pop(0)
						if oldEvent.type == Event.Types.ALLOC:
							total -= oldEvent.res_size
							allocs -= 1
						if oldEvent.type == Event.Types.FREE:
							frees -= 1
						
					# store peak values
					if context.isMaskAll():
						if total > stat.peakSize.size:
							stat.peakSize.size = total
							stat.peakSize.count = allocs
							stat.peakSize.timestamp = event.timestamp
						if allocs > stat.peakAllocs.count:
							stat.peakAllocs.size = total
							stat.peakAllocs.count = allocs
							stat.peakAllocs.timestamp = event.timestamp
						if frees > stat.peakFrees.count:
							stat.peakFrees.count = frees
							stat.peakFrees.size = total
							stat.peakFrees.timestamp = event.timestamp
					#
						
					fileRate.write(event.timestamp, total)
					fileAllocs.write(event.timestamp, allocs)
					fileFrees.write(event.timestamp, frees)
					
					hasData = True
					if total > yrange:
						yrange = total
					if allocs > y2range:
						y2range = allocs
					if frees > y2range:
						y2range = frees

					if timestamp == lastTimestamp:
						break
					timestamp += step
					# calculate the activity of the last timestamp even if it did 
					# not match slice point.
					if timestamp > lastTimestamp:
						timestamp = lastTimestamp

				fileRate.close()
				fileAllocs.close()
				fileFrees.close()
				
				if hasData:
					plotter.addGraph(fileRate, "($1/1000)", "2", "column(2)")
					plotter.addGraph(fileAllocs, "($1/1000)", "2", "column(2)", "x1y2")
					plotter.addGraph(fileFrees, "($1/1000)", "2", "column(2)", "x1y2")
				 
			# peak marker data.
			# Peak markers are vertical lines, drawn at the time of the max peak
			plotter.addGraph(stat.peakSize.file, "($1/1000)", "2", "column(2)")
			plotter.addGraph(stat.peakAllocs.file, "($1/1000)", "2", "column(2)")
			plotter.addGraph(stat.peakFrees.file, "($1/1000)", "2", "column(2)")
			
		# iterate through registered resources to generate peak allocation data files
		for resource in self.events.keys():
			timestamp = stats[resource].peakSize.timestamp
			file = stats[resource].peakSize.file
			file.create("%s (peak rate:%s)" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write(timestamp, 0)
			file.write(timestamp, yrange)
			file.close()
			
			timestamp = stats[resource].peakAllocs.timestamp
			file = stats[resource].peakAllocs.file
			file.create("%s (peak allocs:%s)" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write(timestamp, 0)
			file.write(timestamp, yrange)
			file.close()
			
			timestamp = stats[resource].peakFrees.timestamp
			file = stats[resource].peakFrees.file
			file.create("%s (peak frees:%s)" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write(timestamp, 0)
			file.write(timestamp, yrange)
			file.close()

		#	
		# generate gnuplot configuration file:
		#
		plotter.setTitle("Allocation/deallocation rate")
		
		plotter.setAxisX("time (secs)", xrange)
		plotter.setAxisY("amount per %s sec" % slice.getText(), yrange, "%.1s%c")
		plotter.setAxisY2("count per %s sec" % slice.getText(), y2range)
			
		plotter.setDataStyle("lines")
		
		# Write summary report
		
		# define table headers
		table = plotter.createTable(1, 1)
		# resource name column
		table.addColumn(10)
		# snapshot name column
		table.addColumn(10)
		# allocation count
		table.addColumn(8)
		# allocation size
		table.addColumn(10)

#		table.setText(0, 0, "{/%s}Resource" % Options.fonts.BOLDITALIC, "center")
#		table.setText(0, 1, "{/%s}State" % Options.fonts.BOLDITALIC, "center")
#
#		table.setText(0, 2, "{/%s}Count" % Options.fonts.BOLDITALIC, "center")
#		table.setText(0, 3, "{/%s}Size" % Options.fonts.BOLDITALIC, "center")

		table.setText(0, 0, "Resource", "center")
		table.setText(0, 1, "State", "center")

		table.setText(0, 2, "Count", "center")
		table.setText(0, 3, "Size", "center")


		# write summary data
		resourceIndex = 2
		for resource in self.events.keys():
			stat = stats[resource]
			table.setText(resourceIndex, 0, resource, "left")
			table.setText(resourceIndex, 1, "peak size", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakSize.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakSize.size)

			resourceIndex += 1
			table.setText(resourceIndex, 1, "peak allocs", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakAllocs.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakAllocs.size)
			
			resourceIndex += 1
			table.setText(resourceIndex, 1, "peak frees", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakFrees.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakFrees.size)
			
			resourceIndex += 2
		
		# Reserve space at the bottom for leak data
		bmargin = len(contexts) + 9
		if bmargin < 9 + table.rows:
			bmargin = 9 + table.rows
		if bmargin < 15:
			bmargin = 15
		plotter.setBMargin(bmargin)
		
		# plot the data files
		plotter.plot()
		
# /class ActivityProcessor


class TotalsProcessor(Processor):
	"""
	The TotalsProcessor class generates 'totals' report.
	The 'totals' report contains graph illustrating the total resource 
	allocation over the time.
	"""
	
	class Stats:
		"""
		Resource allocation statistics 
		"""
		class Data:
			count = 0
			size = 0
		# /class Data
		
		endLeaks = None
		peakLeaks = None
		endTotals = None
		peakTotals = None
		peakTimestamp = 0
		peakFile = None

		def __init__(self):
			# non-freed allocations at the end of trace
			self.endLeaks = self.Data()
			# non-freed allocations at the peak time
			self.peakLeaks = self.Data()
			# total allocations at the end of trace
			self.endTotals = self.Data()
			# total allocations at the peak time
			self.peakTotals = self.Data()
	# /class Stats
		
	def writeReport(self, plotter):
		"Writes the end report"
		xrange = 0
		yrange = 0
		# leak summing map
		stats = {}
		# data file list
		files = []

		contexts = [Context(Context.MASK_ALL, "all allocations")]
		if len(self.contexts) > 0:
			contexts.append(Context(Context.MASK_NONE, "no contexts"))
			contexts.extend(self.contexts)
		
		# iterate through registered resources
		for resource in self.events.keys():
			stat = self.Stats()
			stats[resource] = stat
			#
			events = self.events[resource]
			if events[-1].timestamp > xrange:
				xrange = events[-1].timestamp

			# initialize peak statistics data files
			stat.peakFile = plotter.createFile("%s-peak.dat" % resource)

			# iterate through available contexts
			for context in contexts:
				# create the data file for the resource-context filter
				file = plotter.createFile("%s-%x.dat" % (resource, context.value), "%s (%s)" % (resource, context.name))
					
				# resource indexing map 
				index = {}
				# total allocation size
				total = 0
				# flag indicating if the filter has any data
				hasData = False
					
				# iterate through allocation/deallocation events
				for event in events:
					if not event.matchContext(context):
						continue
					
					if event.type == Event.Types.ALLOC:
						if event.res_id in index:
							index[event.res_id].refCount += 1
							continue
						else:
							# store the event in dictionary for event lookup speedup.
							index[event.res_id] = self.Index(event);
							#
							total += event.res_size
							if context.isMaskAll():
								stat.endLeaks.size += event.res_size
								stat.endLeaks.count += 1
								stat.endTotals.size += event.res_size
								stat.endTotals.count += 1
								if stat.endLeaks.size > stat.peakLeaks.size:
									stat.peakLeaks.size = stat.endLeaks.size
									stat.peakLeaks.count = stat.endLeaks.count
									stat.peakTotals.size = stat.endTotals.size
									stat.peakTotals.count = stat.endTotals.count
									stat.peakTimestamp = event.timestamp
							
					if event.type == Event.Types.FREE:
						if event.res_id not in index:
							continue
						ievent = index[event.res_id]
						ievent.refCount -= 1
						if ievent.refCount > 0:
							continue
						total -= ievent.event.res_size
						if context.isMaskAll():
							stat.endLeaks.size -= ievent.event.res_size
							stat.endLeaks.count -= 1
						del index[event.res_id]
						
					file.write(event.timestamp, total)
					hasData = True
					if total > yrange:
						yrange = total
				file.close()
				if hasData:
					plotter.addGraph(file, "($1/1000)", "2", "column(2)")
			# peak marker data.
			# Peak markers are vertical lines, drawn at the time of the max peak
			plotter.addGraph(stat.peakFile, "($1/1000)", "2", "column(2)")

		# iterate through registered resources to generate peak allocation data files
		for resource in self.events.keys():
			file = stats[resource].peakFile
			timestamp = stats[resource].peakTimestamp
			file.create("%s (peak:%s)\n" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write(timestamp, 0)
			file.write(timestamp, yrange)
			file.close()

		#	
		# generate gnuplot configuration file:
		#
		plotter.setTitle("Amount of non-freed allocations")
		
		plotter.setAxisX("time (secs)", xrange)
		plotter.setAxisY("size", yrange, "%.1s%c")
			
		plotter.setDataStyle("lines")
		
		# Write summary report
		
		# define table headers
		table = plotter.createTable(1, 1)
		# resource name column
		table.addColumn(10)
		# snapshot name column
		table.addColumn(5)
		# total allocation count
		table.addColumn(8)
		# total allocation size
		table.addColumn(10)
		# leaked allocation count
		table.addColumn(8)
		# leaked allocation size
		table.addColumn(10)

#		table.setText(1, 0, "{/%s}Resource" % Options.fonts.BOLDITALIC, "center")
#		table.setText(1, 1, "{/%s}State" % Options.fonts.BOLDITALIC, "center")
#
#		table.setText(0, 2, "{/%s}Total" % Options.fonts.BOLDITALIC, "center")
#		table.setText(0, 3, "{/%s}Total" % Options.fonts.BOLDITALIC, "center")
#		table.setText(1, 2, "{/%s}count" % Options.fonts.BOLDITALIC, "center")
#		table.setText(1, 3, "{/%s}size" % Options.fonts.BOLDITALIC, "center")
#
#		table.setText(0, 4, "{/%s}Non-freed" % Options.fonts.BOLDITALIC, "center")
#		table.setText(0, 5, "{/%s}Non-freed" % Options.fonts.BOLDITALIC, "center")
#		table.setText(1, 4, "{/%s}count" % Options.fonts.BOLDITALIC, "center")
#		table.setText(1, 5, "{/%s}size" % Options.fonts.BOLDITALIC, "center")

		table.setText(1, 0, "Resource", "center")
		table.setText(1, 1, "State", "center")

		table.setText(0, 2, "Total", "center")
		table.setText(0, 3, "Total", "center")
		table.setText(1, 2, "count", "center")
		table.setText(1, 3, "size", "center")

		table.setText(0, 4, "Non-freed", "center")
		table.setText(0, 5, "Non-freed", "center")
		table.setText(1, 4, "count", "center")
		table.setText(1, 5, "size", "center")
		
		# write summary data
		resourceIndex = 3
		for resource in self.events.keys():
			stat = stats[resource]
			table.setText(resourceIndex, 0, resource, "left")
			table.setText(resourceIndex, 1, "peak", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakTotals.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakTotals.size)
			table.setText(resourceIndex, 4, "%d" % stat.peakLeaks.count)
			table.setText(resourceIndex, 5, "%d" % stat.peakLeaks.size)

			resourceIndex += 1
			table.setText(resourceIndex, 1, "end", "center")
			table.setText(resourceIndex, 2, "%d" % stat.endTotals.count)
			table.setText(resourceIndex, 3, "%d" % stat.endTotals.size)
			table.setText(resourceIndex, 4, "%d" % stat.endLeaks.count)
			table.setText(resourceIndex, 5, "%d" % stat.endLeaks.size)
			
			resourceIndex += 2
		
		# Reserve space at the bottom for leak data
		bmargin = len(contexts) + 9
		if bmargin < 9 + table.rows:
			bmargin = 9 + table.rows
		if bmargin < 15:
			bmargin = 15
		plotter.setBMargin(bmargin)
		
		# plot the data files
		plotter.plot()
# /class TotalsProcessor
		
	
class Parser:
	"""
	This class parses the sp-rtrace format log file and calls the assigned processor
	to produce the required report file.
	"""
	processor = None
	reAlloc = re.compile("^([0-9]+)\.(?: @([0-9a-fA-F]+)|) [^[]*\[([^\]]+)\][^(<]+(?:<([^>]+)>|)\(([^)]+)\) = (0x[a-fA-F0-9]+)(.*)$")
	reFree = re.compile("^([0-9]+)\.(?: @([0-9a-fA-F]+)|) [^[]*\[([^\]]+)\][^(<]+(?:<([^>]+)>|)\((0x[a-fA-F0-9]+)\)$")
	reResource = re.compile("\<([0-9a-z]+)\> : ([^ ]+) \(([^\)]+)\)")
	reTimestamp = re.compile("^([0-9]+)\:([0-9]+)\:([0-9]+)\.([0-9]+)$")
	reContext = re.compile("^\@ ([0-9a-fA-F]+) : (.*)$")
	timestampOffset = None
	
	def __init__(self, processor):
		self.processor = processor
		
	def read(self, stream):
		for line in stream:
			# don't attempt to parse backtrace records
			if line[0] == '\t':
				continue
			#
			match = self.reAlloc.match(line)
			if match:
				context = match.group(2)
				if context is None:
					context = 0
				timestamp = self.getTimestampFromString(match.group(3))
				if not Options.timeFilter.matchesTimestamp(timestamp, self.timestampOffset or timestamp):
					continue
				if self.timestampOffset is None:
					self.timestampOffset = timestamp
					self.processor.setTimestampOffset(self.timestampOffset)
				timestamp -= self.timestampOffset
				self.processor.registerAlloc(int(match.group(1)), context, timestamp, match.group(4), match.group(6), int(match.group(5)))
				continue
			match = self.reFree.match(line)
			if match:
				context = match.group(2)
				if context is None:
					context = 0
				timestamp = self.getTimestampFromString(match.group(3))
				if not Options.timeFilter.matchesTimestamp(timestamp, self.timestampOffset or timestamp):
					continue
				if self.timestampOffset is None:
					self.timestampOffset = timestamp
					self.processor.setTimestampOffset(self.timestampOffset)
				timestamp -= self.timestampOffset
				self.processor.registerFree(int(match.group(1)), context, self.getTimestampFromString(match.group(3)), match.group(4), match.group(5))
				continue
			match = self.reResource.match(line)
			if match:
				self.processor.registerResource(match.group(2))
				continue
			match = self.reContext.match(line)
			if match:
				self.processor.registerContext(int(match.group(1), 16), match.group(2))
				continue
			
		if self.timestampOffset is None:
			print >> sys.stderr, "The specified time period does not contain any events"
			sys.exit(0)
				
	def write(self, stream):
		self.processor.sort()
		plotter = Plotter(stream, self.processor.timestampOffset, Options.isPng and Plotter.PNG or Plotter.POSTSCRIPT)
		self.processor.writeReport(plotter)
		
		
	def getTimestampFromString(self, text):
		timestamp = 0
		match = self.reTimestamp.match(text)
		if match:
			timestamp = int(match.group(1)) * 3600000 + int(match.group(2)) * 60000 + int(match.group(3)) * 1000 + int(match.group(4))
		return timestamp
# /class Parser	
	

class Options:
	"""
	This class contains options parser implementation.
	"""
	class Modes: 
		NONE = 0
		TOTALS = 1
		LIFETIME = 2
		ACTIVITY = 3
	# /class Modes

	class Fonts:
		NORMAL = "LiberationSans"
		ITALIC = "LiberationSans-Italic"
		BOLDITALIC = "LiberationSans-BoldItalic"
	# /class Fonts

	GDFONTPATH = "/usr/share/fonts/truetype/ttf-liberation"
	
	mode = Modes.NONE
	streamIn = sys.stdin
	streamOut = sys.stdout
	slice = 0
	
	isPng = False
	fonts = Fonts()
	
	filters = []
	timeFilter = None
	
	def parse(argv):
		try:
			opts, args = getopt.gnu_getopt(argv, "tlao:i:s:p", 
										["totals", 
										 "lifetime",
										 "activity", 
										 "in=",
										 "out=",
										 "slice=",
										 "png",
										 "filter-size=",
										 "filter-time="])
		except getopt.GetoptError, err:
			print >> sys.stderr, str(err) 
			Options.displayUsage()
			sys.exit(2)
		
		parser = None;
		
		for opt, val in opts:
			if opt == "-t" or opt == "--totals":
				Options.mode = Options.Modes.TOTALS
				parser = Parser(TotalsProcessor())
				
			if opt == "-l" or opt == "--lifetime":
				Options.mode = Options.Modes.LIFETIME
				parser = Parser(LifetimeProcessor())
				
			if opt == "-a" or opt == "--activity":
				Options.mode = Options.Modes.ACTIVITY
				parser = Parser(ActivityProcessor())
				
			if opt == "-i" or opt == "--in":
				Options.streamIn = open(val, "r")
				
			if opt == "-o" or opt == "--out":
				Options.streamOut = open(val, "w")
			
			if opt == "-s" or opt == "--slice":
				Options.slice = int(val)
				
			if opt == "-p" or opt == "--png":
				Options.isPng =  True
				Options.fonts.NORMAL = "LiberationSans-Regular"
			
			if opt == "--filter-size":
				match = re.match("([0-9kmKM]*)-([0-9kmKM]*)", val)
				if match is None:
					print >> sys.stderr, "Invalid size filter value: %s" % val
					Options.displayUsage()
					sys.exit(2)
				min = Options.parseSize(match.group(1))
				max = Options.parseSize(match.group(2))
				Options.filters.append(SizeFilter(min, max))
				
			if opt == "--filter-time":
				match = re.match("([0-9:.]*)-([0-9:.]*)", val)
				if match is None:
					print >> sys.stderr, "Invalid timestamp filter value: %s" % val
					Options.displayUsage()
					sys.exit(2)
				start = Options.parseTime(match.group(1))
				end = Options.parseTime(match.group(2))
				Options.timeFilter = TimeFilter(start, end)
				
		if parser is None:
			print >> sys.stderr, "No report type specified."
			Options.displayUsage()
			sys.exit(2)
			
		return parser
			
	def parseSize(text):
		if text == "":
			return None
		match = re.match("([0-9]+)([kmKM]?)", text)
		if match is None:
			return None
		mod = 1
		if match.group(2).upper() == "K":
			mod = 1024
		if match.group(2).upper() == "M":
			mod = 1024 * 1024
		return int(match.group(1)) * mod
	
	def parseTime(text):
		match = re.match("([0-9]+):([0-9]+):([0-9]+)(?:\.([0-9]+)|)", text)
		if match is None:
			return -int(text)
		timestamp = int(match.group(1)) * 60 * 60 * 1000
		timestamp += int(match.group(2)) * 60 * 1000
		timestamp += int(match.group(3)) * 1000
		if match.group(4) is not None:
			timestamp += int(match.group(4))
		return timestamp
	
	def displayUsage():
		print \
"""
Usage:
  rtrace-timeline <options>
  Where <options> are:
    -t         generate report of total resource allocations and
               non-free resources.
    -l         generate report of resource life times.
    -a         generate report of resource allocation/deallocation
               activity.
    -s <msec>  the time slice for acivity report.
    -p         generate png file (eps file is generated by default)."
    -i <file>  input file.
    -o <file>  outtput file.
"""

		
	# static method definitions
	parse = staticmethod(parse)
	displayUsage = staticmethod(displayUsage)
	parseSize = staticmethod(parseSize)
	parseTime = staticmethod(parseTime)
# /class Options
	

#
# main()
#
parser = Options.parse(sys.argv[1:])
parser.read(Options.streamIn)
parser.write(Options.streamOut)


