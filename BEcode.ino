//Código disponivél em bit.ly/BEcode

#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define col 20 // Serve para definir o numero de colunas do display utilizado
#define lin  4 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Serve para definir o endereço do display.

LiquidCrystal_I2C lcd(ende,col,lin); 
char URL[] = "bit.ly/BEcode";


// calibração dos transdutores (funções no fim do código)
struct calib{ 
  float cmax;
  float cmin;
};

float volt_adc_max = 6.144; //Valor retirado da documentação do github
int int_adc_max = 32767; //16 bits signed
calib calib_q;
calib calib_pa;
calib calib_pc; 



Adafruit_ADS1115 ads;
int16_t adc0, adc1, adc2; //caudal,Pasp e Pcomp vindo do adc



const int numQ = 10, numP = 10, numR = 50;
float Q = 0, Pasp = 0, Pcomp=0;
float AQ[numQ], APa[numP], APc[numP], Arpm[numR];//arrays de registo
int iQ, iP,iR; //índice dos arrays
float Qout,PAout,PCout, Rpmout;
long ti,tf;
int DT = 100,rpm =0;
int Enviar = 12;

void INTERRUPT()
{
  tf=micros();
  if (tf-ti>1000000/60){
  rpm=60000000./(tf-ti);
  ti=tf;
  }
}

float media(float A[], int num)
{
  float soma =0;
  for (int x=0; x<num; x++)
  {
    soma += A[x];
  }
  return (soma/float(num));
}

void setup() {
  Serial.begin(19200);
  ads.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Circuito e codigo");
  lcd.setCursor(0,1);
  lcd.print("de:");
  lcd.setCursor(2,2);
  lcd.print("Bernardo Santos");
  lcd.setCursor(7,3);
  lcd.print("em 2023");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Baseado no trabalho");
  lcd.setCursor(0,1);
  lcd.print("de:");
  lcd.setCursor(3,2);
  lcd.print("Tiago Barreto");
  lcd.setCursor(7,3);
  lcd.print("em 2017");
  delay(1500);
  lcd.clear();
  lcd.setCursor(4,1);
  lcd.print("Codigo em:");
  lcd.setCursor(3,2);
  lcd.print(URL);
  delay(2000);
  lcd.clear();
  
  ti = millis();
  pinMode(Enviar, INPUT);
  pinMode(2,INPUT_PULLUP);
  calib_q = calibrar_caudal(10000,10000); //resistências do divisor de tensão (R3,R4) em ohms
  calib_pa = calibrar_pressao(300);
  calib_pc = calibrar_pressao(300);


  attachInterrupt(digitalPinToInterrupt(2), INTERRUPT, FALLING);
  }



void loop() {
  if (millis()-tf>500){rpm=0;}
  
  delay(DT);

// Lê o ADC

  adc0 = ads.readADC_SingleEnded(0);if (adc0<0){adc0=0;}
  adc1 = ads.readADC_SingleEnded(1);if (adc1<0){adc1=0;};
  adc2 = ads.readADC_SingleEnded(2);if (adc2<0){adc2=0;}


  Q = mapfloat(adc0,0,calib_q.cmax,0,300);// l/min
  if (Q<5){Q=0;} 
  Pasp = mapfloat(adc1,calib_pa.cmin,calib_pa.cmax,-10,90); // mca
  Pcomp = mapfloat(adc2,calib_pc.cmin,calib_pc.cmax,-10,90);// mca

  
  AQ[iQ]=Q; APa[iP]=Pasp; APc[iP]=Pcomp;Arpm[iR]=rpm;
  iQ++; iP++;iR++;
  if (iQ==numQ){iQ=0;} if (iP==numP){iP=0;} if (iR==numR){iR=0;}

  

  Qout = media(AQ,numQ);
  PAout = media(APa,numP);
  PCout = media(APc,numP);
  Rpmout = media(Arpm,numR);



  
  //Output
  if (digitalRead(Enviar) == HIGH){

    
    Serial.print("Caudal: ");Serial.print(Qout);Serial.println(" l/min");
    Serial.print("Pasp: ");Serial.print(PAout);Serial.println(" mca");
    Serial.print("Pcomp: ");Serial.print(PCout);Serial.println(" mca");
    Serial.print("RPM: ");Serial.print(rpm);Serial.println(" rpm");
    Serial.println("----------------------------");
    
    /*Serial.print(Qout);Serial.print(" ");
    Serial.print(PAout);Serial.print(" ");
    Serial.print(PCout);Serial.print(" ");
    Serial.print(rpm);Serial.println(" ");
    */
  }

  lcd.setCursor(0,0);
  lcd.print("Q:");
  lcd.setCursor(5, 0);
  lcd.print(Qout);
  lcd.print("   ");
  lcd.setCursor(12,0);
  lcd.print ("L/min");

  lcd.setCursor(0,1);
  lcd.print("PA:");
  lcd.setCursor(5, 1);
  lcd.print(PAout);
  lcd.print("   ");
  lcd.setCursor(12,1);
  lcd.print ("mca"); 

  lcd.setCursor(0,2);
  lcd.print("PC:");
  lcd.setCursor(5, 2);
  lcd.print(PCout);
  lcd.print("  ");
  lcd.setCursor(12,2);
  lcd.print ("mca");

  lcd.setCursor(0,3);
  lcd.print("RPM:");
  lcd.setCursor(5,3); 
  lcd.print(rpm);
  lcd.print("     ");
  lcd.setCursor(12,3);
  lcd.print ("RPM");

}


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


struct calib calibrar_caudal(float div_tensao1, float div_tensao2){
  float Volt_div = volt_div(10,div_tensao1,div_tensao2);
  calib ins;
  ins.cmax = ajustar_ao_adc(Volt_div);
  ins.cmin = ajustar_ao_adc(0);
  return ins;
}

struct calib calibrar_pressao(float R_P) {
  
  calib ins;
  ins.cmax = ajustar_ao_adc(0.02*R_P);  //20mA * Resistencia
  ins.cmin = ajustar_ao_adc(0.004*R_P); //4mA * Resistência
  
  return  ins;
}

float volt_div(float volt, float div_tensao1, float div_tensao2){
  return volt * div_tensao2 / (div_tensao1 + div_tensao2);
}

float ajustar_ao_adc(float in){
  return  in / volt_adc_max * int_adc_max;
}