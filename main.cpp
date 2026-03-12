#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const int LED = 5;    // Définition LED
const int Moteur = 4;    // Définition moteur 
const char * SSID = "Livebox";
const char * PASSWORD = "1234";
bool Mode; // Mode = False : Local / True : Distant 
bool On_Manu;
bool Off_Manu;
/* Reglages */
int Param_001;    // Seuil

/* Mesure */
int Lux;  // Luminosité  

String Send_None_Args = "Appel API sans parametre";
String Send_Unk_Args = " Argument inconnu";
String Send_Unk_cmd = " Commande inconnu";
String Send_LED_ON = "LED ALLUMER";
String Send_LED_OFF = "LED ETTEINTE";
String Send_Param = "CONSIGNE APPLIQUEE ";
String Send_mod_On = " Mode manu actif";
String Send_mod_Off = " Mode manu inactif";
String Send_Lux;

ESP8266WebServer serverWeb(80); // Objet Webserver 
// Fonctions du Serveur - Page web 
void switchLEDon() {
        digitalWrite(LED, HIGH);
        serverWeb.send(200, "text/hyml", "LED ON");
    }

void switchLEDoff() {
        digitalWrite(LED, LOW);
        serverWeb.send(200, "text/hyml", "LED OFF");
    }

void handleRoot() {
        serverWeb.send(200, "text/hyml", "Page principale");
    }
// API 
void handleAPI_html() {
    // Au moins un arguments
    if (serverWeb.args() > 0)
    {
        if (serverWeb.args() == 1) // Controle premier argument
        {   // Commande demande d'ETAT
            if (serverWeb.arg(0) == "getLED")
            {
                if (digitalRead(LED) == LOW)  // Etat LED
                {
                    serverWeb.send(200, "text/plain", "0"); // LED Etteinte
                }
                else
                {
                    serverWeb.send(200, "text/plain", "1"); // LED Allumée
                }   
            }
            if (serverWeb.arg(0) == "getLux")
                {
                    int Lux = analogRead(A0);
                        if (!On_Manu)
                        {
                            if (Lux<Param_001)
                            {
                                digitalWrite(LED, HIGH);
                                digitalWrite(Moteur, HIGH);
                            }
                            else
                            {
                                digitalWrite(LED, LOW);
                                digitalWrite(Moteur, LOW);
                            }
                        }
                    serverWeb.send(200, "text/plain", Send_Lux=String(Lux));    // Lumière
                    }
            else
                {
                    serverWeb.send(200, "text/plain", Send_Unk_cmd);    // Demande d'etat inconnue
                }
        }
        else if (serverWeb.args() == 2)
        {
        /* XXXXXXXXX   Consigne via HTTP Lux  XXXXXXXXXXX */
           if (serverWeb.arg(0) == "setINT")
            {
                if (serverWeb.argName(1) == "value")
                {
                    int Lux = analogRead(A0);
                    Param_001 = serverWeb.arg(1).toInt();
                    serverWeb.send(200, "text/plain", Send_Param); // LED Etteinte        
                }
                else
                {
                    serverWeb.send(200, "text/plain", Send_Unk_cmd);
                }
            }

        /* XXXXXXXXX   Comande via HTTP ON / OFF LED XXXXXXXXXXX */
            else if (serverWeb.arg(0) == "setLED")
            {
                if (serverWeb.argName(1) == "value")
                {
                    if (serverWeb.arg(1) == "on")
                    {
                        digitalWrite(LED, HIGH);
                        digitalWrite(Moteur, HIGH);
                        serverWeb.send(200, "text/plain", Send_LED_ON);
                    }
                    else if (serverWeb.arg(1) == "off")
                    {
                        digitalWrite(LED, LOW);
                        digitalWrite(Moteur, LOW);
                        serverWeb.send(200, "text/plain", Send_LED_OFF);
                    }
                    else
                    {
                        serverWeb.send(200, "text/plain", Send_Unk_cmd);
                    } 
                }
                else
                {
                    serverWeb.send(200, "text/plain", Send_Unk_cmd);
                }
            }
            else if (serverWeb.arg(0) == "setMOD")
            {
                if (serverWeb.argName(1) == "value")
                {
                    if (serverWeb.arg(1) == "on")
                    {
                        On_Manu=true;
                        serverWeb.send(200, "text/plain", Send_mod_On);
                    }
                    else if (serverWeb.arg(1) == "off")
                    {
                        On_Manu=false;
                        serverWeb.send(200, "text/plain", Send_mod_Off);
                    }
                    else
                    {
                        serverWeb.send(200, "text/plain", Send_Unk_cmd);
                    }
                }
                else
                {
                    serverWeb.send(200, "text/plain", Send_Unk_cmd);
                }  
            }
            else
            {
                serverWeb.send(200, "text/plain", Send_Unk_cmd);
            }     
        }
    }    
    else
    {
        serverWeb.send(200, "text/plain", Send_None_Args);
    }  
}

void setup() {
/* Liaison Série */
Serial.begin(9600);
/* Mode OUTPUT LED */
pinMode(LED, OUTPUT);
pinMode(Moteur, OUTPUT);
// Connexion reseau
WiFi.hostname("Phare");
WiFi.mode(WIFI_STA);
WiFi.begin(SSID, PASSWORD);
// Mise en place serveur Web
serverWeb.on("/switchLEDon", switchLEDon);  // Page pour Allummer LED
serverWeb.on("/switchLEDoff", switchLEDoff); // Page pour Eteindre LED
serverWeb.on("/", handleRoot);
serverWeb.on("/api.html", handleAPI_html);
serverWeb.begin();
}

void loop() {
    if (WiFi.isConnected())
    {
    serverWeb.handleClient();
    }  

}
