#-fpermissive

all: txgate appWatch
.PHONY: all

ifeq ($(CC),cc)
CC=arm-linux-gnueabihf-
#CC=
#CFLAGS := -m32
endif

#src := $(shell ls *.c*)
src := $(wildcard *.c* ./MX61/*.c* ./NTU_F9P/*.c* ./NET/*.c* ./YK/*.c* ./COMM/*.c*)
objs := $(patsubst %.c* ,%.o,$(src))
	
txgate: $(objs)
	$(CC)g++ -g -o  $@ $^ $(LDFLAGS) $(CFLAGS) -lpthread 

src2 := $(wildcard ./watchdog/*.c* ./COMM/*.c*)
objs2 := $(patsubst %.c* ,%.o,$(src2))

appWatch: $(objs2)
	$(CC)g++ -g -o  $@ $^ $(LDFLAGS) $(CFLAGS) -lpthread

%.o: %.c*
	$(CC)g++ -g -c -o $@ $< $(LDFLAGS) $(CFLAGS)  -lpthread

clean:
	@rm -rf txgate appWatch *.o tx_log
