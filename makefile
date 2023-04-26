CC = clang
CLAFGS = -Wall
outp = a.out

OBJ_PREREQS = rt.o trace.o lodepng/lodepng.o
$(outp): $(OBJ_PREREQS)
	$(CC) $(CLAFGS) -o $@ -lm $^

*.o: *.c
	$(CC) $(CLAFGS) -o $@ -c $<

phony: clean

clean:
	rm -rf $(OBJ_PREREQS); \
	rm -f $(outp)