all:
	g++ -Iheaders src/*.cpp -o FileSystem -std=c++11
run:
	./FileSystem