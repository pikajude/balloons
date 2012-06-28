CC=clang
SRCDIR=balloons
BUILDDIR=build
CFLAGS=-g -Wall
TARGET=build/balloons
INCLUDE=-I/usr/local/include
LDFLAGS=-L/usr/local/lib
LDLIBS=-lcurl

SRCEXT=c
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
 
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET) $(INCLUDE) $(CFLAGS) $(LDFLAGS) $(LDLIBS)
 
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<
 
clean:
	rm -r $(BUILDDIR) $(TARGET)
 
.PHONY: clean
