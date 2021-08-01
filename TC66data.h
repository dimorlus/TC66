//---------------------------------------------------------------------------

#ifndef TC66dataH
#define TC66dataH
#pragma pack(1)
#define DATASIZE 192
typedef struct
{
 union
  {
   struct
    {
     /*000*/    char pac1[4];  //"pac1"
     /*004*/    char model[4]; //"TC66"
     /*008*/    char ver[4];   //"1.15"
     /*012*/    uint32_t sn;
     /*016*/    uint32_t unknown1[7];
     /*044*/    uint32_t runs;
     /*048*/    uint32_t Volts; // /10000.0 V
     /*052*/    uint32_t Amps;  // /100000.0 A
     /*056*/    uint32_t Watts; // /10000.0 W
     /*060*/    uint32_t chk1;
     /*064*/    char pac2[4];  //"pac2"
     /*068*/    uint32_t Ohms; // /10.0 Ohm
     /*072*/    uint32_t mAh0; //mAh
     /*076*/    uint32_t mWh0; //mWh
     /*080*/    uint32_t mAh1; //mAh
     /*084*/    uint32_t mWh1; //mWh
     /*088*/    uint32_t Tsign;
     /*092*/    uint32_t Tmp;  // 'C
     /*096*/    uint32_t DpVolts; // /100.0 V
     /*100*/    uint32_t DmVolts; // /100.0 V
     /*104*/    uint8_t unknown2[20];
     /*124*/    uint32_t chk2;
     /*128*/    char pac3[4]; //"pac3"
     /*132*/    uint8_t unknown3[56]; //zero
     /*188*/    uint32_t chk3;
    };
   uint8_t raw[DATASIZE];
  };
} tTC66Data; //192
//---------------------------------------------------------------------------
#endif
