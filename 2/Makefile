TARGET = main.out
FLAGS = -pthread
# XXX: Don't forget backslash at the end of any line except the last one
HDRS = \
	    include

SRCS = \
	src/ft.c \
	src/st.c \
	src/main.c


.PHONY: all clean

all: $(SRCS)
	gcc -Wall -Wextra -w $(addprefix -I,$(HDRS)) -o $(TARGET) $(CFLAGS) $(SRCS) $(FLAGS)

clean:
	rm -rf $(TARGET)
