#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
 
char ssid[] = "Eco-Connect";                 // Nom du réseau
char pass[] = "Eco-Connect";                 // Mot de passe réseau
byte mac[6];
 
WiFiServer server(80);
 
WiFiClient client;
MySQL_Connection conn((Client *)&client);
 
char query[256];
 
IPAddress server_addr(192, 168 , 0, 101);         // MySQL server IP
char user[] = "arduino";           // MySQL user
char password[] = "S4lutat!ons";       // MySQL password
 
void setup() {
 
  WiFi.begin(ssid, pass); // On se connecte au WiFi
 
  while (WiFi.status() != WL_CONNECTED) { // On attends la confirmation de la connexion
  }
 
  WiFi.macAddress(mac); // On définit l'adresse mac
 
  while (conn.connect(server_addr, 3308, user, password) != true) { // On se connecte à la base de données et on attends la confirmation
    delay(200);
  }
  
  Serial.begin(9600); // On initialise le port série quand tout est prêt
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
 
void loop() {
  if (Serial.available()) { // En cas d'activité sur le port série
    String data = Serial.readStringUntil('\n'); // On récupère les données jusqu'au '\n'
    if (data != "") { // Si les données ne sont pas vide (en cas de parasite)
      if (data.startsWith("prop")) { // Si ça commence par "prop", c'est que l'Arduino demande les propriété
        char GET_PROP[] = "SELECT diametre,longueur,volume FROM fermeeconnect.properties ORDER BY id DESC LIMIT 1"; // On créer la requête
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); // On créer un curseur
        cur_mem->execute(GET_PROP);  // On exécute la requête avec le curseur pour en obtenir les valeurs
        column_names *cols = cur_mem->get_columns(); // On obtiens les colonne
        row_values *row = NULL; // On initialise un pointeur de ligne
        Serial.print("update;"); // Puis on commence la réponse à l'arduino
        do {
          row = cur_mem->get_next_row(); // On récupère la première ligne retournée
          if(row != NULL){ // Si elle n'est pas nul
            for(int x = 0; x < cols->num_fields;x++){
              Serial.print(row->values[x]); // On affiche la valeur
              if (x < cols->num_fields-1){ // Si ce n'est pas la dernière valeur, on ajoute un ";" qui permettra à l'arduino de découpée les valeurs
                Serial.print(';');
              }
            }
          }
        }while (row != NULL); // On continue tant que la ligne n'est pas nul
        Serial.print('\n'); // Puis on termine la ligne
        delete cur_mem; // Enfin, on libère la mémoire
      } else if (data.startsWith("get")) { // Si c'est "get", c'est que l'arduino veut récupéré la dernière valeur
        char GET_LAST[] = "SELECT volume_souple FROM fermeeconnect.eau ORDER BY id DESC LIMIT 1";
        MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
        cur_mem->execute(GET_LAST);
        column_names *cols = cur_mem->get_columns();
        row_values *row = NULL;
        Serial.print("last;");
        do {
          row = cur_mem->get_next_row();
          if(row != NULL){
            for(int x = 0; x < cols->num_fields;x++){
              Serial.print(row->values[x]);
              if (x < cols->num_fields-1){
                Serial.print(';');
              }
            }
          }
        }while (row != NULL);
        Serial.print('\n');
        delete cur_mem;
      } else { // Sinon, c'est que c'est une données de l'arduino et on l'insère
        char INSERT_SQL[] = "INSERT INTO fermeeconnect.eau(conso_inst, volume_souple, volume_rigide) VALUES (%s, %s, %s)";
        String val[] = {getValue(data, ';', 0), getValue(data, ';', 1), getValue(data, ';', 2)};
        bool check[] = {false, false, false};
        for (int i = 0; i < 3; i++) {
          if (val[i].startsWith("fr")) {
            check[0] = true;
          } else if (val[i].startsWith("vs")) {
            check[1] = true;
          } else if (val[i].startsWith("vr")) {
            check[2] = true;
          }
        }
        int verif = 0;
        for (int i = 0; i < 3; i++) {
          if (check[i] == true) {
            verif++;
          }
        }
        //Serial.println(verif);
        if (verif == 3) {
          char ci[20];
          char vs[20];
          char vr[20];
          for (int i = 0; i < 3; i++) {
            if (val[i].startsWith("fr")) {
              dtostrf(getValue(val[i], ':', 1).toFloat(), 19, 2, ci); // Convertie un nombre en chaine de caractère
            } else if (val[i].startsWith("vs")) {
              dtostrf(getValue(val[i], ':', 1).toFloat(), 19, 2, vs);
            } else if (val[i].startsWith("vr")) {
              dtostrf(getValue(val[i], ':', 1).toFloat(), 19, 2, vr);
            }
          }
          sprintf(query, INSERT_SQL, ci, vs, vr); // Permet d'inséré des valeurs à la place d'un %s de la chaîne de caractère.
 
          MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
 
          cur_mem->execute(query);
 
          delete cur_mem;
        }
      }
    }
  }
}
