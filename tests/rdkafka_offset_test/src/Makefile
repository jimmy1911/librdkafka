PREFIX=./thirdparty
INCLUDE=-I./ -I./gen-cpp -I./util -I./msgproto\
        -I${PREFIX}/gflags/include/ \
        -I${PREFIX}/librdkafka/include \


LIB=\
    -L${PREFIX}/gflags/lib -lgflags\
	-L${PREFIX}/librdkafka/lib -lrdkafka++ -lrdkafka  \
	-lpthread -ldl  -rdynamic -lz  -lrt

CFLAG=-g   -std=c++0x
TARGET=rdkafka_test 

all : ${TARGET}
obj_dir=./obj
VPATH+=${obj_dir} ./main	
cmp_dir = $(abspath .)
git_info = $(shell git log -1 |awk  '{if($$1=="commit"){print "ver=",substr($$2,0,8)}; if($$1=="Author:"){print "::user=",$$2}; if($$1=="Date:"){print "::time=",$$5"@"$$6"/"$$3"/"$$4}}'|tr -d "\n\t " )

main_obj=main.o
rdkafka_test : ${main_obj}
	g++ ${CFLAG} $(patsubst %.o,$(obj_dir)/%.o,$(^F)) -o ./$@ $(LIB)


%.o:%.cpp 
	mkdir -p ${obj_dir}
	g++ -c ${INCLUDE} -D_DIR=\"${cmp_dir}\" -D_GIT=\"${git_info}\" ${CFLAG} $< -o ${obj_dir}/$@

%.o:%.cc 
	mkdir -p ${obj_dir}
	g++ -c ${INCLUDE} -D_DIR=\"${cmp_dir}\" -D_GIT=\"${git_info}\" ${CFLAG} $< -o ${obj_dir}/$@

clean:
	rm -f ${TARGET}
	rm -rf ${obj_dir}
