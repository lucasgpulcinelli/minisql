
RM = rm -f
MKDIR = mkdir -p
7ZA = 7za
UNZIP = unzip 

BUILDD ?= build
SRCD ?= src

DATABASE   ?= database.zip
EXECUTABLE ?= $(BUILDD)/sql
ZIPFILE ?= ../zipfile.zip
CFILES = $(wildcard $(SRCD)/*.c)
OFILES = $(patsubst %.c,%.o, $(CFILES))

CFLAGS += -Wall


.PHONY: all clean zip run

all: $(EXECUTABLE)

clean:
	@$(RM) $(OFILES)
	@$(RM) $(ZIPFILE)
	@$(RM) -r $(BUILDD)
	@$(RM) $(EXECUTABLE)

zip: clean
	@$(UNZIP) -d $(BUILDD) $(DATABASE)
	@$(7ZA) a $(ZIPFILE) ./*

run: $(EXECUTABLE)
	@$(UNZIP) -u -d $(BUILDD) $(DATABASE)
	./$(EXECUTABLE)


$(EXECUTABLE): $(OFILES)
	@$(MKDIR) $(BUILDD)
	$(CC) $(LDFLAGS) $(OFILES) -o $(EXECUTABLE)

