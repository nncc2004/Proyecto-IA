
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra


make all: main.cpp lectura.cpp greedy.cpp restricciones.cpp restricciones.h estructuras.h lectura.h  greedy.h
	$(CXX) $(CXXFLAGS) main.cpp lectura.cpp greedy.cpp restricciones.cpp -o all
	#	clear 
	./all

make all2: main2.cpp lectura.cpp greedy.cpp restricciones.cpp restricciones.h estructuras.h lectura.h  greedy.h
	$(CXX) $(CXXFLAGS) main2.cpp lectura.cpp greedy.cpp restricciones.cpp -o all2
	clear 
	./all2
clean:
	rm -f all
	rm -f all2
