CXX = D:/Mingw_compiler/ucrt64/bin/g++.exe
CXXFLAGS = -Iinclude -g -fdiagnostics-color=always
LDFLAGS = -Llib -lglfw3 -lopengl32 -lgdi32
SRCS = main.cpp src/glad.c src/Scene.cpp
TARGET = my_opengl_app.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	del $(TARGET)

run: $(TARGET)
	.\$(TARGET)
