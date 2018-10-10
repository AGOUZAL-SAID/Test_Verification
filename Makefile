CC=gcc

CFLAGS=-Wall -std=c99

# We need to include some source files from unity into the build
UNITY=Unity
UNITY_FILES=$(UNITY)/src/unity.c 

# Source files
SRCS=$(UNITY_FILES) $(wildcard src/*.c)



# Add additional tests here!
TESTS  = TestLightScheduler 
TESTS += TestLightControlSpy



# Test sources and runners. Don't touch the following lines!
TEST_SRCS=$(addprefix test/,$(addsuffix(.c, $(TESTS))))
RUNNERS=$(addprefix test/,$(addsuffix(_Runner.c, $(TESTS))))
TARGETS=$(addprefix run_, $(TESTS))

# Include directories
INCL=-Iinclude -I$(UNITY)/src 


.PHONY: all clean run_tests

default all: clean run_tests

test/%_Runner.c: test/%.c 
	ruby $(UNITY)/auto/generate_test_runner.rb $< $@

run_%: $(SRCS) test/%.c test/%_Runner.c
	$(CC) $(CFLAGS) $(INCL) $(SRCS) test/$*.c test/$*_Runner.c -o $@

run_tests: $(TARGETS)
	@for TEST in $(TARGETS); do		\
		echo Running $${TEST};		\
		./$${TEST};			\
	done					

clean:
	rm -rf $(TARGETS) $(RUNNERS) *~ src/*~ test/*~ include/*~
