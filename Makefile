CXXFLAGS += -pthread -fPIC
CXXFLAGS += -I ./include
LDFLAGS += -shared
LDFLAGS += # -L /usr/local/lib/gtest -lgtest_main -lgtest

LINKER := gcc

TARGET := libtpool.so

TEST_DIR := test

SRCS := $(wildcard src/*.cpp)

OBJS = $(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -Wl,-soname,$@.1 -o $@.1.0 $^
	mv $@.1.0 $@

test: test_target

test_target:
	cd $(TEST_DIR); $(MAKE)