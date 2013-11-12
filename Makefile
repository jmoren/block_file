CC = gcc
CFLAGS = -g -O0 -pedantic -Wall

SRCDIR  = src
TESTDIR = test
OBJSDIR = objs
TMPDIR  = tmp
OBJS = $(OBJSDIR)/block.o $(OBJSDIR)/record.o $(OBJSDIR)/parser.o

# Main target
cargador_masivo: record.o block.o parser.o
	$(CC) $(CFLAGS) -o cargador_masivo $(OBJS) main.c

# Clean, test and cargador masivo
full: clean test record.o block.o parser.o
	$(CC) $(CFLAGS) -o cargador_masivo $(OBJS) main.c

# Test targets
test: test-block test-record test-block-records

test-block: record.o block.o parser.o
	$(CC) $(CFLAGS) -o $(TESTDIR)/block_test $(OBJS) $(TESTDIR)/block_test.c

test-record: record.o block.o parser.o
	$(CC) $(CFLAGS) -o $(TESTDIR)/record_test $(OBJS) $(TESTDIR)/record_test.c

test-block-records: record.o block.o parser.o
	$(CC) $(CFLAGS) -o $(TESTDIR)/block_records_test $(OBJS) $(TESTDIR)/block_records_test.c

# Object targets
parser.o: block.o record.o set-up
	$(CC) $(CFLAGS) -c -o $(OBJSDIR)/parser.o $(SRCDIR)/parser.c

block.o: record.o set-up
	$(CC) $(CFLAGS) -c -o $(OBJSDIR)/block.o $(SRCDIR)/block.c

record.o: set-up
	$(CC) $(CFLAGS) -c -o $(OBJSDIR)/record.o $(SRCDIR)/record.c

# Set up & clean up
set-up:
	@mkdir -p $(OBJSDIR)
	@mkdir -p $(TMPDIR)

clean_sort:
	@echo " * Eliminamos directorio temporal ($(TMPDIR)"
	@rm -rf $(TMPDIR)

clean:
	@echo " * Eliminamos binarios (incluidos los tests)"
	@if [ -f cargador_masivo ]; then rm cargador_masivo; fi
	@if [ -f test/block_test ]; then rm test/block_test; fi
	@if [ -f test/record_test ]; then rm test/record_test; fi
	@if [ -f test/block_records_test ]; then rm test/block_records_test; fi
	@echo " * Eliminamos directorios temporales ($(TMPDIR) y $(OBJSDIR))"
	@rm -rf $(OBJSDIR)
	@rm -rf $(TMPDIR)
	@echo " * Eliminamos archivos de datos (*.dat)"
	@find . -name '*.dat' | xargs rm -f


