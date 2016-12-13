##############################
# Makefile for genSCADAtools
#
#    It's a library where I want to put together all functions I recursive need when developing free software SCADA or industrial control
#    software. (possibility to use it with pvbrowser)
#
#    'genSCADAtools' is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#    'genSCADAtools' is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with genSCADAtools.  If not, see <http://www.gnu.org/licenses/>.
################################

CC            = gcc
DEFINES = 
#32 bits or 64? GNU/Linux or others?
CFLAGS = -m64 -fPIC
CFLAGS2 = -shared

DEL_FILE = rm -f

OBJECTS_DIR = ./

SOURCES = gStDate.c \
          gStMisc.c \
          gStConv.c

OBJECTS = gStDate.o \
          gStMisc.o \
          gStConv.o

TARGET = libgStools.so

first:all

.SUFFIXES: .o .c

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

$(TARGET):  $(OBJECTS)  
	$(CC) $(CFLAGS2) -o $(TARGET) $(OBJECTS)

all: Makefile $(TARGET)

clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core

compiler_clean:

gStDate.o: gStDate.c
	$(CC) -c $(CFLAGS) gStDate.c

gStMisc.o: gStMisc.c
	$(CC) -c $(CFLAGS) gStMisc.c

gStConv.o: gStConv.c
	$(CC) -c $(CFLAGS) gStConv.c
