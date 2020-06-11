CC = gcc -std=c99
CFLAGS = -g -I ./
OBJS = ./main.o
TARGET = pa0_a

all: $(TARGET)

%.o: ./%.c
		$(CC) -c $^

$(TARGET): $(OBJS)
		$(CC) -o $@ $^

.PHONY: clean test help

clean:
	rm $(OBJS) $(TARGET)
	rm -f output*

help:
		@echo "The following options are provided with Make\n\t$$ make \t\t# build main\n\t$$ make clean \t# clean the build\n\t$$ make test \t# test cases"

test: pa0_a test_0 test_1 test_2 test_3 test_4 test_5 test_6 test_7 test_8 test_9

test_0: $(TARGET)
		@echo "Testing example0"; \
		./pa0_a 0 1 2 3 > output0; \
		diff -Naur output0 sample_output/output0; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_1: $(TARGET)
		@echo "Testing example1"; \
		./pa0_a 1 2 4 8	> output1; \
		diff -Naur output1 sample_output/output1; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_2: $(TARGET)
		@echo "Testing example2"; \
		./pa0_a 1024 2048 4096 8192	> output2; \
		diff -Naur output2 sample_output/output2; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_3: $(TARGET)
		@echo "Testing example3"; \
		./pa0_a 64 64 64 64	> output3; \
		diff -Naur output3 sample_output/output3; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_4: $(TARGET)
		@echo "Testing example4"; \
		./pa0_a -2 -4 -2 100 > output4; \
		diff -Naur output4 sample_output/output4; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_5: $(TARGET)
		@echo "Testing example5"; \
		./pa0_a -5 -10 -10 81 > output5; \
		diff -Naur output5 sample_output/output5; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_6: $(TARGET)
		@echo "Testing example6"; \
		./pa0_a -8 -4 -2 -1 > output6; \
		diff -Naur output6 sample_output/output6; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_7: $(TARGET)
		@echo "Testing example7"; \
		./pa0_a -99 99 99 -99 > output7; \
		diff -Naur output7 sample_output/output7; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_8: $(TARGET)
		@echo "Testing example8"; \
		./pa0_a 10 -10 20 -20 > output8; \
		diff -Naur output8 sample_output/output8; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_9: $(TARGET)
		@echo "Testing example9"; \
		./pa0_a 10 10 -20 -20 > output9; \
		diff -Naur output9 sample_output/output9; \
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi
