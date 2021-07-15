
RM = rm -f
MKDIR = mkdir -p
7ZA = 7za

BUILDD ?= build
SRCD ?= src

EXECUTABLE ?= $(BUILDD)/main
ZIPFILE ?= ../zipfile.zip
CFILES = $(wildcard $(SRCD)/*.c)
OFILES = $(patsubst %.c,%.o, $(CFILES))

CFLAGS += -Wall


.PHONY: all clean zip run

all: $(EXECUTABLE)

clean:
	@$(RM) $(OFILES)
	@$(RM) -r $(BUILDD)
	@$(RM) $(ZIPFILE)

zip: clean
	@$(7ZA) a $(ZIPFILE) ./*

run: $(EXECUTABLE)
	./$(EXECUTABLE)


$(EXECUTABLE): $(OFILES)
	@$(MKDIR) $(BUILDD)
	$(CC) $(LDFLAGS) $(OFILES) -o $(EXECUTABLE)

