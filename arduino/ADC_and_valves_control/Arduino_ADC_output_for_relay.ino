int Sentor_P0 = A0; 

void setup() {
  // put your setup code here, to run once:
pinMode(2, OUTPUT); 
pinMode(13, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(2, HIGH); // sets the digital pin 13 on
 int P0 = analogRead(Sentor_P0);
  delay(1000);            // waits for a second
  digitalWrite(2, LOW);  // sets the digital pin 13 off
  //Serial.print("P0");

  Serial.println((String)+P0+",2000,");
/*  Serial.println("1234");
  Serial.println(P2);
  Serial.println(P3);
  Serial.println(P4);
  (String)"x:"+x+" y:"+y
*/
;
  // Serial.print("\n");
  delay(1000);
}
