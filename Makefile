all:
	g++ Shawarma.cpp src/glad.c -Iinclude -lglfw -lGL -ldl -o Shawarma

run: all
	./Shawarma

clean: 
	rm -f run 
