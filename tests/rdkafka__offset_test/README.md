Step1:
make clean;make 

Step2:

cd output;
modify config as you like
-brokers                //broker list
-topic                  //topic for test
-par_id=0               //partition of topic to test
-produce_num=10000      //number of calling produce  
-consume_num=1000       //number of calling consume

Step3:
./start.sh


TestCase1:
set produce_num = 10000
set consume_num=1000
consume with the right offset

TestCase2:
set produce_num = 100000
set consume_num=1000
consume with wrong offset, the log is  liks this:
#################################################
consume topic:kafka_test_10000 par_id:0 offset:1
Read msg at offset 30594
msg_20594
consume topic:kafka_test_10000 par_id:0 offset:2
Read msg at offset 2
msg_2
consume topic:kafka_test_10000 par_id:0 offset:3
consume topic:kafka_test_10000 par_id:0 offset:4
Read msg at offset 3
msg_3
consume topic:kafka_test_10000 par_id:0 offset:5
Read msg at offset 5
msg_5
consume topic:kafka_test_10000 par_id:0 offset:6
Read msg at offset 30598
msg_20598
################################################
