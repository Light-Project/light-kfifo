# SPDX-License-Identifier: GPL-2.0-or-later
flags = -g -O0 -Wall -Werror -I src/ -lpthread
head  = src/kfifo.h src/macro.h
obj   = src/kfifo.o
demo  = examples/selftest examples/simple

all: $(demo)

%.o:%.c $(head)
	@ echo -e "  \e[32mCC\e[0m	" $@
	@ gcc -o $@ -c $< $(flags)

$(demo): $(obj) $(addsuffix .c,$(demo))
	@ echo -e "  \e[34mMKELF\e[0m	" $@
	@ gcc -o $@ $@.c $(obj) $(flags)

clean:
	@ rm -f $(obj) $(demo)
