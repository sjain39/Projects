all: master worker

clean:
	rm {master,worker}

master: master.cpp
	g++ -Wno-write-strings master.cpp -o master

worker: worker.cpp
	g++ -Wall worker.cpp -o worker

gdb: master.cpp worker.cpp
	g++ -g -Wall master.cpp -o master
	g++ -g -Wall worker.cpp -o worker
