clang -v -I./glfw/include/GLFW -I./glfw/include -o example1.o example1.c -L$(pwd)/glfw/build/src -Wl,-rpath=/$(pwd)/glfw/build/src -lGL -lglfw
