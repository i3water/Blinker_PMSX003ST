#include "BLINKER_PMSX003ST.h"

void BLINKER_PMSX003ST::begin(Stream& s) {
    strem = &s;
}

void BLINKER_PMSX003ST::send(byte cmd,byte dataH,byte dataL) {
    Packet[2] = cmd;
    Packet[3] = dataH;
    Packet[4] = dataL;
    LRC = 0;
    foreach(Packet) {
        strem->write(row);
        LRC += row;
    }
    strem->write((LRC >> 8) & 0xff);
    strem->write(LRC & 0xff);
}

void BLINKER_PMSX003ST::sleep() {
    send(PMS_CMD_SLEEP, 0x00, PMS_CMD_SLEEP_SLEEP);
}

void BLINKER_PMSX003ST::wakeUp() {
    send(PMS_CMD_SLEEP, 0x00, PMS_CMD_SLEEP_WAKEUP);
}

void BLINKER_PMSX003ST::setMode(byte mode) {
    send(PMS_CMD_MODE,0x00,mode);
    while(strem->available() > 0) {
        strem->read();
    }
}

void BLINKER_PMSX003ST::request() {
    send(PMS_CMD_READ,0x00,0x00);
}

int BLINKER_PMSX003ST::read(unsigned long timeout) {
    static unsigned long start;
    static int cnt;
    static unsigned int check;
    cnt = 0;
    check = 0;
    start = millis();

    while (strem->available() < 40) {
        if (millis() - start > timeout) {
            return false;
        }
    }

    while (strem->available() > 0) {
        byte c = strem->read();
        buffer[39-(cnt)] = c;
        if (cnt < 38) {
            check += (unsigned int)c;
        }
        cnt++;
        cnt = cnt % 40;
    }

    if ((buffer[39] << 8 | buffer[38]) != 0x424D ||
        (buffer[1] << 8 | buffer[0]) != check) {
        return false;
    }

    for (uint8_t num = 0; num < 20; num++) {
        data[num] = buffer[num*2 + 1] << 8 | buffer[num*2];
    }

    PM25 = getPmAto(2.5);
    PM100 = getPmAto(10);
    parseAQI();

    return true;
}

double BLINKER_PMSX003ST::getHumi() {
    return data[DATA_HUMI]/10.0;
}

double BLINKER_PMSX003ST::getTemp() {
    return data[DATA_TEMP]/10.0;
}

double BLINKER_PMSX003ST::getForm() {
    return data[DATA_FORMALDE]/1000.0;
}

unsigned int BLINKER_PMSX003ST::getPcs(double pcs) {
    switch((int)(pcs*10)){
        case 100: return data[DATA_PCS100ug];
        case 50 : return data[DATA_PCS50ug];
        case 25 : return data[DATA_PCS25ug];
        case 10 : return data[DATA_PCS10ug];
        case 5  : return data[DATA_PCS05ug];
        case 3  : return data[DATA_PCS03ug];
    }
    return 0;
}

unsigned int BLINKER_PMSX003ST::getPmAto(double pm){
    switch((int)(pm*10)){
        case 100: return data[DATA_PM100ATO];
        case 25 : return data[DATA_PM25ATO];
        case 10 : return data[DATA_PM10ATO];
    }
    return 0;
}

unsigned int BLINKER_PMSX003ST::getPmCf1(double pm){
    switch((int)(pm*10)){
        case 100: return data[DATA_PM100CF1];
        case 25 : return data[DATA_PM25CF1];
        case 10 : return data[DATA_PM10CF1];
    }
    return 0;
}

void BLINKER_PMSX003ST::parseAQI() {
    uint16_t AQI25, AQI100, color;
    for (uint8_t Bnum = 0; Bnum < 2; Bnum++) {
    // uint8_t Bnum = 0;
        AQI25 = 0;
        AQI100 = 0;
        for (uint8_t Inum = 1; Inum < 8; Inum++) {
            if (PM25*10 <= AQIindex[Inum][0+Bnum]) {
                // IOT_DEBUG_PRINT4(F("Inum: "), Inum, F("Bnum: "), Bnum);
                // IOT_DEBUG_PRINT2(F("AQIindex[Inum][0+Bnum]: "), AQIindex[Inum][0+Bnum]);
                AQI25 = ((AQIindex[Inum][4] - AQIindex[Inum-1][4])*(PM25*10 - AQIindex[Inum-1][0+Bnum]) / (AQIindex[Inum][0+Bnum] - AQIindex[Inum - 1][0+Bnum]) + AQIindex[Inum-1][4])/10;
                color = AQIindex[Inum][5];
                break;
            }
            if (Inum == 7) {
                AQI25 = 500;
                color = 5;
            }
        }
        for (uint8_t Inum = 1; Inum < 8; Inum++) {
            if (PM100*10 <= AQIindex[Inum][2+Bnum]) {
                // IOT_DEBUG_PRINT4(F("Inum: "), Inum, F("Bnum: "), Bnum);
                // IOT_DEBUG_PRINT2(F("AQIindex[Inum][0+Bnum]: "), AQIindex[Inum][0+Bnum]);
                AQI100 = ((AQIindex[Inum][4] - AQIindex[Inum-1][4])*(PM100*10 - AQIindex[Inum-1][2+Bnum]) / (AQIindex[Inum][2+Bnum] - AQIindex[Inum - 1][2+Bnum]) + AQIindex[Inum-1][4])/10;
                
                // IOT_DEBUG_PRINT4(F("AQI25: "), AQI25, F("  AQI100: "), AQI100);
                if(AQI25 >= AQI100) {
                    // return String(AQI25);
                    AQIBUFFER[Bnum][AQI_DATA] = AQI25;
                    AQIBUFFER[Bnum][AQI_LEVEL] = color;
                    AQIBUFFER[Bnum][AQI_MAIN_POLLUTANT] = POLLUTANT_PM2_5;
                    break;
                }
                else {
                    AQIBUFFER[Bnum][AQI_DATA] = AQI100;
                    color = AQIindex[Inum][5];
                    AQIBUFFER[Bnum][AQI_LEVEL] = color;
                    AQIBUFFER[Bnum][AQI_MAIN_POLLUTANT] = POLLUTANT_PM10;
                    break;
                    // return String(AQI100);
                }
            }
            if (Inum == 7) {
                AQIBUFFER[Bnum][0] = 500;
                AQIBUFFER[Bnum][1] = 5;
            }
        }
    }
}