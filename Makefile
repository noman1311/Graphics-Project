CXX = D:/Mingw_compiler/ucrt64/bin/g++.exe
CXXFLAGS = -Iinclude -O2 -fopenmp -g -fdiagnostics-color=always
LDFLAGS = -Llib -fopenmp -lglfw3 -lopengl32 -lgdi32
SRCS = main.cpp src/glad.c src/Scene.cpp src/Texture.cpp src/ProceduralTextures.cpp src/HudRenderer.cpp src/TableTennisGame.cpp src/CarromGame.cpp src/ChessGame.cpp
TARGET = my_opengl_app.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	cmd /C del /Q $(TARGET)

run: $(TARGET)
	.\$(TARGET)
