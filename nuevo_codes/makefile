CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra



all: main2.cpp lectura.cpp  greedy.cpp restricciones.cpp f_eval.cpp hc.cpp restricciones.h estructuras.h lectura.h  greedy.h f_eval.h hc.h
	$(CXX) $(CXXFLAGS) main2.cpp lectura.cpp greedy.cpp restricciones.cpp f_eval.cpp hc.cpp -o all
	clear
	./all 2>&1 | tee logs.txt

clean:
	rm -f all
	clear
