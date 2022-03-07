// Définition des pins de débit mètre
#define debitMetreRigide 2
#define debitMetreSoupleIn 18
#define debitMetreSoupleOut 19
 
// Pins pour le calcule du volume rigide
#define echoEmit 7
#define echoGet 8
 
// Pins de contrôle du flux
#define versSouple 11
#define versRigide 12
 
#define LED 13
 
// Variable globale des paramètres
int mmDiametre;
int mmLongueur;
float volumeRigideMax;
 
float volumeSouple;
float volumeSoupleMax;
 
long pulseCountRigide;
long pulseCountSouple;
 
unsigned long oldTime;
 
float delai;
 
bool go = false;
 
void pulseCounterRigide()
{
  // A chaque pulsation, on incrémente le compte de pulsation compté, le débitmètre augmente ça fréquence quand le débit augmente
  pulseCountRigide++;
}
 
void entreeSouple(){
  pulseCountSouple++;
}
 
void sortieSouple(){
  pulseCountSouple--;
}
 
float calculVolume(int mmDiametre, int mmLongueur, float mmAir) { // Permet d'appliquer les formules de mathématiques afin de calculer le volume de l'eau dans les cuves rigide en foncion de la distance de l'air, et du volume de la cuve vide
  float mmRayon = mmDiametre / 2;
  float mmHauteur = mmDiametre - mmAir + 20;
  float aire;
  if ((mmHauteur >= mmRayon) && (mmHauteur < mmDiametre))
  {
    aire = (3.14159265359 * (mmRayon * mmRayon)) - (2 * ((0.25 * (3.14159265359 * (mmRayon * mmRayon)) - (atan(((mmRayon - ((2 * mmRayon) - mmHauteur)) / mmRayon) / sqrt(1 - ((mmRayon - ((2 * mmRayon) - mmHauteur)) / mmRayon) * ((mmRayon - ((2 * mmRayon) - mmHauteur)) / mmRayon)))) * (3.14159265359 * (mmRayon * mmRayon)) / (3.14159265359 * 2)) - 0.5 * (mmRayon - ((2 * mmRayon) - mmHauteur)) * sqrt(2 * mmRayon * ((2 * mmRayon) - mmHauteur) - ((2 * mmRayon) - mmHauteur) * ((2 * mmRayon) - mmHauteur))));
  }
  else if (mmHauteur == 0)
  {
    aire = 0;
  }
  else if (mmHauteur == mmDiametre)
  {
    aire = 3.14159265359 * (mmRayon * mmRayon);
  }
  else if ((mmHauteur > 0) && (mmHauteur < mmRayon))
  {
    aire = 2 * ((0.25 * (3.14159265359 * (mmRayon * mmRayon)) - (atan(((mmRayon - mmHauteur) / mmRayon) / sqrt(1 - ((mmRayon - mmHauteur) / mmRayon) * ((mmRayon - mmHauteur) / mmRayon)))) * (3.14159265359 * (mmRayon * mmRayon)) / (3.14159265359 * 2)) - 0.5 * (mmRayon - mmHauteur) * sqrt(2 * mmRayon * mmHauteur - mmHauteur * mmHauteur));
  }
  return aire * mmLongueur * 0.001;
 
}
 
String getValue(String data, char separator, int index)
{ // Équivalent au data.split(seperator)[index]
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
 
void serialEvent(){ // Si il y a de l'activité sur le port série
  String data = Serial.readStringUntil('\n'); // On lit jusqu'au "\n"
  //Serial.println(data);
  if (data.startsWith("update")){ // Si c'est pour mettre à jour les valeurs, on récupère les nouvelles valeurs
    mmDiametre = getValue(data,';',1).toInt();
    mmLongueur = getValue(data,';',2).toInt();
    volumeSoupleMax = getValue(data,';',3).toFloat();
  } else if (data.startsWith("last")){
    volumeSouple = getValue(data,';',1).toInt();
  }
}
 
float mmAirCuveRigide() { // [2cm;4m] distance max
  // On veut lire le niveau d'eau
  digitalWrite(echoEmit, HIGH);
  delayMicroseconds(10);
  //delay(1);
  digitalWrite(echoEmit, LOW);
  unsigned long temps = pulseIn(echoGet, HIGH);
 
  // Calcule de la distance en mètre: (d = v * t) (v en mm/µs) (t en µs)
  return temps / 2 * 0.343;
}
 
long pulseToFlowInML(long data){
  float tmp = (delai / (millis() - oldTime)) * data; // On corrige le temps en cas d'imprécision
  float flowRate;
  if (tmp != 0) { // Si le temps n'est 0, on fait le calcule du débit d'eau
    flowRate = tmp / 6.5163 + 2.7;
  } else {
    flowRate = 0;
  }
   return (flowRate / 60) * 1000;
}
 
void setup() {
  Serial.begin(9600); // On ouvre le port série à 9600 bauds
  // On définit le mode de sortie des pins
  pinMode(debitMetreSoupleIn, INPUT);
  pinMode(debitMetreSoupleOut, INPUT);
  pinMode(debitMetreRigide, INPUT);
  pinMode(echoEmit, OUTPUT);
  pinMode(echoGet, INPUT);
  pinMode(versSouple, OUTPUT);
  pinMode(versRigide, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(versSouple, LOW);
  digitalWrite(versRigide, LOW);
  digitalWrite(debitMetreRigide, HIGH);
  digitalWrite(echoEmit, LOW);
  
  // On initialise les paramètres par défaut
  mmDiametre = -1;
  mmLongueur = -1;
  volumeSouple = -1;
  volumeSoupleMax = -1;
  volumeRigideMax = -1;
 
  pulseCountRigide = 0;
  pulseCountSouple = 0;
 
  delai = 1000.0;
  oldTime = millis();
 
  // On commence à compter le débit
  attachInterrupt(digitalPinToInterrupt(debitMetreRigide), pulseCounterRigide, FALLING);
  attachInterrupt(digitalPinToInterrupt(debitMetreSoupleIn), entreeSouple, FALLING);
  attachInterrupt(digitalPinToInterrupt(debitMetreSoupleOut), sortieSouple, FALLING);
}
 
void loop() {
  if (((millis() - oldTime) > delai) && (go != true)){ // Tant qu'on est pas prêt, on ne fait rien
    if (volumeSouple == -1){ // On obitient le volume de la cuve souple de la base de données
      Serial.print("get;\n");
    } else if ((mmDiametre == -1) || (mmLongueur == -1) || (volumeSoupleMax == -1)){ // On obtient les propriété de la base de données
      Serial.print("prop;\n");
    } else if (volumeRigideMax == -1){ // On calcule le volume maximum de la cuve rigide
      volumeRigideMax = calculVolume(mmDiametre, mmLongueur, 20);
    } else { // Si on a toute les informations, on est prêt
      digitalWrite(LED, LOW);
      go = true;
    }
    oldTime = millis();
  }
  
  if (((millis() - oldTime) > delai) && (go == true)) { // Toute les 1s, on mets à jour les données
    detachInterrupt(debitMetreRigide); // On arrête de compter pendant les calculs
    detachInterrupt(debitMetreSoupleIn);
    detachInterrupt(debitMetreSoupleOut);
 
    long fluxSouple = pulseToFlowInML(pulseCountSouple); // On convertie les pulsations en flux d'eau
    long fluxRigide = pulseToFlowInML(pulseCountRigide);
    oldTime = millis(); // On définit l'ancien temps à millis()
    pulseCountSouple = 0; // On remets à 0 le nombre de pulsation
    pulseCountRigide = 0;
 
    volumeSouple += fluxSouple;
    if (volumeSouple > volumeSoupleMax) { // On évite les érreurs avec des valeurs négative ou au delà du MAX
      volumeSouple = volumeSoupleMax;
    } else if (volumeSouple < 0) {
      volumeSouple = 0;
    }
    
    float mlCuveRigide = calculVolume(mmDiametre, mmLongueur, mmAirCuveRigide()); // On lance le calcule de l'eau dans la cuve rigide
 
    float fillingPourcent = map(mlCuveRigide, 0, volumeRigideMax, 0, 100);
 
    if ((fillingPourcent > 80) && (volumeSouple != volumeSoupleMax)){ // Si la cuve rigide est pleine, et que la cuve souple n'est pas pleine
      digitalWrite(versSouple, HIGH); // On remplis la cuve souple
      digitalWrite(versRigide, LOW);
    } else if ((fillingPourcent < 20) && (volumeSouple != 0)){ // Si la cuve rigide est sur le point d'être vide, et que la cuve souple n'est pas vide
      digitalWrite(versSouple, LOW); // On vide la cuve souple
      digitalWrite(versRigide, HIGH);
    } else { // Sinon, rien ne bouge
      digitalWrite(versSouple, LOW);
      digitalWrite(versRigide, LOW);
    }
 
    Serial.print("fr:"); // On envoie les donnée
    Serial.print(fluxRigide);
    Serial.print(";vs:");
    Serial.print(volumeSouple); 
    Serial.print(";vr:");
    Serial.print(mlCuveRigide);
    Serial.print('\n');
 
    attachInterrupt(digitalPinToInterrupt(debitMetreRigide), pulseCounterRigide, FALLING); // On récommence à compter le débit
    attachInterrupt(digitalPinToInterrupt(debitMetreSoupleIn), entreeSouple, FALLING);
    attachInterrupt(digitalPinToInterrupt(debitMetreSoupleOut), sortieSouple, FALLING);
  }
}
