SRCDIRS := .
SRCEXTS := .cpp

CPPFLAGS := -ggdb -Wall -Werror -fno-exceptions -Wno-deprecated -std=c++11 

ifeq ($(ver),debug)
	PROGRAM := libh_d.a
	CPPFLAGS += -O0 -DNDEBUG
else
	PROGRAM := libh.a
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


.PHONY : all objs clean cleanall rebuild
all : $(PROGRAM)
%.d : %.cpp
	@$(CC) -MM -MD $(CPPFLAGS) $<
objs : $(OBJS)
%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@
%.od : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

$(PROGRAM) : $(OBJS)
	@rm -rf $(PROGRAM)
	@ar cru $(PROGRAM) $(OBJS)
#	@ranlib $(PROGRAM)
-include $(DEPS)

rebuild : clean all

clean:
	@rm -rf *.o *.od *.d

cleanall : clean
	@rm -rf $(PROGRAM)
