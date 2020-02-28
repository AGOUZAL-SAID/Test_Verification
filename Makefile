CC=cc
CFLAGS=-g -Wall -std=c99

# We need to include some source files from unity into the build
UNITY=Unity
UNITY_FILES=$(UNITY)/src/unity.c

# Source files and their objects
SRCS=$(wildcard src/*.c)
OBJS=$(SRCS:.c=.o)


# Add your tests here!
TESTS 	= TestLightScheduler
#TESTS		+= TestLightControlSpy
#TESTS		+= TestFakeTimeService



# Test sources and runners. Don't touch the following lines!
TEST_SRCS=$(addprefix test/,$(addsuffix(.c, $(TESTS))))
RUNNERS=$(addprefix test/,$(addsuffix(_Runner.c, $(TESTS))))
TARGETS=$(addprefix run_, $(TESTS))

# Include directories
INCL=-Iinclude -I$(UNITY)/src


.PHONY: all clean run_tests

default all: clean run_tests

%.o: %.c
	$(CC) $(CFLAGS) $(INCL) $(COVFLAGS) -c $< -o $@

test/%_Runner.c: test/%.c
	ruby $(UNITY)/auto/generate_test_runner.rb $< $@

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
