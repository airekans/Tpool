PROJECT_ROOT := ../..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -I $(PROJECT_ROOT)/include

LDFLAGS += -pthread
LDFLAGS += -L $(PROJECT_ROOT) -ltpool
LDFLAGS += -lboost_system -lprotobuf

PROTODIR = .
PROTOS = $(wildcard $(PROTODIR)/*.proto)
PROTO_SRCS = $(PROTOS:$(PROTODIR)/%.proto=$(SRCDIR)/%.pb.cc)
PROTO_HDRS = $(PROTOS:$(PROTODIR)/%.proto=$(SRCDIR)/%.pb.h)
OBJS += $(PROTO_SRCS:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS += $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TARGET := GetDataChunkServer.test

#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $(OBJS) $(LDFLAGS)

$(SRCDIR)/%.pb.cc: $(PROTODIR)/%.proto
	protoc -I=$(PROTODIR) --cpp_out=$(SRCDIR) $^

run: $(TARGET)
	LD_LIBRARY_PATH=$(PROJECT_ROOT) ./$(TARGET)

clean:
	@rm -fv $(OBJS) $(TARGETS) $(PROTO_SRCS) $(PROTO_HDRS)

include $(PROJECT_ROOT)/Makefile.rules
