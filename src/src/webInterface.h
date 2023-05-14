
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
              client.println("<style>html { font-family: Verdana, Helvetica, sans-serif; display: inline-block; margin: 0px auto; text-align: center; color: #0009ff; background-color: #white;}");

              client.println("h1 {font-size: clamp(1.5rem, 2.5vw, 2.5rem);}");
              client.println("h2 {font-size: clamp(1.3rem, 2.0vw, 2.0rem);}");
              client.println("p {font-size: clamp(1rem, 1.5vw, 1.5rem); color: black; }");
              client.println("a {font-size: clamp(1rem, 1.5vw, 1.5rem); color: black; cursor: pointer; text-decoration: underline;}");

              client.println("input[type=\"checkbox\"] {cursor: pointer; zoom: 1.5;}");

              client.println(".slider { -webkit-appearance: none; width: 95%; height: 25px; background: #d3d3d3; outline: none; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; margin: 5px; border-radius: 10px;}");
              client.println(".slider::-webkit-slider-thumb { -webkit-appearance: none; cursor: pointer; width: 95%; width: 35px; height: 35px; background: #fff; outline: none; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; border-radius: 10px;}");

              client.println(".textbox {cursor: pointer; border: clamp(0.15rem, 0.15vw, 0.3rem) solid black; font-size: clamp(1rem, 2vw, 2rem); padding: clamp(0.2rem, 1vw, 1rem); text-align: center; border-radius: 10px;}");
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

              // Website title
              client.println("</head><body><h1>TheDIYGuy999 Sound & Light Controller</h1>");
              client.printf("<p>Software version %s\n", codeVersion);

#if defined BATTERY_PROTECTION
              client.println("<hr>"); // Horizontal line ===================================================================================================================================================
              client.printf("<p>Battery voltage: %.2f V\n", batteryVolts());
              if (numberOfCells > 1)
              {
                client.printf("<p>Number of cells: %i (%iS battery)\n", numberOfCells, numberOfCells);
              }
              else
              {
                client.printf("<p>Battery error!\n");
              }
              client.printf("<p>Battery cutoff voltage: %.2f V\n", batteryCutoffvoltage);
              client.println("<hr>"); // Horizontal line ===================================================================================================================================================
#endif

              client.printf("<p style=\"color:red;\">Don't mess around while driving!</p>");

              client.println("<hr>"); // Horizontal line ===================================================================================================================================================

              // Settings ------------------------------------------------------------------------------------------------------------

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
              client.printf("<p>Save settings & restart required after changes in this section");

              client.println("<hr>"); // Horizontal line ===================================================================================================================================================

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
              client.printf("<p>Save settings & restart required after changes in this section");

              client.println("<hr>"); // Horizontal line ===================================================================================================================================================

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
                Serial.println("escPulseSpan = "+ String(escPulseSpan));
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
                Serial.println("escTakeoffPunch = "+ String(escTakeoffPunch));
              }

              client.println("<hr>"); // Horizontal line ===================================================================================================================================================

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

              client.println("<p>It is recommended to power cycle the controller as well as to close and reopen the browser window after using this button!</p>");

              //-----------------------------------------------------------------------------------------------------------------------

              client.println("<br>More informations on my <a href=\"https://thediyguy999.github.io/TheDIYGuy999_ESP32_Web_Flasher/index.html\" target=\"_blank\">Website</a><br>");

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
