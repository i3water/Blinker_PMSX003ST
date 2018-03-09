#include "BLINKER_PMSX003ST.h"

#if defined(ESP32)
HardwareSerial pmsSerial(2);// UART1/Serial1 pins 16,17
#else
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(4,5);
#endif

BLINKER_PMSX003ST pms;

void setup() {
    Serial.begin(115200);
    Serial.println("\nStart");

    pmsSerial.begin(9600);
    pms.begin(pmsSerial);
    //  pms.wakeUp();
    pms.setMode(PASSIVE);
}

void loop() {
    pms.request();
    if(!pms.read()){
        return;
    }
    Serial.print("PM1.0(CF1)\t");
    Serial.print(pms.getPmCf1(1.0));
    Serial.println("ug/m3");
    Serial.print("PM2.5(CF1)\t");
    Serial.print(pms.getPmCf1(2.5));
    Serial.println("ug/m3");
    Serial.print("PM10(CF1)\t");
    Serial.print(pms.getPmCf1(10));
    Serial.println("ug/m3");
    Serial.print("PM1.0(ATO)\t");
    Serial.print(pms.getPmAto(1.0));
    Serial.println("ug/m3");
    Serial.print("PM2.5(ATO)\t");
    Serial.print(pms.getPmAto(2.5));
    Serial.println("ug/m3");
    Serial.print("PM10(ATO)\t");
    Serial.print(pms.getPmAto(10));
    Serial.println("ug/m3");
    Serial.print("  PCS0.3\t");
    Serial.print(pms.getPcs(0.3));
    Serial.println("pcs/0.1L");
    Serial.print("  PCS0.5\t");
    Serial.print(pms.getPcs(0.5));
    Serial.println("pcs/0.1L");
    Serial.print("  PCS1.0\t");
    Serial.print(pms.getPcs(1));
    Serial.println("pcs/0.1L");
    Serial.print("  PCS2.5\t");
    Serial.print(pms.getPcs(2.5));
    Serial.println("pcs/0.1L");
    Serial.print("  PCS5.0\t");
    Serial.print(pms.getPcs(5));
    Serial.println("pcs/0.1L");
    Serial.print("   PCS10\t");
    Serial.print(pms.getPcs(10));
    Serial.println("pcs/0.1L");
    Serial.print("Formalde\t");
    Serial.print(pms.getForm());
    Serial.println("ug/m3");
    Serial.print("Temperat\t");
    Serial.print(pms.getTemp());
    Serial.println("'C");
    Serial.print("Humidity\t");
    Serial.print(pms.getHumi());
    Serial.println("%");
    Serial.println();
    delay(1000);
}
