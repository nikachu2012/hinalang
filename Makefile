TARGET = a.out

CC = clang
SRCS = $(wildcard *.cpp **/*.cpp)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))
DEPS = $(patsubst %.cpp, %.d, $(SRCS))

LLVM_CONFIG = llvm-config --link-static
CXXFLAGS += $(shell $(LLVM_CONFIG) --cxxflags) -g -Wall
LDFLAGS  += $(shell $(LLVM_CONFIG) --ldflags) -lLLVM -lstdc++

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	rm -rf **/*.o *.o **/*.d *.d

-include $(DEPS)

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# -MMD: ファイルの依存関係を表した%.dファイルを生成する
# -MP : ヘッダファイルに対してダミーの依存関係を作ることでMakefileのエラー防止
%.o: %.cpp
	$(CC) $(CXXFLAGS) -MMD -MP -c $< -o $@
