KERNELDIR := $(TOP)$(notdir $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST)))))/
INCDIRS += $(shell find $(KERNELDIR)$(INCDIR) -type d)
KERNELSRC := $(KERNELDIR)$(SRCDIR)
KERNELASM := $(KERNELDIR)$(ASMDIR)
KERNELCFILES := $(wildcard $(KERNELSRC)*.c)
KERNELASMFILES := $(wildcard $(KERNELASM)*.S)
KERNEL_COFILES := $(KERNELCFILES:%.c=%.c.o)
KERNEL_SOFILES += $(KERNELASMFILES:%.S=%.S.o)

#Debug part
KERNELDEBUGDIR := $(KERNELDIR)$(DEBUG_OBJECTS_DIR)
KERNELDOFILES := $(patsubst $(KERNELDIR)$(SRCDIR)%,$(KERNELDEBUGDIR)%,$(KERNEL_COFILES))
KERNELDOFILES += $(patsubst $(KERNELDIR)$(ASMDIR)%,$(KERNELDEBUGDIR)%,$(KERNEL_SOFILES))

#Release part
KERNELRELEASEDIR := $(KERNELDIR)$(RELEASE_OBJECTS_DIR)
KERNELROFILES := $(patsubst $(KERNELDIR)$(SRCDIR)%,$(KERNELRELEASEDIR)%,$(KERNEL_COFILES))
KERNELROFILES += $(patsubst $(KERNELDIR)$(ASMDIR)%,$(KERNELRELEASEDIR)%,$(KERNEL_SOFILES))

#Make sure the output paths are created
OBJECTDIRS += $(KERNELDEBUGDIR) $(KERNELRELEASEDIR)

#Rules for compiling the ASM and C files to output files
$(KERNELDEBUGDIR)%.c.o $(KERNELRELEASEDIR)%.c.o: $(KERNELSRC)%.c
	$(CCRECEIPE)

$(KERNELDEBUGDIR)%.S.o $(KERNELRELEASEDIR)%.S.o: $(KERNELASM)%.S
	$(ASMRECEIPE)
