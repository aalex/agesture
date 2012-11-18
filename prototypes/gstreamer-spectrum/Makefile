DEPS=gstreamer-0.10 gstreamer-fft-0.10 glib-2.0

all: run

run: main.c
	gcc -o run main.c -Wall `pkg-config --libs --cflags $(DEPS)`

clean:
	rm -rf run

