# Makefile for the testbench.

# Makefile
OBJS = NBLExperiment.o TestBenchCLI.o
OBJS += NBLExpProducerConsumerBase.o NBLExpProducerConsumer.o NBLExpMandelbrot.o
OBJS += NBLExpGEMM.o
OBJS += NBLExpDictionaryBase.o NBLExpDictionary.o
OBJS += NBLExpWeakDictionaryBase.o NBLExpWeakDictionary.o

all: testbench

%.o:	%.cpp
	g++ $(CFLAGS) $(TGFLAGS) -I../include -c $*.cpp -o $*.o

testbench: $(OBJS)
	g++ -pthread -o ../../testbench $(OBJS) $(LDFLAGS)

clean:
	@rm -f $(OBJS) $(OBJS) ../../testbench
