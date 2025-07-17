# Makefile for Arc Fitting Example
# 圆弧拟合示例编译文件

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = arc_fitting_example
SOURCES = ArcFitting.cpp ArcFittingExample.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# 默认目标
all: $(TARGET)

# 编译可执行文件
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 编译目标文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 运行示例
run: $(TARGET)
	./$(TARGET)

# 清理编译文件
clean:
	rm -f $(OBJECTS) $(TARGET)

# 显示帮助信息
help:
	@echo "可用目标 (Available targets):"
	@echo "  all     - 编译程序 (Compile program)"
	@echo "  run     - 编译并运行示例 (Compile and run example)"
	@echo "  clean   - 清理编译文件 (Clean compiled files)"
	@echo "  help    - 显示此帮助信息 (Show this help)"

.PHONY: all run clean help