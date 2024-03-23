
#include <Arduino.h>

//
// =======================================================================================================
// WEB INTERFACE
// =======================================================================================================
//

void webInterface()
{

  static unsigned long currentTime = millis(); // Current time
  static unsigned long previousTime = 0;       // Previous time
  const long timeoutTime = 2000;               // Define timeout time in milliseconds (example: 2000ms = 2s)

  static bool Mode = false; // TODO

  if (true)
  { // Wifi on
    // if (WIFI_ON == 1) {     //Wifi on
    WiFiClient client = server.available(); // Listen for incoming clients

    if (client)
    { // If a new client connects,
      currentTime = millis();
      previousTime = currentTime;
      Serial.println("New Client."); // print a message out in the serial port
      String currentLine = "";       // make a String to hold incoming data from the client
      while (client.connected() && currentTime - previousTime <= timeoutTime)
      { // loop while the client's connected
        currentTime = millis();
        if (client.available())
        {                         // if there's bytes to read from the client,
          char c = client.read(); // read a byte, then
          Serial.write(c);        // print it out the serial monitor
          header += c;
          if (c == '\n')
          { // if the byte is a newline character
            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0)
            {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              // Display the HTML web page
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              client.println("<title>TheDIYGuy999 Sound & Light Controller</title>");

#if defined USE_CSS
              // CSS styles for buttons

#if defined MODERN_CSS // The modern CSS with scaling for better adaption between different devices
              client.println("<style>html { font-family: Verdana, Helvetica, sans-serif; display: inline-block; margin: 0px auto; text-align: center; color: #0009ff; background-color: #f2f2f2;}");
              // Text
              client.println("h1 {font-size: clamp(1.5rem, 2.5vw, 2.5rem);}");
              client.println("h2 {font-size: clamp(1.3rem, 2.0vw, 2.0rem);}");
              client.println("p {font-size: clamp(1rem, 1.5vw, 1.5rem); color: black; }");
              client.println("label {font-size: clamp(1rem, 1.5vw, 1.5rem); color: black; }");
              client.println("a {font-size: clamp(1rem, 1.5vw, 1.5rem); color: black; cursor: pointer; text-decoration: underline;}");
              // Two columns for checkboxes
              client.println(".multiColumn {display: inline-block; width: 49%; text-align: left; vertical-align: top;}");
              // Checkbox
              client.println("input[type=\"checkbox\"] {cursor: pointer; zoom: 1.5;}");
              // Slider
              client.println(".slider { -webkit-appearance: none; width: 95%; height: 25px; background: #d3d3d3; outline: none; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; margin: 5px; border-radius: 10px;}");
              client.println(".slider::-webkit-slider-thumb { -webkit-appearance: none; cursor: pointer; width: 95%; width: 35px; height: 35px; background: #66ffcc; outline: none; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; border-radius: 10px;}");
              client.println(".sliderServo1::-webkit-slider-thumb { background: #ff9999;}");
              client.println(".sliderServo2::-webkit-slider-thumb { background: #99ccff;}");
              client.println(".sliderServo3::-webkit-slider-thumb { background: #ffbb99;}");
              client.println(".sliderLed::-webkit-slider-thumb { background: #ffff99;}");
              // Collapsible
              client.println(".collapsible {background-color: #888; color: white;cursor: pointer; padding: 10px; width: 100%; border: none; text-align: center; outline: none; font-size: clamp(1rem, 1.5vw, 1.5rem); font-weight: bold;)}");
              client.println(".active, .collapsible:hover {background-color: #555;}");
              client.println(".content { display: none;}");
              // Textbox
              client.println(".textbox {cursor: pointer; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; font-size: clamp(1rem, 2vw, 2rem); padding: clamp(0.2rem, 1vw, 1rem); text-align: center; border-radius: 10px;}");
              // Buttons
              client.println(".buttonGreen {background-color: #4CAF50; color: white;}");
              client.println(".buttonRed {background-color: #ff0000; color: white;}");
              client.println(".buttonGrey {background-color: #7A7A7A; color: black;}");
              client.println(".button { cursor: pointer; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; padding: clamp(0.2rem, 1vw, 1rem); margin: clamp(0.2rem, 1vw, 1rem); font-size: clamp(1rem, 2vw, 2rem); width: 95%; border-radius: 10px;");

              client.println("</style></head>");

#else // Old CSS with green background
              client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color: rgb(60, 161, 120);}");
              client.println(".button { border: yes; color: white; padding: 10px 40px; width: 95%;");
              client.println("text-decoration: none; font-size: 16px; margin: 2px; cursor: pointer;}");
              client.println(".slider { -webkit-appearance: none; width: 95%; height: 25px; background: #d3d3d3; outline: none; opacity: 0.7; -webkit-transition: .2s; transition: opacity .2s; }");
              client.println(".buttonGreen {background-color: #4CAF50;}");
              client.println(".buttonRed {background-color: #ff0000;}");
              client.println(".buttonGrey {background-color: #7A7A7A;}");
              client.println(".textbox {font-size: 16px; text-align: center;}");
              client.println("</style></head>");
#endif
#endif

              client.println("</head>");

              client.println("<body onload=\"readDefaults()\">");

              client.println("<h1>TheDIYGuy999 Sound & Light Controller</h1>"); // Website title
              // client.printf("<p>Vehicle: %s\n", ssid); // TODO, not working!
              client.printf("<p>Software version: %s\n", codeVersion);
              client.printf("<p style=\"color:red;\"><b>Don't mess around while driving!</b></p>");

#if defined BATTERY_PROTECTION
              client.println("<hr>"); // Horizontal line ===================================================================================================================================================
              client.printf("<p>Battery voltage: %.2f V\n", batteryVolts());
              if (numberOfCells > 1)
              {
                client.printf("<p>Number of cells: %i (%iS battery)\n", numberOfCells, numberOfCells);
              }
              else
              {
                client.printf("<p style=\"color:red;\">Battery error!\n");
              }
              client.printf("<p>Battery cutoff voltage: %.2f V\n", batteryCutoffvoltage);
#endif

              client.println("<hr>"); // WiFi settings ===================================================================================================================================================
              client.println("<button type=\"button\" class=\"collapsible\">WiFi settings</button>");
              client.println("<div class=\"content\">");

              // Set1 (ssid) ----------------------------------
              valueString = ssid;              // Read current value
              client.println("<p>SSID: <br>"); // Display current value

              client.println("<input type=\"text\" id=\"Setting1Input\" size=\"31\" maxlength=\"31\" class=\"textbox\" oninput=\"Setting1change(this.value)\" value=\"" + valueString + "\" /></p>"); // Set new value
              client.println("<script> function Setting1change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Set1=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Set1=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                Serial.println(valueString);
                ssid = valueString;
              }

              // Set2 (password) ----------------------------------
              valueString = password;                                // Read current value
              client.println("<p>Password (min. length = 8): <br>"); // Display current value

              client.println("<input type=\"text\" id=\"Setting2Input\" size=\"31\" maxlength=\"31\" class=\"textbox\" oninput=\"Setting2change(this.value)\" value=\"" + valueString + "\" /></p>"); // Set new value
              client.println("<script> function Setting2change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Set2=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Set2=") >= 0)
              {
                pos1 = header.indexOf('='); // Start pos
                pos2 = header.indexOf('&'); // End pos
                valueString = header.substring(pos1 + 1, pos2);
                password = valueString;
              }
              client.println("</div>");

              client.println("<hr>"); // Wireless trailer settings ===================================================================================================================================================
              client.println("<button type=\"button\" class=\"collapsible\">Wireless trailer settings</button>");
              client.println("<div class=\"content\">");
              // Trailer 1 ********************************************************************************************************
              if (useTrailer1 == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr1\" checked onclick=\"CheckboxTr1Change(this.checked)\"> use trailer 1: </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr1\" unchecked onclick=\"CheckboxTr1Change(this.checked)\"> use trailer 1: </input></p>");
              }
              client.println("<script> function CheckboxTr1Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr1=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr1=true") >= 0)
              {
                useTrailer1 = true;
                Serial.println("Trailer 1 enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr1=false") >= 0)
              {
                useTrailer1 = false;
                Serial.println("Trailer 1 disabled");
              }

              // MAC0 ----------------------------------
              valueString = String(broadcastAddress1[0], HEX); // Read current value
              // client.println("<p>Custom MAC: "); // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac0change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr1Mac0change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac0Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac0Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                // Serial.println(pos1);
                // Serial.println(pos2);
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[0] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC1 ----------------------------------
              valueString = String(broadcastAddress1[1], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac1change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr1Mac1change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac1Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac1Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[1] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC2 ----------------------------------
              valueString = String(broadcastAddress1[2], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac2change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr1Mac2change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac2Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac2Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[2] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC3 ----------------------------------
              valueString = String(broadcastAddress1[3], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac3change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr1Mac3change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac3Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac3Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[3] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC4 ----------------------------------
              valueString = String(broadcastAddress1[4], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac4change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr1Mac4change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac4Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac4Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[4] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC5 ----------------------------------
              valueString = String(broadcastAddress1[5], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr1Mac5change(this.value)\" value=\"" + valueString + "\" /></p>"); // Set new value
              client.println("<script> function Tr1Mac5change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr1Mac5Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr1Mac5Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress1[5] = strtol(valueString.c_str(), NULL, 16);
              }

              // Trailer 2 ********************************************************************************************************
              // use trailer ----------------------------------
              if (useTrailer2 == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr2\" checked onclick=\"CheckboxTr2Change(this.checked)\"> use trailer 2: </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr2\" unchecked onclick=\"CheckboxTr2Change(this.checked)\"> use trailer 2: </input></p>");
              }
              client.println("<script> function CheckboxTr2Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr2=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr2=true") >= 0)
              {
                useTrailer2 = true;
                Serial.println("Trailer 2 enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr2=false") >= 0)
              {
                useTrailer2 = false;
                Serial.println("Trailer 2 disabled");
              }

              // MAC0 ----------------------------------
              valueString = String(broadcastAddress2[0], HEX); // Read current value
              // client.println("<p>Custom MAC: "); // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac0change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr2Mac0change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac0Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac0Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[0] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC1 ----------------------------------
              valueString = String(broadcastAddress2[1], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac1change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr2Mac1change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac1Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac1Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[1] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC2 ----------------------------------
              valueString = String(broadcastAddress2[2], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac2change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr2Mac2change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac2Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac2Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[2] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC3 ----------------------------------
              valueString = String(broadcastAddress2[3], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac3change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr2Mac3change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac3Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac3Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[3] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC4 ----------------------------------
              valueString = String(broadcastAddress2[4], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac4change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr2Mac4change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac4Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac4Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[4] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC5 ----------------------------------
              valueString = String(broadcastAddress2[5], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr2Mac5change(this.value)\" value=\"" + valueString + "\" /></p>"); // Set new value
              client.println("<script> function Tr2Mac5change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr2Mac5Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr2Mac5Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress2[5] = strtol(valueString.c_str(), NULL, 16);
              }

              // Trailer 3 ********************************************************************************************************
              // use trailer ----------------------------------
              if (useTrailer3 == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr3\" checked onclick=\"CheckboxTr3Change(this.checked)\"> use trailer 3: </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr3\" unchecked onclick=\"CheckboxTr3Change(this.checked)\"> use trailer 3: </input></p>");
              }

              client.println("<script> function CheckboxTr3Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr3=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr3=true") >= 0)
              {
                useTrailer3 = true;
                Serial.println("Trailer 3 enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr3=false") >= 0)
              {
                useTrailer3 = false;
                Serial.println("Trailer 3 disabled");
              }

              // MAC0 ----------------------------------
              valueString = String(broadcastAddress3[0], HEX); // Read current value
              // client.println("<p>Custom MAC: "); // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac0change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr3Mac0change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac0Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac0Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[0] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC1 ----------------------------------
              valueString = String(broadcastAddress3[1], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac1change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr3Mac1change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac1Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac1Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[1] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC2 ----------------------------------
              valueString = String(broadcastAddress3[2], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac2change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr3Mac2change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac2Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac2Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[2] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC3 ----------------------------------
              valueString = String(broadcastAddress3[3], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac3change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr3Mac3change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac3Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac3Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[3] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC4 ----------------------------------
              valueString = String(broadcastAddress3[4], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac4change(this.value)\" value=\"" + valueString + "\" />"); // Set new value (no </p> = no new line)
              client.println("<script> function Tr3Mac4change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac4Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac4Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[4] = strtol(valueString.c_str(), NULL, 16);
              }

              // MAC5 ----------------------------------
              valueString = String(broadcastAddress3[5], HEX); // Read current value
              client.println(":");                             // Display title

              client.println("<input type=\"text\" style=\"text-transform: uppercase\" size=\"2\" maxlength=\"2\" class=\"textbox\" oninput=\"Tr3Mac5change(this.value)\" value=\"" + valueString + "\" /></p>"); // Set new value
              client.println("<script> function Tr3Mac5change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Tr3Mac5Set=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Tr3Mac5Set=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                broadcastAddress3[5] = strtol(valueString.c_str(), NULL, 16);
              }

              client.printf("<p>Use HEX values (0-9, A-F) only, always starting with FE");

              client.println("</div>");

              client.println("<hr>"); // ESC settings ===================================================================================================================================================
              client.println("<button type=\"button\" class=\"collapsible\">ESC settings</button>");
              client.println("<div class=\"content\">");
              client.printf("<p style=\"color:red;\"><b>Lift traction wheels off the ground while adjusting!</b></p>");

              // Slider1 (ESC pulse span) ----------------------------------
              valueString = String(escPulseSpan, DEC);
              client.println("<p>ESC pulse span (vehicle top speed, 500 = fastest, 1200 = slowest): <span id=\"textSlider1Value\">" + valueString + "</span><br>"); // Label
              client.println("<input type=\"range\" min=\"500\" max=\"1200\" step=\"50\" class=\"slider\" id=\"Slider1Input\" onchange=\"Slider1Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider1Change(pos) { ");
              client.println("var slider1Value = document.getElementById(\"Slider1Input\").value;");
              client.println("document.getElementById(\"textSlider1Value\").innerHTML = slider1Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider1=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider1=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                escPulseSpan = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("escPulseSpan = " + String(escPulseSpan));
              }

              // Slider2 (ESC takeoff punch) ----------------------------------
              valueString = String(escTakeoffPunch, DEC);
              client.println("<p>ESC takeoff punch (additional motor force around neutral): <span id=\"textSlider2Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"150\" step=\"5\" class=\"slider\" id=\"Slider2Input\" onchange=\"Slider2Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider2Change(pos) { ");
              client.println("var slider2Value = document.getElementById(\"Slider2Input\").value;");
              client.println("document.getElementById(\"textSlider2Value\").innerHTML = slider2Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider2=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider2=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                escTakeoffPunch = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("escTakeoffPunch = " + String(escTakeoffPunch));
              }

              // Slider3 (ESC reverse plus) ----------------------------------
              valueString = String(escReversePlus, DEC);
              client.println("<p>ESC reverse plus (additional reverse speed / trim, power-cycle ESC!): <span id=\"textSlider3Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"220\" step=\"5\" class=\"slider\" id=\"Slider3Input\" onchange=\"Slider3Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider3Change(pos) { ");
              client.println("var slider3Value = document.getElementById(\"Slider3Input\").value;");
              client.println("document.getElementById(\"textSlider3Value\").innerHTML = slider3Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider3=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider3=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                escReversePlus = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("escReversePlus = " + String(escReversePlus));
              }

              // Slider4 (Crawler mode ESC ramp time) ----------------------------------
              valueString = String(crawlerEscRampTime, DEC);
              client.println("<p>Crawler mode ESC ramp time: <span id=\"textslider4Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"20\" step=\"2\" class=\"slider\" id=\"Slider4Input\" onchange=\"Slider4Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider4Change(pos) { ");
              client.println("var slider4Value = document.getElementById(\"Slider4Input\").value;");
              client.println("document.getElementById(\"textslider4Value\").innerHTML = slider4Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider4=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider4=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                crawlerEscRampTime = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("crawlerEscRampTime = " + String(crawlerEscRampTime));
              }

              // Slider5 (ESC global acceleration %) ----------------------------------
              valueString = String(globalAccelerationPercentage, DEC);
              client.println("<p>ESC global acceleration % (experimental): <span id=\"textslider5Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"100\" max=\"200\" step=\"10\" class=\"slider\" id=\"Slider5Input\" onchange=\"Slider5Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider5Change(pos) { ");
              client.println("var slider5Value = document.getElementById(\"Slider5Input\").value;");
              client.println("document.getElementById(\"textslider5Value\").innerHTML = slider5Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider5=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider5=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                globalAccelerationPercentage = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("globalAccelerationPercentage = " + String(globalAccelerationPercentage));
              }

              // RZ7886 ESC only options:
#if defined RZ7886_DRIVER_MODE
              // Slider6 (RZ7886 ESC brake margin) ----------------------------------
              valueString = String(brakeMargin, DEC);
              client.println("<p>RZ7886 ESC brake margin (experimental): <span id=\"textslider6Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"20\" step=\"2\" class=\"slider\" id=\"Slider6Input\" onchange=\"Slider6Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider6Change(pos) { ");
              client.println("var slider6Value = document.getElementById(\"Slider6Input\").value;");
              client.println("document.getElementById(\"textslider6Value\").innerHTML = slider6Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider6=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider6=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                brakeMargin = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("brakeMargin = " + String(brakeMargin));
              }

              // Slider7 (RZ7886 ESC frequency) ----------------------------------
              valueString = String(RZ7886_FREQUENCY, DEC);
              client.println("<p>RZ7886 ESC frequency (frequencies > 500 may overheat driver!): <span id=\"textslider7Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"200\" max=\"1000\" step=\"50\" class=\"slider\" id=\"Slider7Input\" onchange=\"Slider7Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider7Change(pos) { ");
              client.println("var slider7Value = document.getElementById(\"Slider7Input\").value;");
              client.println("document.getElementById(\"textslider7Value\").innerHTML = slider7Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider7=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider7=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                RZ7886_FREQUENCY = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("RZ7886_FREQUENCY = " + String(RZ7886_FREQUENCY));
              }

              // Slider8 (RZ7886 ESC dragbrake duty %) ----------------------------------
              valueString = String(RZ7886_DRAGBRAKE_DUTY, DEC);
              client.println("<p>RZ7886 ESC dragbrake % (more = stronger brake): <span id=\"textslider8Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"100\" step=\"5\" class=\"slider\" id=\"Slider8Input\" onchange=\"Slider8Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider8Change(pos) { ");
              client.println("var slider8Value = document.getElementById(\"Slider8Input\").value;");
              client.println("document.getElementById(\"textslider8Value\").innerHTML = slider8Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider8=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider8=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                RZ7886_DRAGBRAKE_DUTY = (valueString.toInt());
                setupMcpwmESC();
                Serial.println("RZ7886_DRAGBRAKE_DUTY = " + String(RZ7886_DRAGBRAKE_DUTY));
              }
#endif
              client.println("</div>");

              client.println("<hr>"); // Servo settings ===================================================================================================================================================
              client.println("<button type=\"button\" class=\"collapsible\">Servo settings</button>");
              client.println("<div class=\"content\">");

              // Slider11 (Steering position left) ----------------------------------
              valueString = String(CH1L, DEC);
              client.println("<p>Steering position left: <span id=\"textslider11Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"800\" max=\"2200\" step=\"10\" class=\"slider sliderServo1\" id=\"Slider11Input\" onchange=\"Slider11Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider11Change(pos) { ");
              client.println("var slider11Value = document.getElementById(\"Slider11Input\").value;");
              client.println("document.getElementById(\"textslider11Value\").innerHTML = slider11Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider11=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider11=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH1L = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH1L = " + String(CH1L));
              }

              // Slider12 (Steering position center) ----------------------------------
              valueString = String(CH1C, DEC);
              client.println("<p>Steering position center: <span id=\"textslider12Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"1300\" max=\"1700\" step=\"5\" class=\"slider sliderServo1\" id=\"Slider12Input\" onchange=\"Slider12Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider12Change(pos) { ");
              client.println("var slider12Value = document.getElementById(\"Slider12Input\").value;");
              client.println("document.getElementById(\"textslider12Value\").innerHTML = slider12Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider12=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider12=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH1C = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH1C = " + String(CH1C));
              }

              // Slider13 (Steering position right) ----------------------------------
              valueString = String(CH1R, DEC);
              client.println("<p>Steering position right: <span id=\"textslider13Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"800\" max=\"2200\" step=\"10\" class=\"slider sliderServo1\" id=\"Slider13Input\" onchange=\"Slider13Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider13Change(pos) { ");
              client.println("var slider13Value = document.getElementById(\"Slider13Input\").value;");
              client.println("document.getElementById(\"textslider13Value\").innerHTML = slider13Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider13=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider13=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH1R = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH1R = " + String(CH1R));
              }

              // Slider14 (Transmission position left) ----------------------------------
              valueString = String(CH2L, DEC);
              client.println("<p>Transmission position left (1st gear): <span id=\"textslider14Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"800\" max=\"2200\" step=\"10\" class=\"slider sliderServo2\" id=\"Slider14Input\" onchange=\"Slider14Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider14Change(pos) { ");
              client.println("var slider14Value = document.getElementById(\"Slider14Input\").value;");
              client.println("document.getElementById(\"textslider14Value\").innerHTML = slider14Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider14=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider14=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH2L = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH2L = " + String(CH2L));
              }

              // Slider15 (Transmission position center) ----------------------------------
              valueString = String(CH2C, DEC);
              client.println("<p>Transmission position center (2nd gear): <span id=\"textslider15Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"800\" max=\"2200\" step=\"10\" class=\"slider sliderServo2\" id=\"Slider15Input\" onchange=\"Slider15Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider15Change(pos) { ");
              client.println("var slider15Value = document.getElementById(\"Slider15Input\").value;");
              client.println("document.getElementById(\"textslider15Value\").innerHTML = slider15Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider15=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider15=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH2C = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH2C = " + String(CH2C));
              }

              // Slider16 (Transmission position right) ----------------------------------
              valueString = String(CH2R, DEC);
              client.println("<p>Transmission position right (3rd gear): <span id=\"textslider16Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"800\" max=\"2200\" step=\"10\" class=\"slider sliderServo2\" id=\"Slider16Input\" onchange=\"Slider16Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider16Change(pos) { ");
              client.println("var slider16Value = document.getElementById(\"Slider16Input\").value;");
              client.println("document.getElementById(\"textslider16Value\").innerHTML = slider16Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider16=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider16=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH2R = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH2R = " + String(CH2R));
              }

              // Slider17 (coupler position left) ----------------------------------
              valueString = String(CH4L, DEC);
              client.println("<p>coupler position left (locked): <span id=\"textslider17Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"1000\" max=\"2000\" step=\"10\" class=\"slider sliderServo3\" id=\"Slider17Input\" onchange=\"Slider17Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider17Change(pos) { ");
              client.println("var slider17Value = document.getElementById(\"Slider17Input\").value;");
              client.println("document.getElementById(\"textslider17Value\").innerHTML = slider17Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider17=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider17=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH4L = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH4L = " + String(CH4L));
              }

              // Slider18 (coupler position right) ----------------------------------
              valueString = String(CH4R, DEC);
              client.println("<p>coupler position right (unlocked): <span id=\"textslider18Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"1000\" max=\"2000\" step=\"10\" class=\"slider sliderServo3\" id=\"Slider18Input\" onchange=\"Slider18Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider18Change(pos) { ");
              client.println("var slider18Value = document.getElementById(\"Slider18Input\").value;");
              client.println("document.getElementById(\"textslider18Value\").innerHTML = slider18Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider18=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider18=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                CH4R = (valueString.toInt());
                setupMcpwm();
                Serial.println("CH4R = " + String(CH4R));
              }
              client.println("</div>");

              client.println("<hr>"); // Light settings ===================================================================================================================================================
              client.println("<button type=\"button\" class=\"collapsible\">Light settings</button>");
              client.println("<div class=\"content\">");

              // Checkbox21 (Flickering while cranking) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (flickeringWileCranking == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr21\" checked onclick=\"CheckboxTr21Change(this.checked)\"> Flickering while cranking </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr21\" unchecked onclick=\"CheckboxTr21Change(this.checked)\"> Flickering while cranking </input></p>");
              }
              client.println("<script> function CheckboxTr21Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr21=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr21=true") >= 0)
              {
                flickeringWileCranking = true;
                Serial.println("Flickering while cranking enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr21=false") >= 0)
              {
                flickeringWileCranking = false;
                Serial.println("Flickering while cranking disabled");
              }
              client.println("</div>");

              // Checkbox22 (Xenon simulation) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (xenonLights == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr22\" checked onclick=\"CheckboxTr22Change(this.checked)\"> Xenon simulation </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr22\" unchecked onclick=\"CheckboxTr22Change(this.checked)\"> Xenon simulation </input></p>");
              }
              client.println("<script> function CheckboxTr22Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr22=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr22=true") >= 0)
              {
                xenonLights = true;
                Serial.println("Xenon simulation enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr22=false") >= 0)
              {
                xenonLights = false;
                Serial.println("Xenon simulation disabled");
              }
              client.println("</div>");
              // Checkbox23 (Swap L & R indicators) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (swap_L_R_indicators == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr23\" checked onclick=\"CheckboxTr23Change(this.checked)\"> Swap L & R indicators </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr23\" unchecked onclick=\"CheckboxTr23Change(this.checked)\"> Swap L & R indicators </input></p>");
              }
              client.println("<script> function CheckboxTr23Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr23=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr23=true") >= 0)
              {
                swap_L_R_indicators = true;
                Serial.println("Swap L & R indicators enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr23=false") >= 0)
              {
                swap_L_R_indicators = false;
                Serial.println("Swap L & R indicators disabled");
              }
              client.println("</div>");

              // Checkbox24 (Indicators as sidemarkers) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (indicatorsAsSidemarkers == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr24\" checked onclick=\"CheckboxTr24Change(this.checked)\"> Indicators as sidemarkers </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr24\" unchecked onclick=\"CheckboxTr24Change(this.checked)\"> Indicators as sidemarkers </input></p>");
              }
              client.println("<script> function CheckboxTr24Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr24=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr24=true") >= 0)
              {
                indicatorsAsSidemarkers = true;
                Serial.println("Indicators as sidemarkers enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr24=false") >= 0)
              {
                indicatorsAsSidemarkers = false;
                Serial.println("Indicators as sidemarkers disabled");
              }
              client.println("</div>");

              // Checkbox28 (LED indicators) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (ledIndicators == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr28\" checked onclick=\"Checkboxtr28Change(this.checked)\"> LED indicators </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr28\" unchecked onclick=\"Checkboxtr28Change(this.checked)\"> LED indicators </input></p>");
              }
              client.println("<script> function Checkboxtr28Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Checkboxtr28=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Checkboxtr28=true") >= 0)
              {
                ledIndicators = true;
                Serial.println("LED indicators enabled");
              }
              else if (header.indexOf("GET /?Checkboxtr28=false") >= 0)
              {
                ledIndicators = false;
                Serial.println("LED indicators disabled");
              }
              client.println("</div>");

              // Checkbox25 (Separate full beam) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (separateFullBeam == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr25\" checked onclick=\"CheckboxTr25Change(this.checked)\">Separate full beam (roof light connector)</input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr25\" unchecked onclick=\"CheckboxTr25Change(this.checked)\">Separate full beam (roof light connector)</input></p>");
              }
              client.println("<script> function CheckboxTr25Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?CheckboxTr25=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?CheckboxTr25=true") >= 0)
              {
                separateFullBeam = true;
                Serial.println("Separate full beam enabled");
              }
              else if (header.indexOf("GET /?CheckboxTr25=false") >= 0)
              {
                separateFullBeam = false;
                Serial.println("Separate full beam disabled");
              }
              client.println("</div>");

              // Checkbox26 (No cab lights) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (noCabLights == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr26\" checked onclick=\"Checkboxtr26Change(this.checked)\"> No cab lights </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr26\" unchecked onclick=\"Checkboxtr26Change(this.checked)\"> No cab lights </input></p>");
              }
              client.println("<script> function Checkboxtr26Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Checkboxtr26=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Checkboxtr26=true") >= 0)
              {
                noCabLights = true;
                Serial.println("No cab lights enabled");
              }
              else if (header.indexOf("GET /?Checkboxtr26=false") >= 0)
              {
                noCabLights = false;
                Serial.println("No cab lights disabled");
              }
              client.println("</div>");

              // Checkbox27 (No fog lights) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (noFogLights == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr27\" checked onclick=\"Checkboxtr27Change(this.checked)\"> No fog lights </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr27\" unchecked onclick=\"Checkboxtr27Change(this.checked)\"> No fog lights </input></p>");
              }
              client.println("<script> function Checkboxtr27Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Checkboxtr27=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Checkboxtr27=true") >= 0)
              {
                noFogLights = true;
                Serial.println("No fog lights enabled");
              }
              else if (header.indexOf("GET /?Checkboxtr27=false") >= 0)
              {
                noFogLights = false;
                Serial.println("No fog lights disabled");
              }
              client.println("</div>");

              // Checkbox29 (Flashing blue lights) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (flashingBlueLight == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr29\" checked onclick=\"Checkboxtr29Change(this.checked)\"> Flashing blue lights </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr29\" unchecked onclick=\"Checkboxtr29Change(this.checked)\"> Flashing blue lights </input></p>");
              }
              client.println("<script> function Checkboxtr29Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Checkboxtr29=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Checkboxtr29=true") >= 0)
              {
                flashingBlueLight = true;
                Serial.println("Flashing blue lights enabled");
              }
              else if (header.indexOf("GET /?Checkboxtr29=false") >= 0)
              {
                flashingBlueLight = false;
                Serial.println("Flashing blue lights disabled");
              }
              client.println("</div>");

              // Checkbox30 (Hazards on, if 5th wheel is unlocked) ----------------------------------
              client.println("<div class=\"multiColumn\">");
              if (hazardsWhile5thWheelUnlocked == true)
              {
                client.println("<p><input type=\"checkbox\" id=\"tr30\" checked onclick=\"Checkboxtr30Change(this.checked)\"> Hazards on, if 5th wheel is unlocked </input></p>");
              }
              else
              {
                client.println("<p><input type=\"checkbox\" id=\"tr30\" unchecked onclick=\"Checkboxtr30Change(this.checked)\"> Hazards on, if 5th wheel is unlocked </input></p>");
              }
              client.println("<script> function Checkboxtr30Change(pos) { ");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Checkboxtr30=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Checkboxtr30=true") >= 0)
              {
                hazardsWhile5thWheelUnlocked = true;
                Serial.println("Hazards on, if 5th wheel is unlocked enabled");
              }
              else if (header.indexOf("GET /?Checkboxtr30=false") >= 0)
              {
                hazardsWhile5thWheelUnlocked = false;
                Serial.println("Hazards on, if 5th wheel is unlocked disabled");
              }
              client.println("</div>");

              // Dummy checkbox
              //client.println("<div class=\"multiColumn\">");
              //client.println("</div>");

              /*
              uint8_t cabLightsBrightness = 100;      // Usually 255, 100 for Actros & Ural
              uint8_t sideLightsBrightness = 150;     // Usually 200, 100 for WPL C44, 50 for Landy, 100 for P407, 150 for Actros
              uint8_t reversingLightBrightness = 140; // Around 140, 50 for Landy & Ural
              uint8_t fogLightBrightness = 200;       // Around 200
              uint8_t rearlightDimmedBrightness = 30; // tailligt brightness, if not braking, about 30
              uint8_t rearlightParkingBrightness = 3; // 0, if you want the taillights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
              uint8_t headlightParkingBrightness = 3; // 0, if you want the headlights being off, if side lights are on, or about 5 if you want them on (0 for US Mode)
              */

              // Slider21 (Cab light brightness) ----------------------------------
              valueString = String(cabLightsBrightness, DEC);
              client.println("<p>Cab light brightness: <span id=\"textslider21Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"50\" max=\"255\" step=\"5\" class=\"slider sliderLed\" id=\"Slider21Input\" onchange=\"Slider21Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider21Change(pos) { ");
              client.println("var slider21Value = document.getElementById(\"Slider21Input\").value;");
              client.println("document.getElementById(\"textslider21Value\").innerHTML = slider21Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider21=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider21=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                cabLightsBrightness = (valueString.toInt());
                Serial.println("cabLightsBrightness = " + String(cabLightsBrightness));
              }

              // Slider22 (Side light brightness) ----------------------------------
              valueString = String(sideLightsBrightness, DEC);
              client.println("<p>Side light brightness: <span id=\"textslider22Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"50\" max=\"255\" step=\"5\" class=\"slider sliderLed\" id=\"Slider22Input\" onchange=\"Slider22Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider22Change(pos) { ");
              client.println("var slider22Value = document.getElementById(\"Slider22Input\").value;");
              client.println("document.getElementById(\"textslider22Value\").innerHTML = slider22Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider22=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider22=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                sideLightsBrightness = (valueString.toInt());
                Serial.println("sideLightsBrightness = " + String(sideLightsBrightness));
              }

              // Slider23 (Reversing light brightness) ----------------------------------
              valueString = String(reversingLightBrightness, DEC);
              client.println("<p>Reversing light brightness: <span id=\"textslider23Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"50\" max=\"255\" step=\"5\" class=\"slider sliderLed\" id=\"Slider23Input\" onchange=\"Slider23Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider23Change(pos) { ");
              client.println("var slider23Value = document.getElementById(\"Slider23Input\").value;");
              client.println("document.getElementById(\"textslider23Value\").innerHTML = slider23Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider23=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider23=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                reversingLightBrightness = (valueString.toInt());
                Serial.println("reversingLightBrightness = " + String(reversingLightBrightness));
              }

              // Slider27 (Fog light brightness) ----------------------------------
              valueString = String(fogLightBrightness, DEC);
              client.println("<p>Fog light brightness: <span id=\"textslider27Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"100\" max=\"255\" step=\"5\" class=\"slider sliderLed\" id=\"Slider27Input\" onchange=\"Slider27Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider27Change(pos) { ");
              client.println("var slider27Value = document.getElementById(\"Slider27Input\").value;");
              client.println("document.getElementById(\"textslider27Value\").innerHTML = slider27Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider27=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider27=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                fogLightBrightness = (valueString.toInt());
                Serial.println("fogLightBrightness = " + String(fogLightBrightness));
              }

              // Slider24 (Tail light dimmed brightness while not braking) ----------------------------------
              valueString = String(rearlightDimmedBrightness, DEC);
              client.println("<p>Tail light dimmed brightness (while not braking, use low value in Indicators as sidemarkers mode): <span id=\"textslider24Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"30\" max=\"255\" step=\"5\" class=\"slider sliderLed\" id=\"Slider24Input\" onchange=\"Slider24Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider24Change(pos) { ");
              client.println("var slider24Value = document.getElementById(\"Slider24Input\").value;");
              client.println("document.getElementById(\"textslider24Value\").innerHTML = slider24Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider24=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider24=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                rearlightDimmedBrightness = (valueString.toInt());
                Serial.println("rearlightDimmedBrightness = " + String(rearlightDimmedBrightness));
              }

              // Slider25 (Tail light dimmed brightness while parking lights only) ----------------------------------
              valueString = String(rearlightParkingBrightness, DEC);
              client.println("<p>Tail light dimmed brightness (while parking lights only): <span id=\"textslider25Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"5\" step=\"1\" class=\"slider sliderLed\" id=\"Slider25Input\" onchange=\"Slider25Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider25Change(pos) { ");
              client.println("var slider25Value = document.getElementById(\"Slider25Input\").value;");
              client.println("document.getElementById(\"textslider25Value\").innerHTML = slider25Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider25=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider25=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                rearlightParkingBrightness = (valueString.toInt());
                Serial.println("rearlightParkingBrightness = " + String(rearlightParkingBrightness));
              }

              // Slider26 (Head light dimmed brightness while parking lights only) ----------------------------------
              valueString = String(headlightParkingBrightness, DEC);
              client.println("<p>Head light dimmed brightness (while parking lights only): <span id=\"textslider26Value\">" + valueString + "</span><br>");
              client.println("<input type=\"range\" min=\"0\" max=\"5\" step=\"1\" class=\"slider sliderLed\" id=\"Slider26Input\" onchange=\"Slider26Change(this.value)\" value=\"" + valueString + "\" /></p>");
              client.println("<script> function Slider26Change(pos) { ");
              client.println("var slider26Value = document.getElementById(\"Slider26Input\").value;");
              client.println("document.getElementById(\"textslider26Value\").innerHTML = slider26Value;");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Slider26=\" + pos + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Slider26=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                headlightParkingBrightness = (valueString.toInt());
                Serial.println("headlightParkingBrightness = " + String(headlightParkingBrightness));
              }

              // Select Neopixel animation mode -------------------------------------------------------------------------------

              client.println("<label for=\"select1Input\">Neopixel animation mode:</label>");
              client.println("<select name=\"select1\" class=\"button\" id=\"select1Input\" onchange=\"select1Change(this)\">");
              client.println("<option value=\"1\">1 = Demo (don't use it)</option>");
              client.println("<option value=\"2\">2 = Knight Rider scanner animation for 8 LED </option>");
              client.println("<option value=\"3\">3 = Bluelight animation for 8 LED </option>");
              client.println("<option value=\"4\">4 = Union Jack United Kingdom animation for 8 LED</option>");
              client.println("<option value=\"5\">5 = B33lz3bub Austria animation for 3 LED</option>");
              client.println("</select>");

              client.println("<script> function select1Change(pos) { ");
              client.println("var select1Value = pos.value;"); // OK
              client.println("console.log(select1Value);");
              client.println("var xhr = new XMLHttpRequest();");
              client.println("xhr.open('GET', \"/?Select1=\" + select1Value + \"&\", true);");
              client.println("xhr.send(); } </script>");

              if (header.indexOf("GET /?Select1=") >= 0)
              {
                pos1 = header.indexOf('=');
                pos2 = header.indexOf('&');
                valueString = header.substring(pos1 + 1, pos2);
                neopixelMode = (valueString.toInt());
                setupNeopixel();
                Serial.println("neopixelMode = " + String(neopixelMode));
              }

              client.println("</div>");

              client.println("<hr>"); // Horizontal line ===================================================================================================================================================

              client.printf("<p>Save settings & restart required after changes above");

              // button1 (Save settings to EEPROM) ----------------------------------
              client.println("<p><a href=\"/save/on\"><button class=\"button buttonRed\" onclick=\"restartPopup()\" >Save settings & restart</button></a></p>");

              if (header.indexOf("GET /save/on") >= 0)
              {
                eepromWrite();
                delay(1000);
                ESP.restart();
              }
              client.println("<script> function restartPopup() {");
              client.println("alert(\"Controller restarted, you may need to reconnect WiFi!\"); ");
              client.println("} </script>");

              client.println("<p>It is recommended to power-cycle the controller as well as to close and reopen the browser window after using this button!</p>");

              //-----------------------------------------------------------------------------------------------------------------------

              client.println("<br>More informations on my <a href=\"https://thediyguy999.github.io/TheDIYGuy999_ESP32_Web_Flasher/index.html\" target=\"_blank\">Website</a><br>");

              //-----------------------------------------------------------------------------------------------------------------------
              // Script for collapsible sections
              client.println("<script> var coll = document.getElementsByClassName(\"collapsible\"); var i; for (i = 0; i < coll.length; i++) { coll[i].addEventListener(\"click\", function() { this.classList.toggle(\"active\"); var content = this.nextElementSibling; if (content.style.display === \"block\") { content.style.display = \"none\"; } else { content.style.display = \"block\"; } }); } </script>");

              // Script for reading defaults
              client.println("<script> function readDefaults() {");
              client.println("document.getElementById(\"select1Input\").value = \"" + String(neopixelMode, DEC) + "\";");
              client.println("}</script>");

              //-----------------------------------------------------------------------------------------------------------------------

              client.println("</body></html>");

              // The HTTP-response ends with an empty column
              client.println();
              // Break out of the while loop
              break;
            }
            else
            { // if you got a newline, then clear currentLine
              currentLine = "";
            }
          }
          else if (c != '\r')
          {                   // if you got anything else but a carriage return character,
            currentLine += c; // add it to the end of the currentLine
          }
        }
      }
      // Clear header
      header = "";
      // Disconnect client
      client.stop();
      Serial.println("Client disconnected.");

#if defined DEBUG
      Serial.print("DEBUG stuff: ");
      // Serial.printf("Trailer 1 MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n", broadcastAddress1[0], broadcastAddress1[1], broadcastAddress1[2], broadcastAddress1[3], broadcastAddress1[4], broadcastAddress1[5]);
      Serial.println("");
#endif
    }
  }
}
