CC=gcc
AR=ar
CXXFLAGS=-c -Wall
CPPFLAGS=
LDFLAGS=
ARFLAGS=-rvs

LZMASOURCES=$(wildcard *.c)
LZMAOBJECTS=$(addprefix obj/,$(notdir $(LZMASOURCES:.c=.o)))

# A directory creation utility
create_output_dir=@mkdir -p $(@D)

all: out/liblzma.a
	@echo all done.

clean:
	rm -rf obj/
	rm -rf out/

out/liblzma.a: $(LZMAOBJECTS)
	$(create_output_dir)
	$(AR) $(ARFLAGS) $@ $^

$(LZMAOBJECTS): obj/%.o: %.c
	$(create_output_dir)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

