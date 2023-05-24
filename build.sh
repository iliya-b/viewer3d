#!/bin/bash
g++ --std=c++17 -I include -lglfw -framework OpenGL \
-I/Users/null_pointer/vcpkg/installed/x64-osx/include \
-L/usr/X11/lib -lpthread -lXrandr -lXi -L /Users/null_pointer/vcpkg/installed/x64-osx/lib \
-lassimp -lz -lminizip -lpoly2tri -lpugixml -ldl -g \
-fsanitize=address -fsanitize=undefined  -D_GLIBCXX_DEBUG_PEDANTIC \
glad.c importer/*.cpp _loader.cpp
#-D_GLIBCXX_DEBUG