#include <Arduino.h>
#include <LoRa.h>

#define MAX_PACKET_SIZE 100

uint8_t loraRxBuffer[MAX_PACKET_SIZE], serialRxBuffer[MAX_PACKET_SIZE];
uint8_t loraRxBytes, serialRxBytes;
int availableLoraBytes = 0;

void onReceive(int bytes);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);

	LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);
	LoRa.begin(868E6);
	LoRa.sleep();
	LoRa.enableCrc();
    LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
    LoRa.setSignalBandwidth(250E3);
    LoRa.setCodingRate4(6);
    LoRa.idle();
	LoRa.onReceive(onReceive);
	LoRa.receive();
}

void loop() {
	if((availableLoraBytes > 0) && (availableLoraBytes < MAX_PACKET_SIZE))
	{
		loraRxBytes = LoRa.readBytes(loraRxBuffer, availableLoraBytes);
		Serial.print("RSSI: "); Serial.print(157 - constrain(LoRa.packetRssi() * -1, 0, 157));
		Serial.print(" SNR: "); Serial.println(LoRa.packetSnr());
		Serial.write(loraRxBuffer, loraRxBytes);
		availableLoraBytes = 0;
	}

	if(Serial.available())
	{
		serialRxBytes = Serial.readBytes(serialRxBuffer, Serial.available());
		LoRa.beginPacket();
		LoRa.write(serialRxBuffer, serialRxBytes);
		LoRa.endPacket();
		// LoRa.sleep();
		LoRa.receive();
	}
}

void onReceive(int bytes)
{
	availableLoraBytes = bytes;
}