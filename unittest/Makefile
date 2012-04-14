TARGET := tpool.test

CXXFLAGS += -pthread
CXXFLAGS += -I ../include

LDFLAGS += -pthread
LDFLAGS += -L /usr/local/lib/gtest -lgtest_main -lgtest
LDFLAGS += -L .. -ltpool

PROJECT_ROOT = ..

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	g++ -o $@ $^ $(LDFLAGS)
	LD_LIBRARY_PATH=$(PROJECT_ROOT) ./$@

clean:
	rm -fv $(OBJS) $(TARGET)
