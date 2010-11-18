CFLAGS=-g
CXX=g++
all: timeline

timeline: timeline.o options.o parser.o resource_registry.o terminal.o plotter.o \
	activity_generator.o filter_manager.o filter.o histogram_generator.o
	$(CXX) $^ -o $@
	
%.o: %.cpp %.h generator_data.h
	$(CXX) $< -o $@ -c $(CFLAGS)

clean:
	rm timeline *.o
