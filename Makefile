CC = gcc
CFLAGS = -std=c99 -O3 -w  

SRC = cache_sim.c
HEADERS = $(wildcard *.h) 
OBJ = $(SRC:.c=.o)

TARGET = cache_simulator

raw: $(TARGET)
	@echo "Build complete."

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)



clean:
	rm -f $(OBJ) $(TARGET)
	@echo "Cleaned up all generated files."


cacti:
	@if [ ! -x ./cacti ]; then \
		echo "Error: 'cacti' executable not found or not executable."; \
		exit 1; \
	fi

run: cacti $(TARGET)
	@echo "Enter values l1_size l1_assoc l1_block_size l1_vc_num_blocks  l2_size l2_assoc :";
	@read  l1_size l1_assoc l1_block_size l1_vc_num_blocks  l2_size l2_assoc ; \
	./$(TARGET)  $$l1_size $$l1_assoc $$l1_block_size $$l1_vc_num_blocks  $$l2_size $$l2_assoc  > results.txt;
	@echo "results will be available in results.txt file."
performance:
	make -f Makefile2
	@echo "results will be available in output.txt file."
