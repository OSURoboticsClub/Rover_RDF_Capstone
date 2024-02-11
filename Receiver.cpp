#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "LoRa-RP2040.h"

void print_if_valid(const char *str) {
    int len = strlen(str);
    int i;
    int has_extra_character = 0;

    for (i = 0; i < len; i++) {
        if (str[i] != '0' && str[i] != '1' && str[i] != '2' && str[i] != '3' && str[i] != '4' && str[i] != '5' && str[i] != '6' && str[i] != '7' && str[i] != '8' && str[i] != '9' && str[i] != ',' && str[i] != ':' && str[i] != '-' && str[i] != '.' && str[i] != 'U' && str[i] != 'T' && str[i] != 'C') {
            has_extra_character += 1;
        }
    }
    if (!has_extra_character) {
        printf("%s\n", str);
    }
    has_extra_character = 0;
}


int main(){
	stdio_init_all();

	if (!LoRa.begin(915E6)){
		printf("Starting LoRa failed!\n");
		while(1){
			printf("Failed!\n");
		}
	}

	printf("LoRa Started\n");

	LoRa.receive();

	int counter=0;

	while(1){
		// printf("Listening...\n");

		int packetSize = LoRa.parsePacket();

		if(packetSize > 0){
			// printf("Packet Size: %i \n", packetSize);
			string message = "";
			while(LoRa.available()){
    			message += (char)LoRa.read();
  				}
			// printf("Node Receive: %s\n", message.c_str());

			print_if_valid(message.c_str());

			// printf("%s\n", message.c_str());
		}
	}


 return 1;
}
