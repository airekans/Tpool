TARGET := tpool.test
PROJECT_ROOT := ..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -I ../include
CPPFLAGS += -D_DEBUG_

LDFLAGS += -pthread
LDFLAGS += -L /usr/local/lib/gtest -lgtest_main -lgtest
LDFLAGS += -L .. -ltpool

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $(OBJS) $(LDFLAGS)
	LD_LIBRARY_PATH=$(PROJECT_ROOT) ./$@

clean:
	rm -fv $(OBJS) $(TARGET)

include  $(PROJECT_ROOT)/Makefile.rules
