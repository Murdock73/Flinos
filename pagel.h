

   client.println("<HTML>\r\n");
   client.println("<head>\r\n");
   client.println("<title>Camera AC</title>\r\n");
   client.println("</head>\r\n");
   client.println("<body>\r\n");
   client.println("<h2><code>Da Flinos project - Camera da letto</code></h2>\r\n");
   client.println("\r\n");
   client.println("<p><code><span style=\"font-size:14px;\">Temperatura attuale</span>&nbsp;
   client.println(temperatureCString);
   client.println(" C&deg;&nbsp;</code></p>\r\n"
   client.println("\r\n");
   client.println("<p><code>Parametri climatizzatore</code></p>\r\n");
   client.println("\r\n");
   client.println("<p><code>Mode &nbsp;
   client.println(HvacMode);                  
   client.println("<input name=\"AcMode\" type=\"button\" value=\"Cambia\" /></code></p>\r\n");
   client.println("\r\n");
   client.println("<p><code>Temp ");
   client.println(HvacTemp);                                    
   client.println(" &nbsp;<input name=\"TempPlus\" type=\"button\" value=\"+\" />&nbsp;<input name=\"TempMinus\" type=\"button\" value=\"-\" /></code></p>\r\n");
   client.println("\r\n");                  
   client.println("<p><code>Acc&nbsp; &nbsp;<input name=\"AcOn\" type=\"button\" value=\"ON\" /></code></p>\r\n");
   client.println("\r\n");
   client.println("<p><code>Spent&nbsp;<input name=\"AcOff\" type=\"button\" value=\"OFF\" /></code></p>\r\n");
   client.println("\r\n");
   client.println("<p><span style=\"font-size:8px;\">Versione 0.1.1</span></p>\r\n");
   client.println("</body>\r\n");
   client.println("</html>\r\n");

