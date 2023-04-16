//Initialisation de la communication Bluetooth
#include <SoftwareSerial.h>             // Librairie nécessaire pour communiquer avec le module bluetooth qui communique via le port série
#define rxPin 6                         // Broche 6 en tant que RXD sur le module bluetooth
#define txPin 7                         // Broche 7 en tant que TXD sur le module bluetooth
#define baudrate 9600                   // Définit la vitesse d'envoie des données entre les différents appareils
SoftwareSerial mySerial(rxPin , txPin); // Definition du software serial. On transforme les pins 6 et 7 en ports séries

//Initialisation pour l'écran OLED
#include <Adafruit_SSD1306.h>               // Librairie nécessaire pour commander le OLED qui communique via le bus I2C
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c    branchement de SCL sur pinA5 et de SDA sur pinA4
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

//Initialisation pour le potentiomètre digital
#include <SPI.h>                      // Librairie nécessaire pour commander le potentiomètre qui communique via l'interface SPI
const byte csPin           = 10;      // Chip select pin du potentiomètre
const int  maxPositions    = 256;     // Le balayage se fait de 0 à 255 = 256 positions
const long rAB             = 44000;   // 44k résistance entre A et B, ajustée par rapport à la valeur du multimètre
const byte rWiper          = 125;     // Modifier d'un cran le potentiomètre modifie de 125 ohms la résistance
const byte pot0            = 0x11;    // Adresse du pot0 permettant de changer la valeur du potentiomètre // B 0001 0001
const byte pot0Shutdown    = 0x21;    // pot0 shutdown // B 0010 0001
int pos_pot = 13 ;                    // Position de départ du potentiomètre, proche de la position optimale pour la calibration à plat

//Initialisation pour Nunchuk
#include <Wire.h>     // Librairie nécessaire pour communiquer avec le Nunchuk qui communique via le bus I2C
#include <string.h>   //
#undef int
#include <stdio.h>
uint8_t outbuf[6];    // Matrice pour stocker les valeurs renvoyées par le nunchuk
int cnt = 0;          // Initialisation de count pour récupérer le bon nombre de bits dans la communication avec le nunchuk, voir ci-dessous
int nunchuk[2] = {0}; // Initialisation de la matrice nécessaire pour stocker l'état du bouton c et la position en y du joystick

//pour le calcul direct dans l'arduino mais pas fait pour le moment car pas d'envoie de float
float Vcc = 4.33;
float R1 = 100000.0;
//float R2 = 1000.0;              //attention R2 devient poten digital
long res_wiper = 0 ;
float R3 = 100000.0;
float R5 = 10000.0;
int min_pour = 0 ;
int max_pour = 0 ;

int calib = 0; // Indique si la calibration a été faite, calib est mise à 1 une fois la calibration réalisée

// Fonction pour commander le potentiomètre digital
/*D10           CS  (pin 1)
  D11 (MOSI)    SI  (pin 3)
  D13 (SCK)     SCK (pin 2)*/
void setPotWiper(int addr, int pos) {
  pos = constrain(pos, 0, 255);            // Limite la valeur de pos de 0 à 255
  digitalWrite(csPin, LOW);                // Selectionne le potentiomètre
  SPI.transfer(addr);                      // Donne l'adresse de l'élément dont on veut changer la valeur à pos
  SPI.transfer(pos);                       // Changement le la valeur à pos
  digitalWrite(csPin, HIGH);               // Fin de la communication avec le potentiomètre
}

// Fonctions pour le Nunchuk
void nunchuck_init()
{
  Wire.beginTransmission (0x52);  // transmit to device 0x52
  Wire.write (0x40);    // sends memory address
  Wire.write (0x00);    // sends sent a zero.
  Wire.endTransmission ();  // stop transmitting
}

void send_zero()
{
  Wire.beginTransmission (0x52);  // transmit to device 0x52
  Wire.write(0x00);   // sends one byte
  Wire.endTransmission ();  // stop transmitting
}

// Encode data to format that most wiimote drivers except
// only needed if you use one of the regular wiimote drivers
char nunchuk_decode_byte (char x)
{
  x = (x ^ 0x17) + 0x17;
  return x;
}

void print (int *output)      // Permet de retirer les valeurs qui nous intéressent uniquement, l'axe y et le bouton c
{
  int joy_y_axis = outbuf[1];
  int c_button = 0;

  // L'octet de outbuf[5] contient le bit pour le bouton c, on récupère son état grâce à un masque
  if ((outbuf[5] >> 1) & 1) {
    c_button = 1;
  }
  // Stock des valeurs utiles pour piloter les valeurs du potentiomètre, utilisé avec nunchuk dans la suite du programme
  output[0] = joy_y_axis ;
  output[1] = c_button ;
}

void setup() {
  // Setup bluetooth
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(baudrate);

  Serial.begin(baudrate);

  // Setup OLED
  ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED);

  // Setup du potentiomètre
  digitalWrite(csPin, HIGH);        // Chip select est par défaut déselectionné
  pinMode(csPin, OUTPUT);           // Configure le chip select comme sortie
  SPI.begin();

  // Setup nunchuk
  Wire.begin ();
  nunchuck_init ();
}

void loop() {
  // Boucle en mode calibration pour calib = 0
  // Le but des deux boucles pour calib = 0 et 1 est de pouvoir régler la sensibilité suivant le capteur que l'on a, en essayant de la maximiser par rapport à notre connaissance de notre capteur
  // Ces boucles permettent une totale adaptabilité à la résistance/conductance de notre capteur pour maximiser l'apaptabilité du circuit d'amplification
  if (calib == 0) {
    int meus = analogRead(A0);
    // Gestion de l'affichage OLED
    ecranOLED.clearDisplay();                 // Effaçage de l'intégralité du buffer
    ecranOLED.setTextSize(2);
    ecranOLED.setCursor(0, 0);
    ecranOLED.setTextColor(SSD1306_WHITE);    // Affichage du texte en "blanc" (avec la couleur principale, en fait, car l'écran monochrome peut être coloré)
    ecranOLED.println("Calib 0");
    pos_pot = constrain(pos_pot, 0, 255) ;    // Force pos_pot entre 0 et 255
    if (pos_pot == 255) {
      ecranOLED.print(meus);
      ecranOLED.println(" max");    // Si le potentiomètre est à sa valeur maximale (255), on ne peut plus l'augmenter et "max" s'affiche
    }
    else if (pos_pot == 0) {
      ecranOLED.print(meus);
      ecranOLED.println(" min");    // Si le potentiomètre est à sa valeur minimale (0), on ne peut plus la diminuer et "min" s'affiche
    }
    else {
      ecranOLED.print(meus);
      ecranOLED.print(" ");
      ecranOLED.println(pos_pot);
    }
    ecranOLED.println("Validez");
    ecranOLED.print("avec C");
    ecranOLED.display();          // Transfert le buffer à l'écran

    // Permet de piloter la valeur du potentiomètre avec les données récupérées du nunchuk
    Wire.requestFrom (0x52, 6);  // Demande les 6 octets de données du nunchuck formant un set complet de données
    while (Wire.available ()) {
      outbuf[cnt] = nunchuk_decode_byte (Wire.read());  // Convertit les octets en entiers
      cnt++;
    }
    // Si on reçoit 6 octets on en extrait les valeurs pertinentes pour notre programme que l'on stocke dans la matrice nunchuk
    if (cnt >= 5) {
      print (nunchuk);
    }
    cnt = 0;
    send_zero ();     // Demande les prochains octets

    // Conditions permettant de faire changer la valeur du potentiomètre ou de valider la calibration
    if (nunchuk[1] == 0) {
      calib = 1;        // Valide la calibration pour c appuyé, calib prend donc la valeur 1 pour sortir de la boucle de calibration
      res_wiper = ((rAB * pos_pot) / maxPositions ) + rWiper ;    // Fixe la valeur de la résistance du potentiomètre pour les calculs
      min_pour = meus ;
      nunchuk[1] = 1 ;
      delay(500) ;
    }
    if (nunchuk[0] <= 90) {
      pos_pot = pos_pot - 1 ;       // Si la valeur selon l'axe Y est inférieure à 90 (soit le joystick baissé légèrement) alors on réduit la résistance du potentiomètre
      setPotWiper(pot0, pos_pot);
    }
    if (nunchuk[0] >= 180) {
      pos_pot = pos_pot + 1 ;     // Si la valeur selon l'axe Y est supérieure à 180 (soit le joystick relevé légèrement) alors on augmente la résistance du potentiomètre
      setPotWiper(pot0, pos_pot);
    }
    delay (75);
  }
  // Cette boucle permet de sélectionner le maximum de déformation afin de pouvoir avoir une référence haute
  else if (calib == 1) {
    int meus = analogRead(A0);
    // Gestion de l'affichage OLED
    ecranOLED.clearDisplay();                 // Effaçage de l'intégralité du buffer
    ecranOLED.setTextSize(2);
    ecranOLED.setCursor(0, 0);
    ecranOLED.setTextColor(SSD1306_WHITE);    // Affichage du texte en "blanc" (avec la couleur principale, en fait, car l'écran monochrome peut être coloré)
    ecranOLED.println("Calib 100");
    ecranOLED.println(meus);
    ecranOLED.println("Validez");
    ecranOLED.print("avec C");
    ecranOLED.display();          // Transfert le buffer à l'écran

    // Permet de piloter la valeur du potentiomètre avec les données récupérées du nunchuk
    Wire.requestFrom (0x52, 6);  // Demande les 6 octets de données du nunchuck formant un set complet de données
    while (Wire.available ()) {
      outbuf[cnt] = nunchuk_decode_byte (Wire.read());  // Convertit les octets en entiers
      cnt++;
    }
    // Si on reçoit 6 octets on en extrait les valeurs pertinentes pour notre programme que l'on stocke dans la matrice nunchuk
    if (cnt >= 5) {
      print (nunchuk);
    }
    cnt = 0;
    send_zero ();     // Demande les prochains octets

    // Conditions permettant de faire changer la valeur du potentiomètre ou de valider la calibration
    if (nunchuk[1] == 0) {
      calib = 2;        // Valide la calibration pour c appuyé, calib prend donc la valeur 2 pour sortir de la deuxième boucle de calibration
      max_pour = meus ;
    }
    delay (100);
  }

  // Boucle en mode mesure une fois la calibration finie
  else if (calib == 2) {
    // On lit la valeur du capteur sur le pin A0
    int sensorValue = analogRead(A0);
    // Convertit la valeur lue (qui va de 0 à 1023) en volts (0 - 5V):
    float voltage = sensorValue * (Vcc / 1023.0);
    // Calcul de la résistance
    float res = (Vcc * R1 * (R3 + res_wiper)) / (voltage * res_wiper) - (R1 + R5);
    // Convertit la valeur du capteur en pourcentage de déformation par rapport à une plage (780 - 880) prédéfinie pour notre capteur
    int pourcentage_def = map(sensorValue, min_pour, max_pour, 0, 100) ;

    Serial.println(res);
    mySerial.print(res);                // On ecrit sur le port serie software vers le bluetooth
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(2);
    ecranOLED.setCursor(0, 0);
    ecranOLED.setTextColor(SSD1306_WHITE);
    ecranOLED.println("Deforme a");
    ecranOLED.print(pourcentage_def);
    ecranOLED.println(" %");
    ecranOLED.display();
    delay(500);
  }
}
