# This Makefile isn't part of the original cyoencode package but was provided
# by Manuel Reithuber for the picosync project. I'm however putting this up
# under the BSD License (the same license cyoencode uses).

OBJFILES=build/CyoEncode.o build/CyoDecode.o
TGTLIB=libcyoencode.a

all: $(TGTLIB)

clean:
	rm -f $(OBJFILES) $(TGTLIB)
	rmdir build/

libcyoencode.a: $(OBJFILES)
	ar rcs $(TGTLIB) $(OBJFILES)
	ranlib $(TGTLIB)

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build/
