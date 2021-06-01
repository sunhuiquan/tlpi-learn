TLPI_LIB = libtlpi.a
GEN_EXE = fifo_seqnum_client fifo_seqnum_server 

fifo_seqnum_client.o fifo_seqnum_server.o : fifo_seqnum.h

all: ${GEN_EXE}

clean: 
	${RM} ${GEN_EXE} *.o
	
showall:
	@ echo ${GEN_EXE}

${GEN_EXE} : ${TLPI_LIB}
