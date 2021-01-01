
#include <Pinger.h>
#include <ESP8266WiFi.h>
#ifndef STASSID
#define STASSID "Wifi"
#define STAPSK  "!1I$410v3#3u"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

extern "C"
{
  #include <lwip/icmp.h> // needed for icmp packet definitions
}

// Set global to avoid object removing after setup() routine
Pinger pinger;

void setup()
{  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(13, OUTPUT);  //Yellow 
  pinMode(0, OUTPUT);   //Red
  pinMode(4, OUTPUT);   //Buzzer
  pinMode(12, OUTPUT); //aa
  pinMode(5, OUTPUT);  //bb
  
  // Begin serial connection at 9600 baud
  Serial.begin(115200);

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  
  // Connect to WiFi access point
  //bool stationConnected = WiFi.begin(
  //"Wifi",
  //"!1I$410v3#3u");

  // Check if connection errors
  //if(!stationConnected)
  //{
    //Serial.println("Error, unable to connect specified WiFi network.");
  //}
  
  // Wait connection completed
  //Serial.print("Connecting to AP...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.println();
  Serial.println();
  
  Serial.print("Wait for WiFi...");
  
 
  
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    digitalWrite(0, HIGH);
    digitalWrite(4, HIGH);
    //digitalWrite(5, HIGH);
    delay(500);    
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Use WiFiClient class to create TCP connections
  digitalWrite(4, LOW);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  String ssid = WiFi.SSID(); 
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(13, HIGH);
  digitalWrite(4, HIGH);
  //digitalWrite(12, HIGH);
  delay(500);

  WiFiClient client;
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  delay(500);
  
  
  pinger.OnReceive([](const PingerResponse& response)
  {
    if (response.ReceivedResponse)
    {
      Serial.printf(
        "Reply from %s: bytes=%d time=%lums TTL=%d\n",
        response.DestIPAddress.toString().c_str(),
        response.EchoMessageSize - sizeof(struct icmp_echo_hdr),
        response.ResponseTime,
        response.TimeToLive);
      digitalWrite(12, HIGH);
      digitalWrite(5, LOW);
    }
    else
    {
      Serial.printf("Request timed out.\n");
      digitalWrite(4, HIGH);
      digitalWrite(12, LOW);
      digitalWrite(5, HIGH);

    }

    // Return true to continue the ping sequence.
    // If current event returns false, the ping sequence is interrupted.
    return true;
  });
  
  pinger.OnEnd([](const PingerResponse& response)
  {
    // Evaluate lost packet percentage
    float loss = 100;
    if(response.TotalReceivedResponses > 0)
    {
      loss = (response.TotalSentRequests - response.TotalReceivedResponses) * 100 / response.TotalSentRequests;
    }
    
    // Print packet trip data
    Serial.printf(
      "Ping statistics for %s:\n",
      response.DestIPAddress.toString().c_str());
    Serial.printf(
      "    Packets: Sent = %lu, Received = %lu, Lost = %lu (%.2f%% loss),\n",
      response.TotalSentRequests,
      response.TotalReceivedResponses,
      response.TotalSentRequests - response.TotalReceivedResponses,
      loss);

    // Print time information
    if(response.TotalReceivedResponses > 0)
    {
      Serial.printf("Approximate round trip times in milli-seconds:\n");
      Serial.printf(
        "    Minimum = %lums, Maximum = %lums, Average = %.2fms\n",
        response.MinResponseTime,
        response.MaxResponseTime,
        response.AvgResponseTime);
    }
    
    // Print host data
    Serial.printf("Destination host data:\n");
    Serial.printf(
      "    IP address: %s\n",
      response.DestIPAddress.toString().c_str());
    if(response.DestMacAddress != nullptr)
    {
      Serial.printf(
        "    MAC address: " MACSTR "\n",
        MAC2STR(response.DestMacAddress->addr));
    }
    if(response.DestHostname != "")
    {
      Serial.printf(
        "    DNS name: %s\n",
        response.DestHostname.c_str());
    }

    return true;
  });
   
}

void loop()
{
 
if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(4, LOW);
    digitalWrite(0, LOW);
    digitalWrite(13, LOW);
    delay(50);
    digitalWrite(13, HIGH); 
    delay(50);

    // Ping default gateway
    //Serial.printf(
    //"\n\nPinging default gateway with IP %s\n",
    //WiFi.gatewayIP().toString().c_str());
    //pinger.Ping(WiFi.gatewayIP());  
    //delay(1000);

    // Ping Google DNS ip
    //Serial.printf("\n\nPinging Google DNS Internet Protocol \n");
    pinger.Ping(IPAddress(8,8,8,8));
    delay(500);
}
    delay(50);

//Wifi Status Disconnected  
if (WiFi.status() != WL_CONNECTED) {
    Serial.print(ssid);
    Serial.print(" is not connected\n");
    digitalWrite(13, LOW);
    digitalWrite(0, LOW);
    digitalWrite(5, LOW); 
    digitalWrite(4, HIGH);
    delay(50);
    digitalWrite(13, LOW);
    digitalWrite(0, HIGH); 
    digitalWrite(5, HIGH);
    digitalWrite(4, LOW); 
    delay(50);
  }  
}
