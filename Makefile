
RM = rm -f
MKDIR = mkdir -p
7ZA = 7za
UNZIP = unzip -qq

BUILDD ?= build
SRCD ?= src

DATABASE   ?= database.zip
EXECUTABLE ?= $(BUILDD)/sql
ZIPFILE ?= ../zipfile.zip
CFILES = $(wildcard $(SRCD)/*.c)
OFILES = $(patsubst %.c,%.o, $(CFILES))

CFLAGS += -Wall


.PHONY: all clean zip run prepare_db

all: $(EXECUTABLE)

clean:
	@$(RM) $(OFILES)
	@$(RM) $(ZIPFILE)
	@$(RM) -r $(BUILDD)
	@$(RM) $(EXECUTABLE)

zip: clean prepare_db
	.$(7ZA) a $(ZIPFILE) ./*

prepare_db:
	@$(MKDIR) $(BUILDD)
	$(UNZIP) -u -d $(BUILDD) $(DATABASE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)


$(EXECUTABLE): $(OFILES)
	@$(MKDIR) $(BUILDD)
	$(CC) $(LDFLAGS) $(OFILES) -o $(EXECUTABLE)

