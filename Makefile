CXXFLAGS += -fPIC -Wall
CXXFLAGS += -I ./include

LIBTYPE := -shared

LDFLAGS += -pthread

TARGET := libtpool.so

TEST_DIR := unittest

HDRDIR := include
SRCDIR := src
OBJDIR := src
#HDRS := $(wildcard $(HDRDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)


#### Rules ####
all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINK.cpp) $(LIBTYPE) -Wl,-soname,$@ -o $@.1.0 $^ $(LDFLAGS)
	ln -fs $@.1.0 $@

include Makefile.rules

test:
	cd $(TEST_DIR); $(MAKE)

clean:
	rm -fv $(OBJS) $(TARGET).1.0 $(TARGET)
	cd $(TEST_DIR); $(MAKE) clean

