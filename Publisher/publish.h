#pragma once

// send topic and msg to PubSub Engine
// returns number of bytes sent
// returns -1 if an error occured
int publish(char* topic, char* msg);