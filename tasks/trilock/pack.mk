PROGS_TRILOCK_OBJS := trilock.o
PROGS_TRILOCK_OBJS := $(PROGS_TRILOCK_OBJS:%=$(TDIR)/trilock/%)
ALL_OBJS += $(PROGS_TRILOCK_OBJS)

$(TDIR)/bin/trilock : $(TSTART) $(PROGS_TRILOCK_OBJS) $(TLIBC)
