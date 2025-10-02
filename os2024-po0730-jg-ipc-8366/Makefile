# Targets for executable files
BFILES = zadanie proc_serv2 proc_d proc_t proc_p1 proc_p2

# Compile all executables
all: $(BFILES)

zadanie: zadanie.cpp
	g++ -std=c++11 zadanie.cpp -o zadanie -lrt

proc_serv2: proc_serv2.cpp
	g++ -std=c++11 proc_serv2.cpp -o proc_serv2 -lrt

proc_d: proc_d.cpp
	g++ -std=c++11 proc_d.cpp -o proc_d -lrt

proc_t: proc_t.cpp
	g++ -std=c++11 proc_t.cpp -o proc_t -lrt

proc_p1: proc_p1.cpp
	g++ -std=c++11 proc_p1.cpp -o proc_p1 -lrt

proc_p2: proc_p2.cpp
	g++ -std=c++11 proc_p2.cpp -o proc_p2 -lrt

# Clean all executables and temporary files
clean:
	rm -f $(BFILES)
