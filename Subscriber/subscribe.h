#pragma once

// send topic to PubSub Engine
// returns number of bytes sent
// returns -1 if an error occured
int subscribe(char* topic);