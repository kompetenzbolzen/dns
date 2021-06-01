CC              = clang
CFLAGS          = -Wall -std=c89 -D_DEFAULT_SOURCE
LDFLAGS         = -lm
BUILDDIR        = build
SOURCEDIR       = src
OBJECTDIR       = obj
TESTDIR         = tests

OUTPUT          = dnsd

SRCS = $(wildcard $(SOURCEDIR)/*.c)
OBJS = $(SRCS:.c=.o)
OBJ  = $(OBJS:$(SOURCEDIR)/%=$(OBJECTDIR)/%)

TESTS = $(wildcard $(TESTDIR)/*.c)
TOBJS = $(TESTS:.c=.o)
TSUBS = $(filter-out $(OBJECTDIR)/main.o,$(OBJ))

build: dir $(OBJ)
	@echo [LD] $(OBJ)
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$(OUTPUT) $(OBJ) $(LDFLAGS)

debug: CFLAGS += -g -D _DEBUG
debug: build;

test: LDFLAGS += -lcheck
test: clean dir $(TOBJS) $(TSUBS)
	@echo [LD] $(TOBJS) $(TSUBS)
	@$(CC) $(CFLAGS) -I $(SOURCEDIR) -o $(TESTDIR)/run $(TOBJS) $(TSUBS) $(LDFLAGS)
	@$(TESTDIR)/run

dir:
	@mkdir -p $(OBJECTDIR)
	@mkdir -p $(BUILDDIR)

$(OBJECTDIR)/%.o: $(SOURCEDIR)/%.c
	@echo [CC] $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	@echo [TEST] $<
	@$(CC) $(CFLAGS) -I $(SOURCEDIR) -c $< -o $@

#sudo setcap 'cap_net_bind_service=+ep' /path/to/prog
#to allow port access
run: build
	$(BUILDDIR)/$(OUTPUT)

.PHONY: clean
clean:
	@echo [RM] $(OBJ) $(TOBJS)
	@echo [RM] $(BUILDDIR)/$(OUTPUT) $(TESTDIR)/run
	@rm -df  $(OBJ) $(TOBJS) $(TESTDIR)/run
	@rm -Rdf $(BUILDDIR) $(OBJECTDIR)

.PHONY: all
all: clean build

devsetup:
	@echo "$(CFLAGS)" | tr ' ' '\n' > compile_flags.txt
