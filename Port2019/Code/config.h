#ifndef CONFIG_H
#define CONFIG_H

// piny komponentów
#define  PIN_MQ9 A0;
#define  PIN_BAT A2;
#define  PIN_PHR A1;
#define  PIN_LED 13;
#define  PIN_DHT 3;
#define  PIN_SD 11;

// konfiguracja powiadomień
#define  LOG_SD 1;
#define  LOG_SERIAL 1;
#define  WAIT_SERIAL 1;

// precyzja zmiennych podczas wysyłania (kompresji) i pisania na USB
#define  PREC_LAT 7;
#define  PREC_LON 7;
#define  PREC_ALT 1;
#define  PREC_PRE 4;
#define  PREC_TEM 2;
#define  PREC_SPS 1;
#define  PREC_HUM 1;
#define  PREC_MPU 7;

// minimalne przewidywane odczyty (sensory wysyłane przez radio)
#define  MIN_SNU 0.0f;
#define  MIN_LAT 49.0f;
#define  MIN_LON 14.07f;
#define  MIN_ALT 0.0f;
#define  MIN_PRE 600.0f;
#define  MIN_TEM -10.0f;
#define  MIN_AIR 0.0f;
#define  MIN_SPS 0.0f;
#define  MIN_HUM 0.0f;
#define  MIN_BAT 0.0f;
	
// maksymalne przewidywane odczyty (sensory wysyłane przez radio)
#define  MAX_SNU 128.0f;
#define  MAX_LAT 52.0f;
#define  MAX_LON 24.09f;
#define  MAX_ALT 6553.0f;
#define  MAX_PRE 1100.0f;
#define  MAX_TEM 50.0f;
#define  MAX_AIR 4096.0f;
#define  MAX_SPS 102.4f;
#define  MAX_HUM 100.0f;
#define  MAX_BAT 4096.0f;
	
// czas między pomiarami, 0 oznacza najmniejszy możliwy
#define  UPD_GPS 0;
#define  UPD_BMP 0;
#define  UPD_MQ9 30;
#define  UPD_SPS 0;
#define  UPD_DHT 0;
#define  UPD_MPU 0;
#define  UPD_BAT 5000;
#define  UPD_PHR 30;

#define  DEL_BEFTRAN 0;
#define  DEL_BETWEENTRAN 0;
#define  DEL_AFTTRAN 0;
#define  DEL_BEFSAVE 0;
#define  DEL_BETWEENSAVE 5000;
#define  DEL_AFTSAVE 0;
#define  DEL_SAVETRAN 30;
	
// opóźnienie pierwszego odczytu (ms)
#define  DEL_SPS 10000;
#define  DEL_DHT 1000;
	
#define  STA_PREINIT 0;
#define  STA_DURINGINIT 1;
#define  STA_INITIALIZED 2;
#define  STA_MISSING 3;
#define  STA_ERROR 4;
#define  STA_DISABLED 5;
#define  STA_PHANTOM 6;

#define  MODE_TX 0;
#define  MODE_RX 1;
	
#endif
