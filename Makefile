CC := gcc
OUT_DIR := ~/.tmp/file_io
OUT_FILE := $(OUT_DIR)/stalls.o

stalls:
	@mkdir -p $(OUT_DIR)
	$(CC) -g main.c -o $(OUT_FILE)
	$(OUT_FILE)

clean:
	@rm -f $(OUT_DIR)/out $(OUT_DIR)/*.data $(OUT_DIR)/*.data.old *.o $(OUT_DIR)/*.o
