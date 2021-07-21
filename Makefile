
RM = rm -f
MKDIR = mkdir -p
7ZA = 7za
UNZIP = unzip -qq

BUILDD ?= build
SRCD ?= src

DATABASE   ?= database.zip
PROGNAME   ?= sql
EXECUTABLE ?= $(BUILDD)/$(PROGNAME)
ZIPFILE ?= ../zipfile.zip
CFILES = $(wildcard $(SRCD)/*.c)
OFILES = $(patsubst %.c,%.o, $(CFILES))

CFLAGS += -Wall


.PHONY: all clean zip run prepare_db valgrind debug

all: $(EXECUTABLE)

clean:
	@$(RM) $(OFILES)
	@$(RM) $(ZIPFILE)
	@$(RM) $(EXECUTABLE)

zip: clean prepare_db
	$(7ZA) a $(ZIPFILE) ./*

prepare_db:
	@$(MKDIR) $(BUILDD)
	$(UNZIP) -u -d $(BUILDD) $(DATABASE)

run: $(EXECUTABLE)
	cd $(BUILDD) && ./$(PROGNAME)

valgrind: $(EXECUTABLE)
	cd $(BUILDD) && valgrind ./$(PROGNAME)

debug: CFLAGS+=-g 
debug: clean
debug: $(EXECUTABLE)


$(EXECUTABLE): $(OFILES)
	@$(MKDIR) $(BUILDD)
	$(CC) $(LDFLAGS) $(OFILES) -o $(EXECUTABLE)

