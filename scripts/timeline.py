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

import sys, string, re, os, getopt, subprocess

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
		
	format = staticmethod(format)
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
	res_size = 0
	res_id = ""
	type = Types.UNDEFINED
	context = 0
	
	def __init__(self, type, context, timestamp, res_id, res_size):
		self.type = type
		self.timestamp = timestamp
		self.res_size = res_size
		self.res_id= res_id
		self.context = context
		
	def matchContext(self, context):
		"Checks if the event matches the specified context mask"
		if context.isMaskNone():
			if self.context != 0:
				return False
		elif not (context.isMaskAll() or context.value & self.context):
				return False
		return True
# /class Event

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
	columns = []
	
	def __init__(self, row, col):
		self.row = row
		self.col = col
	
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
			
	events = {}
	contexts = []
	timestampOffset = 0
	
	def registerAlloc(self, context, timestamp, res_type, res_id, res_size):
		"Registers resource allocation"
		resource = res_type;
		if resource is None:
			resource = self.events.keys()[0]
		if resource not in self.events:
			print >> sys.stderr, "Unknown resource: %s" % resource
			sys.exit(2)
		self.events[resource].append(Event(Event.Types.ALLOC, context, timestamp, res_id, res_size))
		
	def registerFree(self, context, timestamp, res_type, res_id):
		"Registers resource deallocation"
		resource = res_type;
		if resource is None:
			resource = self.events.keys()[0]
		if resource not in self.events:
			print >> sys.stderr, "Unknown resource: %s" % resource
			sys.exit(2)
		self.events[resource].append(Event(Event.Types.FREE, context, timestamp, res_id, 0))
		
	def registerContext(self, value, name):
		"Registers context declaration"
		self.contexts.append(Context(value, name))
		
	def writeReport(self, stream):
		"Writes the end report"
		
	def registerResource(self, resource):
		"Registers tracked resource"
		self.events[resource] = []

	def setTimestampOffset(self, offset):
		"Sets the initial timestamp value from where the offsets are calculated"
		self.timestampOffset = offset
# /class Processor		
		

class ActivityProcessor(Processor):
	"""
	The ActivityProcessor class generates 'activity' report.
	The activity report contains graphs illustrating the resource
	allocation rate per time slice.
	"""
	GNUPLOT_CONFIG = "timeline.cfg"
	
	class Stats:
		"""
		Resource allocation activity statistics 
		"""
		class Data:
			count = 0
			size = 0
			timestamp = 0
		# /class Data

		peakSize = None
		peakCount = None
		
		def __init__(self):
			self.peakSize = self.Data()
			self.peakCount = self.Data()
	# /class Stats

	def writeReport(self, stream):
		"Writes the end report"
		xrange = 0
		yrange = 0
		y2range= 0
		plotData = ""
		stats = {}
		
		# calculate the X axis range and activity time slice
		for resource in self.events.keys():
			events = self.events[resource]
			if events[-1].timestamp > xrange:
				xrange = events[-1].timestamp
		slice = Options.slice
		if slice == 0:
			slice = xrange / 20
			if slice == 0:
				slice = 1
		#
		
		contexts = [Context(Context.MASK_ALL, "all allocations")]
		if len(self.contexts) > 0:
			contexts.append(Context(Context.MASK_NONE, "no contexts"))
			contexts.extend(self.contexts)
				
		# iterate through registered resources
		for resource in self.events.keys():
			# iterate through available contexts
			for context in contexts:
				# create the data files for the resource-context filter
				fileNameRate = "%s-size-%x.dat" % (resource, context.value)
				fileRate = open(fileNameRate, "w")
				if fileRate is None:
					print >> sys.stderr, "Failed to create data file: %s" % fileNameRate
					sys.exit(2)
				fileRate.write("Resource \"%s (%s)\"\n" % (resource, context.name))
				
				fileNameCount = "%s-count-%x.dat" % (resource, context.value)
				fileCount = open(fileNameCount, "w")
				if fileCount is None:
					print >> sys.stderr, "Failed to create data file: %s" % fileNameCount
					sys.exit(2)
				fileCount.write("Resource \"%s (%s)\"\n" % (resource, context.name))
				
				# resource indexing map 
				index = {}
				# total allocation size per time slice
				total = 0
				# allocation number for time slice
				count = 0
				# allocation events per time slice
				allocs = []
				# flag indicating if the filter has any data
				hasData = False
				# initialize resource statistics data
				if context.isMaskAll():
					stat = self.Stats()
					stats[resource] = stat
				#
				events = self.events[resource]
				
				# iterate through allocation events
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
							count += 1
							allocs.append(event)
							# remove allocation events outside time slice
							while allocs[0].timestamp < event.timestamp - slice:
								oldEvent = allocs.pop(0)
								total -= oldEvent.res_size
								count -= 1
								
							# store peak values
							if context.isMaskAll():
								if total > stat.peakSize.size:
									stat.peakSize.size = total
									stat.peakSize.count = count
									stat.peakSize.timestamp = event.timestamp
								if count > stat.peakCount.count:
									stat.peakCount.size = total
									stat.peakCount.count = count
									stat.peakCount.timestamp = event.timestamp
								#
							
					if event.type == Event.Types.FREE:
						if event.res_id not in index:
							continue
						ievent = index[event.res_id]
						ievent.refCount -= 1
						if ievent.refCount > 0:
							continue
						del index[event.res_id]
						
					fileRate.write("%d %d\n" % (event.timestamp, total))
					fileCount.write("%d %d\n" % (event.timestamp, count))
					hasData = True
					if total > yrange:
						yrange = total
					if count > y2range:
						y2range = count
						
				fileRate.close()
				fileCount.close()
				if hasData:
					if plotData != "":
						plotData += ", "
					plotData += "\"%s\" using ($1/1000):2 title column(2)" % fileNameRate
					plotData += ", \"%s\" using ($1/1000):2 title column(2) axes x1y2" % fileNameCount
			# peak marker data.
			# Peak markers are vertical lines, drawn at the time of the max peak
			plotData += ", \"%s-size-peak.dat\" using ($1/1000):2 title column(2)" % resource
			plotData += ", \"%s-count-peak.dat\" using ($1/1000):2 title column(2)" % resource
			
		# iterate through registered resources to generate peak allocation data files
		for resource in self.events.keys():
			fileName = resource + "-size-peak.dat"
			file = open(fileName, "w")
			if file is None:
				print >> sys.stderr, "Failed to create data file: %s" % fileName
				sys.exit(2)
			timestamp = stats[resource].peakSize.timestamp
			file.write("Resource \"%s (size peak:%s)\"\n" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write("%d %d\n" % (timestamp, 0))
			file.write("%d %d\n" % (timestamp, yrange))
			file.close()
			
			fileName = resource + "-count-peak.dat"
			file = open(fileName, "w")
			if file is None:
				print >> sys.stderr, "Failed to create data file: %s" % fileName
				sys.exit(2)
			timestamp = stats[resource].peakCount.timestamp
			file.write("Resource \"%s (count peak:%s)\"\n" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write("%d %d\n" % (timestamp, 0))
			file.write("%d %d\n" % (timestamp, yrange))
			file.close()
			
		#	
		# generate gnuplot configuration file:
		#
		file = open(self.GNUPLOT_CONFIG, "w")
		if file is None:
			print >> sys.stderr, "Failed to create configuration file: %s" % self.GNUPLOT_CONFIG
			sys.exit(2)
		
		# set terminal properties
		file.write("set terminal postscript eps enhanced color\n")
		# set file title
		file.write("set title \"Amount of {/Helvetica-Italic}non-freed {/Helvetica} allocations\"\n")
		# set X axis range 
		file.write("set xrange[0.000:%.3f]\n" % (float(xrange) / 1000))
		# set Y/Y2 axis range 
		file.write("set yrange[0:%d]\n" % (yrange))
		file.write("set y2range[0:%d]\n" % (y2range))
		# format X axis tics
		xtic = float(int(xrange / 10)) / 1000
		if xtic == 0:
			xtic = 0.001
		file.write("set xtics %f,%f,%f\n" % (xtic, xtic, float(xrange) / 1000 - xtic))
		file.write("set xtics rotate\n")
		file.write("set xtics add (\"%s\" %f)\n" % (Timestamp.format(self.timestampOffset), 0))
		file.write("set xtics add (\"%s\" %f)\n" % (Timestamp.format(self.timestampOffset + xrange), float(xrange) / 1000))
		# set the X axis tic label format
		file.write("set format x \"+%.3f\"\n")
		# set the Y/Y2 axis tic label format
		file.write("set format y \"%.1s%c\"\n")
		file.write("set ytics out\n")
		file.write("set y2tics out\n")
		file.write("set ytics nomirror\n")
		# set Y/Y2 axis label
		file.write("set ylabel \"size per %.3f sec\"\n" % (float(slice) / 1000))
		file.write("set y2label \"count per %.3f sec\"\n" % (float(slice) / 1000))
		# set X axis label
		file.write("set xlabel \"time (secs)\" offset 0,-3\n")
		# set graph line style
		file.write("set style data lines\n")
		# write graph information outside graph
		file.write("set key bmargin\n")
		
		# Write summary report
		
		# define table headers
		table = Table(1, 1)
		# resource name column
		table.addColumn(10)
		# snapshot name column
		table.addColumn(10)
		# allocation count
		table.addColumn(8)
		# allocation size
		table.addColumn(10)

		table.setText(0, 0, "{/Helvetica-Italic-Bold}Resource", "center")
		table.setText(0, 1, "{/Helvetica-Italic-Bold}State", "center")

		table.setText(0, 2, "{/Helvetica-Italic-Bold}Count", "center")
		table.setText(0, 3, "{/Helvetica-Italic-Bold}Size", "center")

		# write summary data
		resourceIndex = 2
		for resource in self.events.keys():
			stat = stats[resource]
			table.setText(resourceIndex, 0, resource, "left")
			table.setText(resourceIndex, 1, "peak size", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakSize.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakSize.size)

			resourceIndex += 1
			table.setText(resourceIndex, 1, "peak count", "center")
			table.setText(resourceIndex, 2, "%d" % stat.peakCount.count)
			table.setText(resourceIndex, 3, "%d" % stat.peakCount.size)
			
			resourceIndex += 2
		
		table.write(file)
		
		# Reserve space at the bottom for leak data
		bmargin = len(contexts) + 9
		if bmargin < 9 + table.rows:
			bmargin = 9 + table.rows
		if bmargin < 15:
			bmargin = 15
		file.write("set bmargin %d\n" % bmargin)
		
		# plot the data files
		file.write("plot %s\n" % plotData)
		file.close()
		
		# Convert with gnuplot
		gnuplot = subprocess.Popen(["gnuplot", self.GNUPLOT_CONFIG], 0, None, None, stream, None)
		gnuplot.wait()
			
		# cleanup gnuplot configuration and data files
		os.remove(self.GNUPLOT_CONFIG)
		
		for resource in self.events.keys():
			os.remove(resource + "-size-peak.dat")
			os.remove(resource + "-count-peak.dat")
			for context in contexts:
				os.remove("%s-size-%x.dat" % (resource, context.value))
				os.remove("%s-count-%x.dat" % (resource, context.value))
	
# /class ActivityProcessor


class TotalsProcessor(Processor):
	"""
	The TotalsProcessor class generates 'totals' report.
	The 'totals' report contains graph illustrating the total resource 
	allocation over the time.
	"""
	GNUPLOT_CONFIG = "timeline.cfg"
	
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
			
		
	def writeReport(self, stream):
		"Writes the end report"
		xrange = 0
		yrange = 0
		plotData = ""
		# leak summing map
		stats = {}

		contexts = [Context(Context.MASK_ALL, "all allocations")]
		if len(self.contexts) > 0:
			contexts.append(Context(Context.MASK_NONE, "no contexts"))
			contexts.extend(self.contexts)
		
		# iterate through registered resources
		for resource in self.events.keys():
			# iterate through available contexts
			for context in contexts:
				# create the data file for the resource-context filter
				fileName = "%s-%x.dat" % (resource, context.value)
				file = open(fileName, "w")
				if file is None:
					print >> sys.stderr, "Failed to create data file: %s" % fileName
					sys.exit(2)
				file.write("Resource \"%s (%s)\"\n" % (resource, context.name))
					
				# resource indexing map 
				index = {}
				# total allocation size
				total = 0
				# flag indicating if the filter has any data
				hasData = False
				# initialize resource statistics data
				if context.isMaskAll():
					stat = self.Stats()
					stats[resource] = stat
				#
				events = self.events[resource]
				if events[-1].timestamp > xrange:
					xrange = events[-1].timestamp
					
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
						
					file.write("%d %d\n" % (event.timestamp, total))
					hasData = True
					if total > yrange:
						yrange = total
				file.close()
				if hasData:
					if plotData != "":
						plotData += ", "
					plotData += "\"%s\" using ($1/1000):2 title column(2)" % fileName
			# peak marker data.
			# Peak markers are vertical lines, drawn at the time of the max peak
			plotData += ", \"%s-peak.dat\" using ($1/1000):2 title column(2)" % resource

		# iterate through registered resources to generate peak allocation data files
		for resource in self.events.keys():
			fileName = resource + "-peak.dat"
			file = open(fileName, "w")
			if file is None:
				print >> sys.stderr, "Failed to create data file: %s" % fileName
				sys.exit(2)
			timestamp = stats[resource].peakTimestamp
			file.write("Resource \"%s (peak:%s)\"\n" % (resource, Timestamp.format(self.timestampOffset + timestamp)))
			file.write("%d %d\n" % (timestamp, 0))
			file.write("%d %d\n" % (timestamp, yrange))
			file.close()


		#	
		# generate gnuplot configuration file:
		#
		file = open(self.GNUPLOT_CONFIG, "w")
		if file is None:
			print >> sys.stderr, "Failed to create configuration file: %s" % self.GNUPLOT_CONFIG
			sys.exit(2)
		
		# set terminal properties
		file.write("set terminal postscript eps enhanced color\n")
		# set file title
		file.write("set title \"Amount of {/Helvetica-Italic}non-freed {/Helvetica} allocations\"\n")
		# set X axis range 
		file.write("set xrange[0.000:%.3f]\n" % (float(xrange) / 1000))
		# set Y axis range 
		file.write("set yrange[0:%d]\n" % (yrange))
		# format X axis tics
		xtic = float(int(xrange / 10)) / 1000
		if xtic == 0:
			xtic = 0.001
		file.write("set xtics %f,%f,%f\n" % (xtic, xtic, float(xrange) / 1000 - xtic))
		file.write("set xtics rotate\n")
		file.write("set xtics add (\"%s\" %f)\n" % (Timestamp.format(self.timestampOffset), 0))
		file.write("set xtics add (\"%s\" %f)\n" % (Timestamp.format(self.timestampOffset + xrange), float(xrange) / 1000))
		# set the X axis tic label format
		file.write("set format x \"+%.3f\"\n")
		# set the Y axis tic label format
		file.write("set format y \"%.1s%c\"\n")
		file.write("set ytics out\n")
		# set Y axis label
		file.write("set ylabel \"size\"\n")
		# set X axis label
		file.write("set xlabel \"time (secs)\" offset 0,-3\n")
		# set graph line style
		file.write("set style data lines\n")
		# write graph information outside graph
		file.write("set key bmargin\n")
		
		# Write summary report
		
		# define table headers
		table = Table(1, 1)
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

		table.setText(1, 0, "{/Helvetica-Italic-Bold}Resource", "center")
		table.setText(1, 1, "{/Helvetica-Italic-Bold}State", "center")

		table.setText(0, 2, "{/Helvetica-Italic-Bold}Total", "center")
		table.setText(0, 3, "{/Helvetica-Italic-Bold}Total", "center")
		table.setText(1, 2, "{/Helvetica-Italic-Bold}count", "center")
		table.setText(1, 3, "{/Helvetica-Italic-Bold}size", "center")

		table.setText(0, 4, "{/Helvetica-Italic-Bold}Non-freed", "center")
		table.setText(0, 5, "{/Helvetica-Italic-Bold}Non-freed", "center")
		table.setText(1, 4, "{/Helvetica-Italic-Bold}count", "center")
		table.setText(1, 5, "{/Helvetica-Italic-Bold}size", "center")
		
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
		
		table.write(file)
		
		# Reserve space at the bottom for leak data
		bmargin = len(contexts) + 9
		if bmargin < 9 + table.rows:
			bmargin = 9 + table.rows
		if bmargin < 15:
			bmargin = 15
		file.write("set bmargin %d\n" % bmargin)
		
		# plot the data files
		file.write("plot %s\n" % plotData)
		file.close()
		
		# Convert with gnuplot
		gnuplot = subprocess.Popen(["gnuplot", self.GNUPLOT_CONFIG], 0, None, None, stream, None)
		gnuplot.wait()
		
		# cleanup gnuplot configuration and data files
		os.remove(self.GNUPLOT_CONFIG)
		
		for resource in self.events.keys():
			os.remove(resource + "-peak.dat")
			for context in contexts:
				os.remove("%s-%x.dat" % (resource, context.value))
# /class TotalsProcessor
		
	
class Parser:
	"""
	This class parses the sp-rtrace format log file and calls the assigned processor
	to produce the required report file.
	"""
	processor = None
	reAlloc = re.compile("^[0-9]+\.(?: @([0-9a-fA-F]+)|) [^[]*\[([^\]]+)\][^(<]+(?:<([^>]+)>|)\(([^)]+)\) = (0x[a-fA-F0-9]+)(.*)$")
	reFree = re.compile("^[0-9]+\.(?: @([0-9a-fA-F]+)|) [^[]*\[([^\]]+)\][^(<]+(?:<([^>]+)>|)\((0x[a-fA-F0-9]+)\)$")
	reResource = re.compile("\<([0-9a-z]+)\> : ([^ ]+) \(([^\)]+)\)")
	reTimestamp = re.compile("^([0-9]+)\:([0-9]+)\:([0-9]+)\.([0-9]+)$")
	reContext = re.compile("^\@ ([0-9a-fA-F]+) : (.*)$")
	timestampOffset = 0
	
	def __init__(self, processor):
		self.processor = processor
		
	def read(self, stream):
		self.timestampOffset = 0
		for line in stream:
			match = self.reAlloc.match(line)
			if match:
				context = match.group(1)
				if context is None:
					context = 0
				self.processor.registerAlloc(context, self.getTimestampFromString(match.group(2)), match.group(3), match.group(5), int(match.group(4)))
				continue
			match = self.reFree.match(line)
			if match:
				context = match.group(1)
				if context is None:
					context = 0
				self.processor.registerFree(context, self.getTimestampFromString(match.group(2)), match.group(3), match.group(4))
				continue
			match = self.reResource.match(line)
			if match:
				self.processor.registerResource(match.group(2))
				continue
			match = self.reContext.match(line)
			if match:
				self.processor.registerContext(int(match.group(1), 16), match.group(2))
				continue
				
	def write(self, stream):
		self.processor.writeReport(stream);
		
	def getTimestampFromString(self, text):
		timestamp = 0
		match = self.reTimestamp.match(text)
		if match:
			timestamp = int(match.group(1)) * 3600000 + int(match.group(2)) * 60000 + int(match.group(3)) * 1000 + int(match.group(4))
		if self.timestampOffset == 0 and timestamp != 0:
			self.timestampOffset = timestamp
			self.processor.setTimestampOffset(timestamp)
		return timestamp - self.timestampOffset
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

	mode = Modes.NONE
	streamIn = sys.stdin
	streamOut = sys.stdout
	slice = 0
	
	def parse(argv):
		try:
			opts, args = getopt.gnu_getopt(argv, "tlao:i:s:", ["totals", "lifetime", "activity", "in=", "out=", "slice="])
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
				
			if opt == "-a" or opt == "--activity":
				Options.mode = Options.Modes.ACTIVITY
				parser = Parser(ActivityProcessor())
				
			if opt == "-i" or opt == "--in":
				Options.streamIn = open(val, "r")
				
			if opt == "-o" or opt == "--out":
				Options.streamOut = open(val, "w")
			
			if opt == "-s" or opt == "--slice":
				Options.slice = int(val)
				
		if parser is None:
			print >> sys.stderr, "No report type specified."
			Options.displayUsage()
			sys.exit(2)
			
		return parser
			
	
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
    -i <file>  input file.
    -o <file>  outtput file.
"""

		
	# static method definitions
	parse = staticmethod(parse)
	displayUsage = staticmethod(displayUsage)
	
# /class Options
	

#
# main()
#
parser = Options.parse(sys.argv[1:])
parser.read(Options.streamIn)
parser.write(Options.streamOut)


