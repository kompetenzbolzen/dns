CC              = clang
CFLAGS          = -Wall -std=c89 -D_DEFAULT_SOURCE -Wvla \
		  -Wdeclaration-after-statement -Wstrict-prototypes \
		  -Wunreachable-code -Wsign-compare -Wimplicit-int-conversion \
		  -Wsign-conversion
LDFLAGS         = -lm
BUILDDIR        = build
SOURCEDIR       = src
OBJECTDIR       = obj

TESTDIR         = tests
TESTFLAGS       = $(CFLAGS) -D_TEST

OUTPUT          = dnsd

SRCS = $(wildcard $(SOURCEDIR)/*.c)
OBJS = $(SRCS:.c=.o)
OBJ  = $(OBJS:$(SOURCEDIR)/%=$(OBJECTDIR)/%)

TESTS = $(wildcard $(TESTDIR)/*.c)
TOBJS = $(TESTS:.c=.o)
TSUBS = $(filter-out $(OBJECTDIR)/main.o,$(OBJ))

RUNARGS = -p 5333

build: dir $(OBJ)
	@echo [LD] $(OBJ)
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$(OUTPUT) $(OBJ) $(LDFLAGS)

debug: CFLAGS += -g -D _DEBUG
debug: build;

build_test: LDFLAGS += -lcheck
build_test: dir $(TOBJS) $(TSUBS)
	@echo [LD] $(TOBJS) $(TSUBS)
	@$(CC) $(TESTFLAGS) -o $(TESTDIR)/run $(TOBJS) $(TSUBS) $(LDFLAGS)

test: build_test
	@$(TESTDIR)/run

dir:
	@mkdir -p $(OBJECTDIR)
	@mkdir -p $(BUILDDIR)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	@echo [TEST] $<
	@$(CC) $(TESTFLAGS) -c $< -o $@

#sudo setcap 'cap_net_bind_service=+ep' /path/to/prog
#to allow port access
run: build
	$(BUILDDIR)/$(OUTPUT) $(RUNARGS)

.PHONY: clean
clean:
	@echo [RM] $(OBJ) $(TOBJS)
	@echo [RM] $(BUILDDIR)/$(OUTPUT) $(TESTDIR)/run
	@rm -df  $(OBJ) $(TOBJS) $(TESTDIR)/run
	@rm -Rdf $(BUILDDIR) $(OBJECTDIR)

.PHONY: all
all: clean build

devsetup:
	@echo "[" > compile_commands.json
	@for file in $(SRCS); do \
		echo "{\"directory\":\"$(PWD)\",\"command\":\"$(shell which $(CC)) $(CFLAGS) -c $$file\",\"file\":\"$(PWD)/$$file\"}," >> compile_commands.json;\
	done
	@for file in $(TESTS); do \
		echo "{\"directory\":\"$(PWD)\",\"command\":\"$(shell which $(CC)) $(TESTFLAGS) -c $$file\",\"file\":\"$(PWD)/$$file\"}," >> compile_commands.json;\
	done
	@echo "]" >> compile_commands.json
