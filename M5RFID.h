#include "esphome.h"
#include <string>



UHF_RFID RFID;
CardpropertiesInfo card;
ManyInfo cards;

class M5RFID : public PollingComponent, public TextSensor {
	public:
		M5RFID() : PollingComponent(200) {}

	float get_setup_priority() const override { return esphome::setup_priority::LATE; }
	void setup() override {
    Serial.println("Starting setup");
    RFID._debug = 1;
    Serial2.begin(115200, SERIAL_8N1, 32, 26);
    //if (RFID._debug == 1) Serial1.begin(115200, SERIAL_8N1, 21, 22);
    //if (RFID._debug == 1) Serial1.begin(115200, SERIAL_8N1);

    // UHF_RFID set UHF_RFID设置
    Serial.println("Setup 1");
    //RFID.Set_transmission_Power(2600);
    Serial.println(RFID.Inquire_manufacturer());
    Serial.println("Setup 2");
    //RFID.Set_the_Select_mode();
    Serial.println("Setup 3");
    RFID.Delay(100);
    Serial.println("Setup 4");
    RFID.Readcallback();
    RFID.clean_data();

    // Prompted to connect to UHF_RFID 提示连接UHF_RFID
    Serial.println("Please connect UHF_RFID to Port C");

    // Determined whether to connect to UHF_RFID 判断是否连接UHF_RFID
    String soft_version;
    soft_version = RFID.Query_software_version();
    while (soft_version.indexOf("V2.3.5") == -1) {
        RFID.clean_data();
        RFID.Delay(150);
        RFID.Delay(150);
        soft_version = RFID.Query_software_version();
    }

    // The prompt will be RFID card close 提示将RFID卡靠近
    Serial.println("Please approach the RFID card you need to use");
	}
	void update() override {
		//publish_state(42.0);
		// Read one
    //  A read/write operation specifies a particular card
    //  读写操作需指定某一张卡 comd =
    //  RFID.Set_the_select_parameter_directive("30751FEB705C5904E3D50D70");
    //  Serial.println(comd);
    //  RFID.clean_data();

    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     Query the card information once 查询一次卡的信息例子
     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    card = RFID.A_single_poll_of_instructions();
    if (card._ERROR.length() != 0) {
        //Serial.println(card._ERROR);
    } else {
        if (card._EPC.length() == 24) {
            Serial.println("RSSI :" + card._RSSI + "," + card._EPC);
            //Serial.println("PC :" + card._PC);
            //Serial.println("EPC :" + card._EPC);
            //Serial.println("CRC :" + card._CRC);
            //Serial.println(" ");
            publish_state(card._EPC.c_str());
        }
    }
    RFID.clean_data();  // Empty the data after using it
                        // 使用完数据后要将数据清空


		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		 Read multiple RFID cards at once 一次读取多张RFID卡
		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		//Serial.println("POLLING\n");
		//cards = RFID.Multiple_polling_instructions(6);
		//Serial.println("POLLED\n");
		//for (size_t i = 0; i < cards.len; i++)
		//{
		//	if(cards.card[i]._EPC.length() == 24)
		//	{
		//		Serial.println("RSSI :" + cards.card[i]._RSSI);
		//		Serial.println("PC :" + cards.card[i]._PC);
		//		Serial.println("EPC :" + cards.card[i]._EPC);
		//		Serial.println("CRC :" + cards.card[i]._CRC);
		//	}
		//}
		//Serial.println("-------------");
		//RFID.clean_data();  // Empty the data after using it
		//this->publish_state(cards.len);
	}
};
