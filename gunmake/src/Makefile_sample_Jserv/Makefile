CC = gcc
CFLAGS = -O0 -std=gnu99 -Wall -fopenmp -mavx
EXECUTABLE = \
	time_test_baseline time_test_openmp_2 time_test_openmp_4 \
	time_test_avx time_test_avxunroll \
	time_test_leibniz time_test_leibniz_openmp_2 time_test_leibniz_openmp_4 \
	time_test_leibniz_avx time_test_leibniz_avxunroll \
	time_test_euler time_test_euler_openmp_2 time_test_euler_openmp_4 \
	time_test_euler_avx time_test_euler_avxunroll \
	benchmark_clock_gettime methods_error_rate

GIT_HOOKS := .git/hooks/pre-commit
METHOD ?= BASELINE

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

default: $(GIT_HOOKS) $(EXECUTABLE)

benchmark_clock_gettime: computepi.o benchmark_clock_gettime.c
	$(CC) $(CFLAGS) $? -D$(METHOD) -o $@ -lm

methods_error_rate: computepi.o methods_error_rate.c
	$(CC) $(CFLAGS) $? -o $@ -lm

time_test_%: computepi.o %_method.o
	$(CC) $(CFLAGS) $? -o $@ -lm

.PHONY: clean default

%_method.o: time_test.c
	$(CC) -c $(CFLAGS) $< -D$(shell echo $(subst _method.o,,$@) | tr a-z A-Z) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

check: default
	time ./time_test_baseline
	time ./time_test_openmp_2
	time ./time_test_openmp_4
	time ./time_test_avx
	time ./time_test_avxunroll
	time ./time_test_leibniz
	time ./time_test_leibniz_openmp_2
	time ./time_test_leibniz_openmp_4
	time ./time_test_leibniz_avx
	time ./time_test_leibniz_avxunroll
	time ./time_test_euler
	time ./time_test_euler_openmp_2
	time ./time_test_euler_openmp_4
	time ./time_test_euler_avx
	time ./time_test_euler_avxunroll

gencsv: default
	for i in `seq 1008 4000 1000000`; do \
		printf "%d " $$i;\
		./benchmark_clock_gettime $$i; \
	done > result_clock_gettime.csv

plot: gencsv
	gnuplot scripts/runtime.gp

gencsv-methods: default
	for i in `seq 1008 4000 1000000`; do \
		printf "%d " $$i;\
		./methods_error_rate $$i; \
	done > methods_error_rate.csv

plot-methods: gencsv-methods
	gnuplot scripts/methods_error_rate.gp

clean:
	rm -f $(EXECUTABLE) *.o *.s *.png *.csv
