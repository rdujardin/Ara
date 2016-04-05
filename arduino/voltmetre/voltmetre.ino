float valeur;
int mesure;
  //R1=97.9 kOhms 
  //R2=98.8 kOhms
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initialisation");
  delay(500);
}

void loop() {
  mesure = analogRead(0); 
  valeur = mesure / (255/5); //Entrée analogique 0-255 sortie 0-5V
  valeur = valeur * 0.50229 ; //Valeur = mesure * R2 / (R1+R2)
  Serial.println(valeur); 
}
