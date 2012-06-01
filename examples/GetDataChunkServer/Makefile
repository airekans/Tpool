PROJECT_ROOT := ../..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -I $(PROJECT_ROOT)/include

LDFLAGS += -pthread
LDFLAGS += -L $(PROJECT_ROOT) -ltpool
LDFLAGS += -lboost_system

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TARGET := GetDataChunkServer.test

#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $< $(LDFLAGS)

clean:
	@rm -fv $(OBJS) $(TARGETS)

include $(PROJECT_ROOT)/Makefile.rules
