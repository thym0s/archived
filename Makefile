CXX = g++
CFLAGS = -std=c++11 -Wall -pedantic -g
LIBS = -Llibs

RM = rm -rf
# define the CPP source files
SRCS = archived_test.cpp

OBJS = $(SRCS:.cpp=.o)




.PHONY: depend clean docs internal_docs interface_docs test

all:    

test: archived_test
	./archived_test

archived_test: $(OBJS) 
	$(CXX) $(CFLAGS) $(LIBS) $(OBJS) -o $@ 

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

docs: internal_docs interface_docs

internal_docs:
	doxygen Doxyfile_internal

interface_docs:
	doxygen Doxyfile_interface

clean:
	$(RM) *.o archived_test

depend: $(SRCS)
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE THIS LINE -- make depend needs it

archived_test.o: archived.h
