CC =gcc

ifeq ($(DEBUG), 1)
    CFLAGS =-DDEBUG
endif

all:	faketime date

check:	all
	./faketime 21137400 ./date

clean:
	rm -f faketime date

program1: faketime.c $(CFLAGS)
program2: date.c $(CFLAGS)
