CXXFLAGS += -pthread
LDFLAGS += -L /usr/local/lib/gtest -lgtest_main -lgtest

SRCS := Thread.cpp \
	test.cpp

OBJS = $(SRCS:%.cpp=%.o)

all: test.test

test.test: $(OBJS)
	g++ -o $@ $(CXXFLAGS) $^ $(LDFLAGS)