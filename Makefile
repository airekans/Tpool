CXXFLAGS += -pthread
CXXFLAGS += -I ./include
LDFLAGS += -shared
LDFLAGS += # -L /usr/local/lib/gtest -lgtest_main -lgtest

LINKER := ld

TARGET := tpool.so

SRCS := $(wildcard src/*.cpp)

OBJS = $(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LINKER) $(LDFLAGS) -o $@ $^
