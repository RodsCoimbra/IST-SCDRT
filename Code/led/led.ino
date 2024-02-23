const int LED_PIN = 15;
const int DAC_RANGE = 4096;
int counter = 0;
const float vcc = 3.3;
const float adc_conv = 4096/vcc;
const float offset_R_Lux = log10(225000)+0.8;
void setup() {// the setup function runs once
Serial.begin(115200);
analogReadResolution(12);//default is 10
analogWriteFreq(60000);//60KHz, about max
analogWriteRange(DAC_RANGE); //100% duty cycle
}
void loop() {// the loop function runs cyclically
int read_adc;
float LDR_volt;
float LDR_resistance;
float Lux;
analogWrite(LED_PIN, 0); // set led PWM
delay(1);
//delay 1ms
read_adc = analogRead(A0); // read analog voltage
LDR_volt = read_adc/adc_conv;
LDR_resistance = (vcc * 10000)/LDR_volt - 10000;
Lux = pow(10,(log10(LDR_resistance) - offset_R_Lux)/(-0.8));
counter = counter + 1;
if (counter > DAC_RANGE) // if counter saturates
counter = 0;


// reset counter
//format that Serial Plotter likes
Serial.print(0); Serial.print(" ");
Serial.print(Lux); Serial.print(" ");
//Serial.print(read_adc); Serial.print(" ");
Serial.print(1000); 
Serial.println();
//Serial.printf("%f\n", log10(read_adc));
}