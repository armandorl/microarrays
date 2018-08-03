

# Makefile
# This project uses Microchip x16 compiler

CC = xc16-gcc
RM = -rm -rf
MKDIR = mkdir
MV = mv

EXECUTABLE = MicroArrays.hex
OBJ = system.o main.o interrupts.o vad.o init.o serialDriver.o
LFLAGS = -Wl,--local-stack,,,,,--script=p33EP256GP502.gld,--heap=128,--stack=32,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--library=dsp,--library=q,--no-force-link,--smart-io,-Map="microarrays.debug.map",--report-mem,--memorysummary,memoryfile.xml 

CFLAGS = -c -mcpu=33EP256GP502  -MMD -mno-eds-warn  -omf=elf  -no-legacy-libc -msmall-code -msmall-data -O0 -msmart-io=1 -Wall -msfr-warn=off

CINCLUDE = -I./headers \
		   -I"C:\Program Files (x86)\Microchip\xc16\v1.34\support\generic\h" \
		   -I"C:\Program Files (x86)\Microchip\xc16\v1.34\support\dsPIC33E\h"


all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJ)
	$(RM) bin ; true
	$(CC) -o $@ $^ $(LFLAGS)
	$(MKDIR) bin
	$(MV) *.o *.hex *.xml *.map *.d ./bin


%.o: %.c
	$(CC) $(CFLAGS) $(CINCLUDE) -c $< -o $@

clean:
	$(RM) bin *.o *.hex *.xml *.map *.d ; true