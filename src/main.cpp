#include <Arduino.h>
#include <LoRa.h>

#define MAX_PACKET_SIZE 100

uint8_t loraRxBuffer[MAX_PACKET_SIZE], serialRxBuffer[MAX_PACKET_SIZE];
uint8_t loraRxBytes, serialRxBytes;
int availableLoraBytes = 0;

uint32_t nextTaskAutoTransmit = 0;
#define TASK_AUTO_TRANSMIT_RATE (500)

uint32_t lastLoraReceived = 0;
#define LORA_RX_TIMEOUT (2000)

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

	pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
	if((availableLoraBytes > 0) && (availableLoraBytes < MAX_PACKET_SIZE))
	{
		loraRxBytes = LoRa.readBytes(loraRxBuffer, availableLoraBytes);
		Serial.print("RSSI: "); Serial.print(157 - constrain(LoRa.packetRssi() * -1, 0, 157));
		Serial.print(" SNR: "); Serial.println(LoRa.packetSnr());
		Serial.write(loraRxBuffer, loraRxBytes);
		Serial.println();
		availableLoraBytes = 0;
	}

	if(Serial.available() || serialRxBytes)
	{
		if(serialRxBytes == 0)
		{
			serialRxBytes = Serial.readBytes(serialRxBuffer, Serial.available());
		}
		LoRa.beginPacket();
		LoRa.write(serialRxBuffer, serialRxBytes);
		LoRa.endPacket();
		serialRxBytes = 0;
		// LoRa.sleep();
		LoRa.receive();
	}

	if(nextTaskAutoTransmit < millis())
	{
		sprintf((char *) serialRxBuffer, "ping");
		serialRxBytes = 4;
		nextTaskAutoTransmit = millis() + TASK_AUTO_TRANSMIT_RATE;
	}

	if((lastLoraReceived + LORA_RX_TIMEOUT) > millis())
	{
		//timeout
		digitalWrite(BUILTIN_LED, HIGH);
	}
	else
	{
		//correct receiving
		digitalWrite(BUILTIN_LED, LOW);
	}
}

void onReceive(int bytes)
{
	availableLoraBytes = bytes;
	lastLoraReceived = millis();
}