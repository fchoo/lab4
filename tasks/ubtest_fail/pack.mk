PROGS_UBTEST_FAIL_OBJS := ubtest_fail.o
PROGS_UBTEST_FAIL_OBJS := $(PROGS_UBTEST_FAIL_OBJS:%=$(TDIR)/ubtest_fail/%)
ALL_OBJS += $(PROGS_UBTEST_FAIL_OBJS)

$(TDIR)/bin/ubtest_fail : $(TSTART) $(PROGS_UBTEST_FAIL_OBJS) $(TLIBC)

