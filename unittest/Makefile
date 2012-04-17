TARGET := tpool.test
PROJECT_ROOT := ..
DEPEND_LIB := $(PROJECT_ROOT)/libtpool.so

CXXFLAGS += -pthread
CXXFLAGS += -I ../include

LDFLAGS += -pthread
LDFLAGS += -L /usr/local/lib/gtest -lgtest_main -lgtest
LDFLAGS += -L .. -ltpool


SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)


all: $(TARGET)

$(TARGET): $(OBJS) $(DEPEND_LIB)
	g++ -o $@ $^ $(LDFLAGS)
	LD_LIBRARY_PATH=$(PROJECT_ROOT) ./$@

clean:
	rm -fv $(OBJS) $(TARGET)
