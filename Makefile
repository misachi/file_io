CC := gcc
OUT_DIR := /tmp/file_io
OUT_FILE := $(OUT_DIR)/main.o
PERF_DIR := perf_out
CURR_DIR := $(shell pwd)

all:
	@mkdir -p $(OUT_DIR)
	@rm -f $(OUT_DIR)/stalls.txt && touch $(OUT_DIR)/stalls.txt
	$(CC) -g main.c -o $(OUT_FILE)
	$(OUT_FILE)

perf: all
	@cd $(OUT_DIR); \
	sudo perf record -F 99 -g $(OUT_FILE); \
	sudo perf script | stackcollapse-perf.pl > out; \
	flamegraph.pl --colors=blue out > $(CURR_DIR)/$(PERF_DIR)/profile.svg; \
	cd $(CURR_DIR);

clean:
	@rm -f out *.data *.data.old *.o $(OUT_DIR)/*.o $(OUT_DIR)/*.data $(OUT_DIR)/*.data.old