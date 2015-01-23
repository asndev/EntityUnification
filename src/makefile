CXX = g++ -O3 -Wall -std=c++0x -I gtest/include
MAIN_BINARIES = $(basename $(wildcard *main.cc))
TEST_BINARIES = $(basename $(wildcard *test.cc))
HEADER = $(wildcard *.h)
OBJECTS = $(addsuffix .o, $(basename $(filter-out %main.cc %test.cc,$(wildcard *.cc)))) 
LIBGTEST = libgtest.a

.PRECIOUS: %.o


all: compile test checkstyle

compile: $(MAIN_BINARIES) $(TEST_BINARIES)

test: $(TEST_BINARIES)
	for T in $(TEST_BINARIES); do ./$$T; done

checkstyle:
	python ../cpplint.py *.h *.cc

clean:
	rm -f *.o
	rm -f $(MAIN_BINARIES)
	rm -f $(TEST_BINARIES)
	rm -f *.class
	rm -f *test.TMP.*
	rm -f core

%main: %main.o $(OBJECTS)
	$(CXX) -o $@ $^ $(LIBS)

%test: %test.o $(OBJECTS) $(LIBGTEST)
	$(CXX) -o $@ $^ $(LIBGTEST) -lpthread

%.o: %.cc $(HEADER)
	$(CXX) -c $<

$(LIBGTEST):
	$(CXX) -I gtest/include -I gtest -c gtest/src/gtest-all.cc
	$(CXX) -I gtest/include -I gtest -c gtest/src/gtest_main.cc
	ar -rv $@ gtest-all.o gtest_main.o

