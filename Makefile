CC=clang
SRCDIR=balloons
BENCHDIR=bench
BUILDDIR=build
BENCHBUILDDIR=build/bench
CFLAGS=-g -Wall
TARGET=build/balloons
INCLUDE=-I/usr/local/include
LDFLAGS=-L/usr/local/lib
LDLIBS=-lcurl
PREFIX=/usr/local

SRCEXT=c
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
BENCH_SOURCES=$(shell find $(BENCHDIR) -type f -name *.$(SRCEXT))
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
BENCHOBJECTS=$(patsubst $(BENCHDIR)/%,$(BENCHBUILDDIR)/%,$(BENCH_SOURCES:.$(SRCEXT)=.o))
NOMAIN=$(filter-out $(BUILDDIR)/main.o,$(OBJECTS))
DIR=$(mkdir -p build/damn build/packet)

.PHONY: directories

$(TARGET): directories $(OBJECTS)
	$(CC) $(filter-out $<,$^) -o $(TARGET) $(INCLUDE) $(CFLAGS) $(LDFLAGS) $(LDLIBS)

bench_%: directories $(BENCHDIR)/%.$(SRCEXT) $(NOMAIN)
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) $(LDLIBS) -I$(SRCDIR) -o $(BENCHBUILDDIR)/$@ $(filter-out $<,$^)
	@echo Running benchmark for $@
	./$(BENCHBUILDDIR)/$@

$(BUILDDIR)/%.o: directories $(SRCDIR)/%.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $(filter-out $<,$^)

clean:
	rm -rf $(BUILDDIR)

install: $(TARGET)
	install -m 0755 $(TARGET) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/balloons

directories:
	mkdir -p build/{damn,packet,bench}
