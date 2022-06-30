CFLAGS = -O2 -Wall
objects = panel-info-sensors.o
target = rc-panel-info-sensors

%.o : %.c
	$(CC) $(CFLAGS) -c $<

$(target) : $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(target)

.PHONY : clean
clean: 
	rm -f $(objects) $(target)
