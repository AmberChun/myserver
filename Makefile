HTEST_DIRS = HBase HTest

ALL_DIRS = $(HTEST_DIRS)

GLOBAL_COMPILE_OPT = -std=c++11 

export CC = gcc
export CXX = g++

.PHONY : test clean

test :
	@for dir in $(HTEST_DIRS);\
	do\
	  ver='debug' $(MAKE) -C $$dir ||exit 1;\
	done

clean:
	@for dir in $(ALL_DIRS);\
	do\
	  $(MAKE) -C $$dir clean;\
	done 
