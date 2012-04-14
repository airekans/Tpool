CXXFLAGS += -pthread -fPIC -Wall
CXXFLAGS += -I ./include

LIBTYPE := -shared

LDFLAGS += -pthread

LINKER := g++

TARGET := libtpool.so

TEST_DIR := unittest

HDRS := $(wildcard include/*.h)
SRCS := $(wildcard src/*.cpp)
OBJS = $(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LIBTYPE) -Wl,-soname,$@ -o $@.1.0 $^ $(LDFLAGS)
	ln -fs $@.1.0 $@

test:
	cd $(TEST_DIR); $(MAKE)

clean:
	rm -fv $(OBJS) $(TARGET).1.0 $(TARGET)
	cd $(TEST_DIR); $(MAKE) clean

depend: $(SRCS)
	for s in $(SRCS); do \
	g++ -I ./include -MM -MT $$s $$s; \
	done
