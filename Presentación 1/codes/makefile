
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

lectura: main.cpp lectura.cpp estructuras.h lectura.h
	$(CXX) $(CXXFLAGS) main.cpp lectura.cpp -o lectura
	clear
	./lectura

make all: main.cpp lectura.cpp greedy.cpp restricciones.cpp restricciones.h estructuras.h lectura.h  greedy.h
	$(CXX) $(CXXFLAGS) main.cpp lectura.cpp greedy.cpp restricciones.cpp -o all
#	clear 
	./all
clean:
	rm -f lectura
	rm -f all
