CXXFLAGS += -pthread -fPIC
CXXFLAGS += -I ./include
LDFLAGS += -shared
LDFLAGS += # -L /usr/local/lib/gtest -lgtest_main -lgtest

LINKER := g++

TARGET := libtpool.so

TEST_DIR := unittest

HDRS := $(wildcard include/*.h)
SRCS := $(wildcard src/*.cpp)
OBJS = $(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -Wl,-soname,$@ -o $@.1.0 $^
	ln -s $@.1.0 $@

test:
	cd $(TEST_DIR); $(MAKE)

clean:
	rm -fv $(OBJS) $(TARGET).1.0 $(TARGET)
	cd $(TEST_DIR); $(MAKE) clean

depend: $(SRCS)
	for s in $(SRCS); do \
	g++ -MM -MT $s $s; \
	done