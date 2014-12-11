# Makefile for the testbench.

# Makefile
OBJS = NBLExpProducerConsumerBase.o NBLExpProducerConsumer.o NBLExpMandelbrot.o NBLExpGEMM.o NBLExperiment.o TestBenchCLI.o


all: testbench

%.o:	%.cpp
	g++ $(CFLAGS) $(TGFLAGS) -I../include -c $*.cpp -o $*.o

testbench: $(OBJS)
	g++ -pthread -o ../../testbench $(OBJS) $(LDFLAGS)

clean:
	@rm -f $(OBJS) $(OBJS) ../../testbench
