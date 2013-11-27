SIZES=1024 2048 4096 4608 5120 5632 6144 6656 7168 7680 8192

ALL=$(addsuffix k_1000, $(addprefix random_, $(SIZES)))

all: $(ALL)

random_%k_1000: random.S
	$(CC) -o $@ -DSIZE_k=$* -DITERATIONS=1000 -nostdlib $^

clean:
	$(RM) $(ALL)

.PHONY: all clean
