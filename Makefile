cc = gcc
objects = obj/jrb.o obj/fields.o obj/jval.o obj/main.o

link: compile
	gcc $(objects) -o bin/kripto

compile: $(objects)

obj/jrb.o: src/jrb.c
	$(cc) -I include -c $< -o $@

obj/fields.o: src/fields.c
	$(cc) -I include -c $< -o $@

obj/jval.o: src/jval.c
	$(cc) -I include -c $< -o $@

obj/main.o: src/main.c
	$(cc) -I include -c $< -o $@

clean:
	@rm obj/*
	@rm bin/*

