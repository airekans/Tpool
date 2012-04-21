PROJECT_ROOT := ..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -I ../include

LDFLAGS += -pthread
LDFLAGS += -L .. -ltpool
LDFLAGS += -lboost_system

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TARGETS := $(OBJS:%.o=%.test)

#### Rules ####
all: $(TARGETS)

%.test: %.o $(DEPEND_LIB)
	g++ -o $@ $< $(LDFLAGS)

clean:
	@rm -fv $(OBJS) $(TARGETS)

include $(PROJECT_ROOT)/Makefile.rules
