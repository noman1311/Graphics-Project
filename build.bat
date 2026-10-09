@echo off
cd /d D:\MyProgramming\Graphics
D:\Mingw_compiler\ucrt64\bin\g++.exe -fdiagnostics-color=always -g src\glad.c src\Scene.cpp src\Texture.cpp src\ProceduralTextures.cpp main.cpp -Iinclude -Llib -lglfw3 -lopengl32 -lgdi32 -o my_opengl_app.exe
echo Exit code: %ERRORLEVEL%
