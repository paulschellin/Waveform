
# g++ -std=c++11 -I. -I $GTEST_HOME/include -L $GTEST_HOME/lib -lgtest -lgtest_main -lpthread test_Waveform.cpp -o test_Waveform


# If you did not install Google Test via your package manager, you must manually explicitly point g++ to:
#	$(GTEST_HOME)/include		for the headers
#	$(GTEST_HOME)/lib			for the libs
# (GTEST_HOME isn't defined by default, you must do it)

# I highly recommend just leaving the installation of Google Test to your package manager.
# Though apparently Google disagrees. I'd go with Google on this one, if I were you.

#CXX=clang++
#CXX=g++-4.8
#LD=$(CXX)

TESTS=Waveform FftwTransform IdentityTransform
TEST_SOURCES=$(addprefix test_src/,$(addsuffix _test.cpp,$(TESTS)))
#TEST_SOURCES=$(addprefix test_,$(addsuffix .cpp,$(TESTS)))


#TEST_SOURCES=test_src/*.cpp
#TESTS=$(subst _test,,$(basename $(notdir $(TEST_SOURCES))))

TEST_EXES=$(addprefix test_bin/,$(addsuffix _test,$(TESTS)))

#TEST_SOURCES=$(addsuffix .cpp,$(addprefix test_src/,$(TESTS)))

MAKEFILE=makefile

gtest_dir=gtest-1.7.0

std_lib_flags=-std=c++11

ifeq ($(shell uname -s),Darwin)
	gtest_dir=gtest-1.7.0-svn-mac
	std_lib_flags=-std=c++11 -stdlib=libc++
endif

#gtest_dir=$(GTEST_HOME)
INCLUDE_DIRS=-I. -I$(gtest_dir)/include

gtest_libs=-L$(gtest_dir) -lgtest -lgtest_main
boost_libs=-lboost_iostreams -lboost_serialization

LIBS=$(gtest_libs) -lpthread -lfftw3 $(boost_libs)


.PHONY: all
all:	$(TESTS)


.SECONDEXPANSION:
$(TESTS):	test_src/$$@_test.cpp $$@.hpp $(MAKEFILE)
	$(CXX) $(std_lib_flags) $(INCLUDE_DIRS) $(LIBS) $< -o test_bin/$@_test

#$(TESTS):	$(MAKEFILE) $$@.hpp test_src/$$@_test.cpp
#	$(CXX) $(std_lib_flags) $(INCLUDE_DIRS) $(LIBS) test_src/$@_test.cpp -o test_bin/$@_test



%.o:	%.cpp
	$(CXX) -c $(WARNFLAGS) $(CXXFLAGS) $< -o $@


.PHONY: clean
clean:
	rm -f $(TEST_EXES)


#.PHONY: testall
#test: $(TESTS)
#	./$<
