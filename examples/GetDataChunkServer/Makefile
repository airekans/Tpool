PROJECT_ROOT := ../..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -I $(PROJECT_ROOT)/include

LDFLAGS += -pthread
LDFLAGS += -L $(PROJECT_ROOT) -ltpool
LDFLAGS += -lboost_system -lprotobuf

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

PROTODIR = .
PROTOS = $(wildcard $(PROTODIR)/*.proto)
PROTO_SRCS = $(PROTOS:$(PROTODIR)/%.proto=$(SRCDIR)/%.pb.cc)
OBJS += $(PROTO_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

TARGET := GetDataChunkServer.test

#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $(OBJS) $(LDFLAGS)

$(SRCDIR)/%.pb.cc: $(PROTODIR)/%.proto
	protoc -I=$(PROTODIR) --cpp_out=$(SRCDIR) $^

clean:
	@rm -fv $(OBJS) $(TARGETS)

include $(PROJECT_ROOT)/Makefile.rules
