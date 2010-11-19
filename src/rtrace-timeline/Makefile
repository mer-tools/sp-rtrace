CXXFLAGS=-g
CXX=g++

all: timeline.h.gch timeline


timeline: \
	timeline.o options.o parser.o resource_registry.o terminal.o plotter.o \
	activity_generator.o filter_manager.o filter.o histogram_generator.o \
	lifetime_generator.o totals_generator.o processor.o
	$(CXX) $^ -o $@

%.o: %.cpp %.h 
	$(CXX) $< -o $@ -c $(CXXFLAGS)

%.h.gch: %.h
	$(CXX) -c $<  $(CXXFLAGS)

clean:
	rm timeline *.o *.gch
