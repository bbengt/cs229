assignment := $(shell git symbolic-ref --short HEAD | cut -c3-6)
OBJS = map.o binary_search.o mon.o player.o rungame.o screen.o heap.o main.o io.o

.PHONY: clean objs dungeon cleanup

%.o: %.c
	echo Compiling $<
	gcc -MMD -MF $?.d -c $<

dungeon: $(OBJS) 
	echo Linking $@
	gcc $^ -o $@ -lm -lncurses

clean:
	rm -f dungeon
	rm -f out.txt
	rm -f *.d
	rm -f *.o

cleanup:
	rm -f *.d
	rm -f *.o

tar:
	mkdir bengtson_brian.assignment-$(assignment)
	cp *.c *.h extras/README extras/CHANGELOG extras/Makefile.real bengtson_brian.assignment-$(assignment)/
	mv bengtson_brian.assignment-$(assignment)/Makefile.real bengtson_brian.assignment-$(assignment)/Makefile
	tar cvfz bengtson_brian.assignment-$(assignment).tar.gz bengtson_brian.assignment-$(assignment)/
	rm -rf bengtson_brian.assignment-$(assignment)/
