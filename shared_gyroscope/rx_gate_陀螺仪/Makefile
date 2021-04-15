#-fpermissive

all: rxgate appWatch
.PHONY: all

ifeq ($(CC),cc)
CC=arm-linux-gnueabihf-
#CC=
#CFLAGS := -m32
endif

#src := $(shell ls *.c*)
src := $(wildcard *.c* ./MX61/*.c* ./NTU_F9P/*.c* ./NET/*.c* ./NET/CJsonObject/*.c* ./YK/*.c* ./COMM/*.c*)
#src := $(wildcard *.c* ./MX61/*.c* ./NTU_F9P/*.c* ./NET/*.c*)
objs := $(patsubst %.c* ,%.o,$(src))

rxgate: $(objs)
	$(CC)g++ -g -o  $@ $^ $(LDFLAGS) $(CFLAGS) -lpthread

src2 := $(wildcard  ./watchdog/*.c* ./COMM/*.c*)
objs2 := $(patsubst %.c* ,%.o,$(src2))

appWatch: $(objs2)
	$(CC)g++ -g -o  $@ $^ $(LDFLAGS) $(CFLAGS) -lpthread

%.o: %.c*
	$(CC)g++ -g -c -o $@ $< $(LDFLAGS) $(CFLAGS)  -lpthread

clean:
	@rm -rf rxgate *.o appWatch rx_log
