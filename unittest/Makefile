TARGET := tpool.test
PROJECT_ROOT := ..
DEPEND_LIB := $(wildcard $(PROJECT_ROOT)/libtpool.*)

CXXFLAGS += -I ../include
CXXFLAGS += -Wall -Werror

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
