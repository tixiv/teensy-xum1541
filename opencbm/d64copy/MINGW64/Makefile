RELATIVEPATH=../
include ${RELATIVEPATH}MINGW64/config.make

LIBD64COPY=../libd64copy

OBJS = main.o \
 	  $(foreach t,d64copy fs gcr pp s1 s2 s3 std, $(LIBD64COPY)/$(t).o)

PROG = d64copy

include asm_files.make

include ${RELATIVEPATH}MINGW64/prgrules.make
