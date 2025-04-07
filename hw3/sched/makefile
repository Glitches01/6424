all: sender receiver threshold

sender: sched_sender.c
	gcc -O2 -o sender sched_sender.c

receiver: sched_receiver.c
	gcc -O2 -o receiver sched_receiver.c

threshold: sched_threshold.c
	gcc -O2 -o threshold sched_threshold.c



clean:
	rm -f sender receiver threshold

