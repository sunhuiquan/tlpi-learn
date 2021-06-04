TLPI_LIB = /usr/local/lib/libtlpi.a
GEN_EXE = svshm_xfr_reader svshm_xfr_writer 

svshm_xfr_reader.o svshm_xfr_writer .o  : svshm_xfr.h

all: ${GEN_EXE}

clean: 
	${RM} ${GEN_EXE} *.o
	
showall:
	@ echo ${GEN_EXE}

${GEN_EXE} : ${TLPI_LIB}
