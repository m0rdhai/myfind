all: myfind
myfind: main.o args.o
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -o myfind main.o args.o
main.o: main.cpp args.h
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -c main.cpp
args.o: args.cpp args.h
	g++ -std=c++17 -Wall -Wextra -Werror -pedantic -c args.cpp
clean:
	rm -f *.o myfind