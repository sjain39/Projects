all: findit

findit: findit.cpp
	g++ -Wno-write-strings findit.cpp -o findit 2>foo

gdb: findit.c
	g++ -Wno-write-strings -g findit.cpp -o findit

clean:
	rm findit
