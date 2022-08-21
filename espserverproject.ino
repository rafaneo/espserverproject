#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

const char* ssid     = "SSID"; //Change this to your SSID
const char* password = "PASS"; // Change this to your password

WebServer server(8888); 

//Set pins
const int trigPin = 5;
const int echoPin = 18;

long duration;
float distanceCm;

String adcValue;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

//Html, to display information
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
.card{
    max-width: 400px;
     min-height: 250px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
</style>
<body>

<div class="card">
  <h4>Is Rafael Online ?</h4><br>
  <h1><span id="ADCValue">Undefined</span></h1><br>
</div>
<script>

setInterval(function() {
  getData();
}, 2000);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML =
      this.responseText;
      console.log(this.responceText);
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
</script>
</body>
</html>
)=====";


//Updates automatically the page
void handleRoot() {
 String s = MAIN_page;
 server.send(200, "text/html", s);
}

//Updates the data sent
void handleADC(){ 
 if(distanceCm <=60){ 
   adcValue = "He is online :)";
 }
 else{
  adcValue = "He is offline :(";
 }

 server.send(200, "text/plane", adcValue);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);  
  server.on("/readADC", handleADC);
 
  server.begin(); 
  Serial.println("HTTP server started");
}

void loop(){
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;

  Serial.println(adcValue);
  
  server.handleClient();
  delay(1);
}
