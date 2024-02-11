#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/time.h"
#include "pico/multicore.h"

// UART definitions
#define UART_ID uart0
#define BAUD_RATE 9600

// Configure LED pin
#define LED_PIN 25

// Pins GP16 and GP17 are being used for testing purposes with a pico;
// pins 0(TX) and 1(RX) will be used when this code is uploaded to the
// transceiver.
#define UART_TX_PIN 16
#define UART_RX_PIN 17

// Definitions used by the parse_nmea_sentence function
#define NMEA_BUF_SIZE 100
#define NMEA_SENTENCE_GPGGA "$GPGGA"

// Define the NMEA_data struct
typedef struct {
  double latitude;
  double longitude;
  int hour;
  int minute;
  int second;
} NMEA_data;

// NMEA_data parsing function which takes in an NMEA sentence and fills in the
// longitude, latitude, and time members of the NMEA_data struct.
NMEA_data parse_nmea_sentence(const char *sentence) {
  NMEA_data data = {0};

  if (strstr(sentence, NMEA_SENTENCE_GPGGA) == sentence) {
    char *p = (char *)sentence;

    // move pointer to time
    p = strchr(p, ',') + 1;

    // parse time
    int hour, minute, second;
    sscanf(p, "%2d%2d%2d", &hour, &minute, &second);
    data.hour = hour;
    data.minute = minute;
    data.second = second;

    // move pointer to latitude
    p = strchr(p, ',') + 1;

    // parse latitude
    double latitude, latitude_minutes;
    sscanf(p, "%2lf%lf", &latitude, &latitude_minutes);
    data.latitude = latitude + latitude_minutes / 60.0;

    // move pointer to N/S indicator
    p = strchr(p, ',') + 1;

    // check N/S indicator and adjust latitude
    if (*p == 'S') {
      data.latitude = -data.latitude;
    }

    // move pointer to longitude
    p = strchr(p, ',') + 1;

    // parse longitude
    double longitude, longitude_minutes;
    sscanf(p, "%3lf%lf", &longitude, &longitude_minutes);
    data.longitude = longitude + longitude_minutes / 60.0;

    // move pointer to E/W indicator
    p = strchr(p, ',') + 1;

    // check E/W indicator and adjust longitude
    if (*p == 'W') {
      data.longitude = -data.longitude;
    }
  }

  return data;
}



int main() {
  stdio_init_all();

  // Initialize nmea_sentence and NMEA_data
  char nmea_sentence[NMEA_BUF_SIZE];
  NMEA_data data;


  // Set up LED
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  // Set up UART with the required speed
  uart_init(UART_ID, BAUD_RATE);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);


  while (true) {

    // Read each of the 7 NMEA sentences printed from the the GPS module individually
    for (int q = 0; q < 7; q++) {
      scanf("%s", nmea_sentence);

      // The following 7 if/else if statements simulate the reading of NMEA data,
      // for testing purposes, when a signal is not available.
      //
      // if (q == 0) {
      //   strcpy(nmea_sentence, "$GPRMC,173843,A,3349.896,N,11808.521,W,000.0,360.0,230108,013.4,E*69");
      // }else if (q == 1) {
      //   strcpy(nmea_sentence, "$GPGGA,223856.00,4432.61522,N,12320.25521,W,1,06,1.79,103.8,M,-22.7,M,,*6F");
      // }else if (q == 2) {
      //   strcpy(nmea_sentence, "$GPGSV,2,1,08,01,05,005,80,02,05,050,80,03,05,095,80,04,05,140,80*7f");
      // }else if (q == 3) {
      //   strcpy(nmea_sentence, "$GPGSV,2,2,08,05,05,185,80,06,05,230,80,07,05,275,80,08,05,320,80*71");
      // }else if (q == 4) {
      //   strcpy(nmea_sentence, "$GPGSA,A,3,01,02,03,04,05,06,07,08,00,00,00,00,0.0,0.0,0.0*3a");
      // }else if (q == 5) {
      //   strcpy(nmea_sentence, "$GPRMC,111609.14,A,5001.27,N,3613.06,E,11.2,0.0,261206,0.0,E*50");
      // }else if (q == 6) {
      //   strcpy(nmea_sentence, "$GPVTG,217.5,T,208.8,M,000.00,N,000.01,K*4C");
      // }

      // Parse NMEA data
      data = parse_nmea_sentence(nmea_sentence);

      // Print NMEA data if latitude and longitude members are nonzero, otherwise print searching.
      if (data.latitude != 0 && data.longitude != 0) {
        printf("%lf", data.latitude);
        printf(",%lf", data.longitude);
        printf(",%02d", data.hour);
        printf(":%02d", data.minute);
        printf(":%02dUTC\n", data.second);
        // printf("%s\n", nmea_sentence);
      }else{
        // printf("%s\n", "  Searching...");
        // printf("%s\n", nmea_sentence);
      }
    }

    // sleep_ms(500);

    // Blink LED
    gpio_put(LED_PIN, 1);
    sleep_ms(250);
    gpio_put(LED_PIN, 0);
    sleep_ms(250);

    // Clear NMEA data from memory
    memset(&data, 0, sizeof(NMEA_data));


  }
  return 0;

}
