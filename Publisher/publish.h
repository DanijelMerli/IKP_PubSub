#pragma once

// send topic and msg to PubSub Engine
// returns number of bytes sent
// returns -1 if an error occured
int publish(char* topic, char* msg);


typedef struct PublisherData_st{
	char message[100];
	char topic[100];
}PublisherData;
