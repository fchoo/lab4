SYSCALL_OBJS := io.o proc.o time.o c_swi_handler.o swi_handler.o
SYSCALL_OBJS := $(SYSCALL_OBJS:%=$(KDIR)/syscall/%)

KOBJS += $(SYSCALL_OBJS)
