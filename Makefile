# Makefile for the EXCESS data structures framework.

# Configuration

# Data-structure libraries
NOBLEDIR = /opt/NOBLE/stable
NOBLELIB = $(NOBLEDIR)/lib/Linux64_x86/libNOBLE64.a

TBBDIR = /opt/IntelTBB/tbb
TBBLIB = -Wl,-rpath,$(TBBDIR)/build/linux_intel64_gcc_cc4.6_libc2.15_kernel3.13.0_release -L$(TBBDIR)/build/linux_intel64_gcc_cc4.6_libc2.15_kernel3.13.0_release/ -ltbb

# Hash map implementations from
#  [Nhan Nguyen and Philippas Tsigas, "Lock-free Cuckoo Hashing".
#   In the Proceedings of the 34th International Conference on Distributed
#   Computing Systems (ICDCS 2014), pages 627 - 636, IEEE press 2014].
#CCKHT = /opt/MiscDS/CCKHashtable
#CCKHTLIB = $(CCKHT)/libCCKHashtable.a

# Support libraries
#  OpenBLAS
BLASDIR = /opt/OpenBLAS/st
BLASLIB = $(BLASDIR)/lib/libopenblas.a

#  EXCESS MF integration
EXCESSMFDIR = /opt/mf/0.1.4
EXCESSMFURL = \"http://192.168.0.2:3000\"
EXCESSMFLIB = #-Wl,-rpath=$(EXCESSMFDIR)/lib $(EXCESSMFDIR)/lib/libmf.so

# General testbench configuration
TGFLAGS += -DUSE_NOBLE
TGFLAGS += -DUSE_TBB
#TGFLAGS += -DUSE_CCKHT
TGFLAGS += -DUSE_BLAS
TGFLAGS += -DNOSAVEMANDELBROT
#TGFLAGS += -DUSE_EXCESS_MF

# Compiler settings
CFLAGS = -DLINUX64_X86 -O2 -fpermissive -march=native -DNDEBUG
CFLAGS += -I$(NOBLEDIR)/Include/ -I$(NOBLEDIR)/Src/
CFLAGS += -I$(TBBDIR)/include -I$(BLASDIR)/include
CFLAGS += -I$(CCKHT)
CFLAGS += -I$(EXCESSMFDIR)/include -DEXCESSMFURL=$(EXCESSMFURL)

LDFLAGS = $(NOBLELIB) $(TBBLIB) $(BLASLIB) $(EXCESSMFLIB) -lrt
LDFLAGS += $(CCKHTLIB)

export

# Makefile

all:
	cd src; $(MAKE) all

clean:
	cd src; $(MAKE) clean
#	@rm 
