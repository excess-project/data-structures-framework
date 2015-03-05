# Makefile for the EXCESS data structures framework.

# Configuration

# Data-structure libraries
NOBLEDIR = /opt/NOBLE/stable
NOBLELIB = $(NOBLEDIR)/lib/Linux64_x86/libNOBLE64.a

TBBDIR = /opt/IntelTBB/tbb
TBBLIB = -Wl,-rpath,$(TBBDIR)/build/linux_intel64_gcc_cc4.6_libc2.15_kernel3.13.0_release -L$(TBBDIR)/build/linux_intel64_gcc_cc4.6_libc2.15_kernel3.13.0_release/ -ltbb

# Support libraries
#  OpenBLAS
BLASDIR = /opt/OpenBLAS/st
BLASLIB = $(BLASDIR)/lib/libopenblas.a

# General testbench configuration
TGFLAGS += -DUSE_NOBLE
TGFLAGS += -DUSE_TBB
TGFLAGS += -DUSE_BLAS
TGFLAGS += -DNOSAVEMANDELBROT

# Compiler settings
CFLAGS = -DLINUX64_X86 -O2 -fpermissive -march=native -DNDEBUG
CFLAGS += -I$(NOBLEDIR)/Include/ -I$(NOBLEDIR)/Src/ -I$(TBBDIR)/include -I$(BLASDIR)/include

LDFLAGS = $(NOBLELIB) $(TBBLIB) $(BLASLIB) -lrt

export

# Makefile

all:
	cd src; $(MAKE) all

clean:
	cd src; $(MAKE) clean
#	@rm 
