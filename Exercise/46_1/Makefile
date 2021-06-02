TLPI_LIB = /usr/local/lib/libtlpi.a
GEN_EXE = svmsg_create svmsg_send svmsg_receive 

all: ${GEN_EXE}

clean: 
	${RM} ${GEN_EXE} *.o
	
showall:
	@ echo ${GEN_EXE}

${GEN_EXE} : ${TLPI_LIB}
