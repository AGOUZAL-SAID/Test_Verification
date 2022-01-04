CC=cc
CFLAGS=-g -Wall -std=c99

# We need to include some source files from cmock and unity into the build
CMOCKDIR=CMock
UNITYDIR=CMock/vendor/unity
UNITY_FILES=$(UNITYDIR)/src/unity.c $(CMOCKDIR)/src/cmock.c

# command to execute to create mock objects
CMOCK=ruby $(CMOCKDIR)/lib/cmock.rb

# Source files
SRCS=$(wildcard src/*.c)


# =============================================================
# Add your tests here!
# =============================================================
TESTS 	= TestLightScheduler
#TESTS		+= TestLightControlSpy


# =============================================================
# Modules to mock
# =============================================================
TOMOCK = TimeService


MOCKDIR=./mocks
MOCKS = $(addprefix $(MOCKDIR)/Mock, $(addsuffix .c, $(TOMOCK)))
SRCS += $(MOCKS)

# Objects
OBJS=$(SRCS:.c=.o)


# Test sources and runners. Don't touch the following lines!
TEST_SRCS=$(addprefix test/,$(addsuffix .c, $(TESTS)))
RUNNERS=$(addprefix test/,$(addsuffix _Runner.c, $(TESTS)))
TARGETS=$(addprefix run_, $(TESTS))

# Include directories
INCL=-Iinclude -I$(UNITYDIR)/src -I$(CMOCKDIR)/src -I$(MOCKDIR)


.PHONY: all clean run_tests cmock_init

default all: clean cmock_init mock_objects run_tests

cmock_init: CMock/vendor/unity/README.md
	cd CMock && git submodule init && git submodule update

mock_objects: $(MOCKS)

mocks/Mock%.c: include/%.h
	$(CMOCK) $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCL) $(COVFLAGS) -c $< -o $@

test/%_Runner.c: test/%.c
	ruby $(UNITYDIR)/auto/generate_test_runner.rb $< $@

run_%: $(OBJS) test/%.c test/%_Runner.c
	echo $(OBJS)
	$(CC) $(CFLAGS) $(INCL) $(OBJS) $(UNITY_FILES) test/$*.c test/$*_Runner.c -o $@

run_tests: $(TARGETS)
	@for TEST in $(TARGETS); do		\
		echo Running $${TEST};		\
		./$${TEST};			\
	done

clean:
	rm -f  $(OBJS) $(TARGETS) $(RUNNERS) *~ src/*~ test/*~ include/*~
	rm -rf mocks
	rm -f *.gcda *.gcno *.info src/*.gcda src/*.gcno
	rm -rf coverage


coverage: CFLAGS += --coverage -O0
coverage: clean $(TARGETS)
	lcov -c -i -d src --path src -o baseline.info
	for TEST in $(TARGETS); do     \
		echo Running $${TEST};       \
		./$${TEST};							     \
		lcov -c -d src --path src -o test.info; \
		lcov --path src -a baseline.info -a test.info -o total.info; \
		cp total.info baseline.info; \
	done
	genhtml total.info -o coverage
