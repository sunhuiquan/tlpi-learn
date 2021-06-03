TLPI_LIB = /usr/local/lib/libtlpi.a
GEN_EXE = msg_seqnum_client msg_seqnum_server 

msg_seqnum_client.o msg_seqnum_server.o  : msg_seqnum.h

all: ${GEN_EXE}

clean: 
	${RM} ${GEN_EXE} *.o
	
showall:
	@ echo ${GEN_EXE}

${GEN_EXE} : ${TLPI_LIB}
