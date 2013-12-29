TARGETS=hw7_sender hw7_receiver

all: $(TARGETS)

hw7_sender: hw7_sender.c hw7.c
	gcc --std=gnu99 -o hw7_sender hw7_sender.c hw7.c 2>foo

hw7_receiver: hw7_receiver.c hw7.c
	gcc --std=gnu99 -o hw7_receiver hw7_receiver.c hw7.c 2>foo

clean:
	rm -f $(TARGETS)
