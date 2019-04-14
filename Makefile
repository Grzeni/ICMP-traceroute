src = $(wildcard *.c)
obj = $(src:.c=.o)

FLAGS = -Wall -Wextra -Werror -std=gnu99

traceroute: $(obj)
	$(CC) -o $@ $^ $(FLAGS)

.PHONY: clean distclean
clean:
	rm  -f $(obj)

distclean:
	rm  -f $(obj) traceroute
