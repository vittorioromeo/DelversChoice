CXX = clang++
CXX_FLAGS = -std=c++14 -Wall -Wextra -Iinclude -pthread -Wall \
			-Wextra -Wpedantic -Wundef -Wshadow -Wno-missing-field-initializers \
			-Wpointer-arith -Wcast-align -Wwrite-strings -Wno-unreachable-code \
			-Wnon-virtual-dtor -Woverloaded-virtual -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC \
			-lsfml-system -lsfml-graphics -lsfml-audio -lsfml-network -lsfml-window 

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

main: $(OBJ_FILES)
	$(CXX) $(CXX_FLAGS) main.cpp -o $@ $^

obj/%.o: src/%.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

clean:
	rm ./main
	rm ./obj/*.o