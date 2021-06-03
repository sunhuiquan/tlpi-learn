TLPI_LIB = /usr/local/lib/libtlpi.a
GEN_EXE = svmsg_file_client svmsg_file_server

svmsg_file_client.o svmsg_file_server.o : svmsg_file.h

all: ${GEN_EXE}

clean: 
	${RM} ${GEN_EXE} *.o
	
showall:
	@ echo ${GEN_EXE}

${GEN_EXE} : ${TLPI_LIB}
