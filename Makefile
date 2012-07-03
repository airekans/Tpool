ifdef STATIC
LIBTYPE :=
LIB_SUFFIX := .a
else
LIBTYPE := -shared
CXXFLAGS += -fPIC
LIB_SUFFIX := .so
endif

CXXFLAGS += -Wall -Werror
CXXFLAGS += -I ./include

LDFLAGS += -pthread

TARGET := libtpool$(LIB_SUFFIX)

TEST_DIR := unittest
EXAMPLE_DIR := examples

HDRDIR := include
SRCDIR := src
OBJDIR := src
HDRS := $(wildcard $(HDRDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

LINT = ./cpplint.py

#### Rules ####
all: $(TARGET)

ifdef STATIC
$(TARGET): $(OBJS)
	$(AR) rcs $@ $^
else
$(TARGET): $(OBJS)
	$(LINK.cpp) $(LIBTYPE) -Wl,-soname,$@ -o $@.1.0 $^ $(LDFLAGS)
	ln -fs $@.1.0 $@
endif

include Makefile.rules

test:
	@cd $(TEST_DIR); $(MAKE)

example:
	@cd $(EXAMPLE_DIR); $(MAKE)

clean:
	rm -fv $(OBJS) $(TARGET).1.0 $(TARGET)
	@cd $(TEST_DIR); $(MAKE) clean

lint:
	for f in $(HDRS) $(SRCS); \
	do \
		$(LINT) $$f; \
		echo "================="; \
	done
