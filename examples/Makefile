PROJECT_ROOT := ..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -g -I ../include

LDFLAGS += -pthread
LDFLAGS += -L .. -ltpool
LDFLAGS += -lboost_system
LDFLAGS += -lssh

SRCDIR := .
OBJDIR := .
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

TARGETS := $(OBJS:%.o=%.test)
SUBDIRS = $(shell find . -maxdepth 1 -type d -and -regex '\./[^.].*')

#### Rules ####
all: $(TARGETS) $(SUBDIRS)

%.test: %.o $(DEPEND_LIB)
	g++ -o $@ $< $(LDFLAGS)

$(SUBDIRS):
	cd $@; $(MAKE)

clean:
	@rm -fv $(OBJS) $(TARGETS)
	for d in $(SUBDIRS); \
	do \
		(cd $$d; $(MAKE) clean;) \
	done

.PHONY: $(SUBDIRS)

include $(PROJECT_ROOT)/Makefile.rules
