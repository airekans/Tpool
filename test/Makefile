CXXFLAGS += -pthread
CXXFLAGS += -I ../include

LDFLAGS += -pthread
LDFLAGS += -L /usr/local/lib/gtest -lgtest_main -lgtest
LDFLAGS += -L .. -ltpool


TARGET := tpool.test
SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)
	./$@
