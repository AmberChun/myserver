SRCDIRS := .
SRCEXTS := .cpp
#Werror 所有警告当错误处理
#Wshadow 不允许内层作用域有变量名和外层作用域变量名相同
#Wall 允许gcc提供所有有用警告 （一般都得带）
#Werror 添加一些额外的警告 （具体自己google）（如果函数有没有使用过的参数，也会报错）
#fno-exceptions 禁用异常机制
#Wno-deprecated 如果使用了一些废弃的接口 该选项可以屏蔽这些警告
#std=c++11 使用C++11
#ggdb 产生GDB所需的调试信息


CPPFLAGS := -I../HBase -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -ggdb -Wall  -Werror  -Wno-deprecated -std=c++11  

ifeq ($(ver),debug)
	PROGRAM := hData
	LIB_H := ../HBase/libh_d.a
	LDFLAGS := -L../HBase -lh_d  
	CPPFLAGS += -O0 -DNDEBUG
else
	PROGRAM := hData
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

LDFLAGS += -lpthread -L/usr/local/lib64 -lmongocxx -lbsoncxx 



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
