// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gflags/gflags.h>
#include "rdkafkacpp.h"

using namespace std;


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
    public:
        void dr_cb (RdKafka::Message &message) {
            std::cout << "Message delivery for (" << message.len() << " bytes): " <<
                message.errstr() << " offset:" << message.offset() <<std::endl;
        }
};

class ExampleEventCb : public RdKafka::EventCb {
    public:
        void event_cb (RdKafka::Event &event) {
            switch (event.type())
            {
                case RdKafka::Event::EVENT_ERROR:
                    std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " <<
                        event.str() << std::endl;
                    break;

                case RdKafka::Event::EVENT_STATS:
                    std::cerr << "\"STATS\": " << event.str() << std::endl;
                    break;

                case RdKafka::Event::EVENT_LOG:
                    fprintf(stderr, "LOG-%i-%s: %s\n",
                            event.severity(), event.fac().c_str(), event.str().c_str());
                    break;

                default:
                    std::cerr << "EVENT " << event.type() <<
                        " (" << RdKafka::err2str(event.err()) << "): " <<
                            event.str() << std::endl;
                    break;
            }
        }
};

RdKafka::Conf *consumer_conf;
RdKafka::Conf *consumer_tconf;
RdKafka::Conf *producer_conf;
RdKafka::Conf *producer_tconf;
string errstr;
RdKafka::Consumer *consumer;
RdKafka::Producer *producer;
ExampleEventCb ex_event_cb;
ExampleDeliveryReportCb ex_dr_cb;


DEFINE_string(brokers,"10.10.38.70:9501,10.10.38.70:9502,10.10.38.70:9503","kafka broker list");
DEFINE_string(zookeeper,"10.10.38.70:2181","zookeeper");
DEFINE_string(topic,"rt_mem04","topic");
DEFINE_int32(par_id,0,"partition id");
DEFINE_int32(produce_num,10000,"partition id");
DEFINE_int32(consume_num,1000,"partition id");

int init_kafka();
void msg_consume(RdKafka::Message* message, void* opaque);
void produce(string topic_name, int partition_id,int id);
void consume(string topic_name, int partition_id, int64_t offset);



int main(int argc, char **argv) {

    ::google::ParseCommandLineFlags(&argc, &argv, true);

    init_kafka();
    for (int i=0; i<FLAGS_produce_num;i++){
        produce(FLAGS_topic, FLAGS_par_id,i);
    }
    cout<<"waiting 5 seconds"<<endl;
    sleep(5);
    for (int i=0; i<FLAGS_consume_num;i++){
        consume(FLAGS_topic, FLAGS_par_id,i);
    }

    delete consumer;
    delete producer;
    RdKafka::wait_destroyed(5000);
    return 0;
}

int init_kafka(){
    /*
     *Create configuration objects
     */
    consumer_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    consumer_tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    producer_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    producer_tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    consumer_conf->set("metadata.broker.list", FLAGS_brokers, errstr);
    consumer_conf->set("queued.min.messages", "1000000", errstr);
    //consumer_conf->set("statistics.interval.ms", "60*1000", errstr);
    consumer_conf->set("event_cb", &ex_event_cb, errstr);

    producer_conf->set("metadata.broker.list", FLAGS_brokers, errstr);
    producer_conf->set("queue.buffering.max.messages", "1000000", errstr);
    //producer_conf->set("statistics.interval.ms", "60*1000", errstr);
    producer_conf->set("event_cb", &ex_event_cb, errstr);
    producer_conf->set("dr_cb", &ex_dr_cb, errstr);
    producer = RdKafka::Producer::create(producer_conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    std::cout << "% Created producer " << producer->name() << std::endl;
    consumer = RdKafka::Consumer::create(consumer_conf, errstr);
    if (!consumer) {
        std::cerr << "Failed to create consumer: " << errstr << std::endl;
        exit(1);
    }
    std::cout << "% Created consumer " << consumer->name() << std::endl;
}



void msg_consume(RdKafka::Message* message, void* opaque) {
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            std::cout << "Read msg at offset " << message->offset() << std::endl;
            if (message->key()) {
                std::cout << "Key: " << *message->key() << std::endl;
            }

            printf("%.*s\n",static_cast<int>(message->len()),static_cast<const char *>(message->payload()));
            break;

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
           break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            break;

        default:
            /* Errors */
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
    }
}

void produce(string topic_name, int partition_id,int id) {
    cout<<"produce "<<id<<endl;
    char str[30];
    sprintf(str,"msg_%d",id);
    string msg = string(str,0,strlen(str));
    RdKafka::Topic *topic = RdKafka::Topic::create(producer, topic_name,producer_tconf, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }
    /*produce message*/
    RdKafka::ErrorCode resp =producer->produce(topic, partition_id,
            RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
            const_cast<char *>(msg.c_str()), msg.size(),
            NULL, NULL);
    if (resp != RdKafka::ERR_NO_ERROR){
        std::cerr << "% Produce failed: " <<RdKafka::err2str(resp) << std::endl;
    }else{
        std::cerr << "% Produced message (" <<msg.size() << " bytes)" <<std::endl;
    }
    delete topic;

}

void consume(string topic_name, int partition_id, int64_t offset) {
    cout<<"consume topic:"<<topic_name<<" par_id:"<<partition_id<<" offset:"<<offset<<endl;
    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, topic_name,consumer_tconf, errstr);
    if (!topic) {
        std::cerr << "Failed to create topic: " << errstr << std::endl;
        exit(1);
    }  
    RdKafka::ErrorCode resp = consumer->start(topic, partition_id, offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "Failed to start consumer: " <<RdKafka::err2str(resp) << std::endl;
        exit(1);
    }

    RdKafka::Message *msg = consumer->consume(topic, partition_id, 1000);
    msg_consume(msg, NULL);
    delete msg;
    consumer->poll(0);
    consumer->stop(topic, partition_id);
    consumer->poll(1000);

    delete topic;



}
