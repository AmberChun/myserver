SRCDIRS := .
SRCEXTS := .cpp

CPPFLAGS := -I../HBase -ggdb -Wall -Werror -fno-exceptions -Wno-deprecated -std=c++11 

ifeq ($(ver),debug)
	PROGRAM := hTest
	LIB_H := ../HBase/libh_d.a
	LDFLAGS := -L../HBase -lh_d
	CPPFLAGS += -O0 -DNDEBUG
else
	PROGRAM := hTest
	LIB_H := ../HBase/libh.a 
	LDFLAGS := -L../HBase -lh
	CPPFLAGS += -O2 -DNDEBUG
endif

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

ifeq ($(ver),debug)
	OBJS = $(foreach x, $(SRCEXTS),$(patsubst %$(x),%.od,$(filter %$(x),$(SOURCES))))
	DEPS = $(patsubst %.od,%.d,$(OBJS))
else
	OBJS = $(foreach x, $(SRCEXTS),$(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))
	DEPS = $(patsubst %.o,%.d,$(OBJS))
endif

LDFLAGS += -lpthread

.PHONY : all objs clean cleanall rebuild

all : $(PROGRAM)
%.d : %.cpp
	@$(CXX) -MM -MD $(CPPFLAGS) $<

objs : $(OBJS)
%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@
%.od : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(PROGRAM) : $(OBJS) $(LIB_H)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	mv -f $(PROGRAM) ../bin
	
-include $(DEPS)

rebuild : clean all

clean :
	@rm -rf *.o *.od  *.d

cleanall : clean
	@rm -rf $(PROGRAM)
