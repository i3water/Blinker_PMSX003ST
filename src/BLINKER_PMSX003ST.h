#ifndef BLINKER_PMSX003ST_H
#define BLINKER_PMSX003ST_H

#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

#define foreach(x)  for(int i=0,row = x[i],size = sizeof(x)/sizeof(x[0]);i < size;row=x[++i])

#define PMS_HEAD1               0x42
#define PMS_HEAD2               0x4d



/* CMD parameters */
#define PMS_CMD_READ            0xe2
//only works in passive mode

#define PMS_CMD_MODE            0xe1
#define PMS_CMD_MODE_PASSIVE    0x00
#define PASSIVE                 0x00
#define PMS_CMD_MODE_ACTIVE     0x01
#define ACTIVE                  0x01
#define PMS_CMD_SLEEP           0xe4
#define PMS_CMD_SLEEP_SLEEP     0x00
#define PMS_CMD_SLEEP_WAKEUP    0x01

/* receive packet prameters */

#define DATA_HUMI               3
#define DATA_TEMP               4
#define DATA_FORMALDE           5
#define DATA_PCS100ug           6
#define DATA_PCS50ug            7
#define DATA_PCS25ug            8
#define DATA_PCS10ug            9
#define DATA_PCS05ug            10
#define DATA_PCS03ug            11
#define DATA_PM100ATO           12
#define DATA_PM25ATO            13
#define DATA_PM10ATO            14
#define DATA_PM100CF1           15
#define DATA_PM25CF1            16
#define DATA_PM10CF1            17

#define AQI_BASE_US             0
#define AQI_BASE_CN             1

#define AQI_DATA                0
#define AQI_LEVEL               1
#define AQI_MAIN_POLLUTANT      2

#define POLLUTANT_PM2_5         0
#define POLLUTANT_PM10          1
/* class */

class BLINKER_PMSX003ST {
    public:
        unsigned int data[20];

        //Setup
        void begin(Stream& s);
        void sleep();
        void wakeUp();
        void setMode(byte);
        void request();
        int read(unsigned long timeout = 900);

        //Get the parameters
        double getHumi();
        double getTemp();
        double getForm();
        unsigned int getPcs(double);    //available to 10, 5, 2.5, 1, 0.5, 0.3
        unsigned int getPmAto(double);  //availalbe to 10, 2.5, 1
        unsigned int getPmCf1(double);  //availalbe to 10, 2.5, 1

        uint16_t getAQI(uint8_t _base) {
            if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
                return AQIBUFFER[_base][AQI_DATA];
            }
            else {
                return AQIBUFFER[AQI_BASE_US][AQI_DATA];
            }
        }
        
        uint8_t getAQILevel(uint8_t _base) {
            if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
                return AQIBUFFER[_base][AQI_LEVEL];
            }
            else {
                return AQIBUFFER[AQI_BASE_US][AQI_LEVEL];
            }
        }

        String getMainPollu(uint8_t _base) {
            if (_base >= AQI_BASE_US && _base <= AQI_BASE_CN) {
                return AQIBUFFER[_base][AQI_MAIN_POLLUTANT] ? "PM10" : "PM2.5";
            }
            else {
                return AQIBUFFER[AQI_BASE_US][AQI_MAIN_POLLUTANT] ? "PM10" : "PM2.5";
            }
        }

    private:
        byte Packet[5] = {0x42, 0x4d, 0,};
        byte buffer[40];

        Stream * strem;
        unsigned int LRC = 0;
        void send(byte, byte, byte);

        uint8_t  AQI_BASE = AQI_BASE_US;
        uint16_t PM25, PM100;

        uint16_t AQIindex[8][6] = {
            {0,     0,      0,      0,      0,      0},
            {120,   350,    540,    500,    500,    0},//35 50
            {354,   750,    1540,   1500,   1000,   1},//75 150
            {554,   1150,   2540,   2500,   1500,   2},//115 250
            {1504,  1500,   3540,   3500,   2000,   3},//150 350
            {2504,  2500,   4240,   4200,   3000,   4},//250 420
            {3504,  3500,   5040,   5000,   4000,   5},//350 500
            {5004,  5000,   6040,   6000,   5000,   5}//500 600C I
        };

        uint16_t AQIBUFFER[2][3] = {{0,0,0},{0,0,0}};

        void parseAQI();
};
#endif
