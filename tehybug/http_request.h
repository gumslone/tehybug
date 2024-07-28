#pragma once
namespace http {
void get(HTTPClient &http,  WiFiClient & espClient, String url) {
  D_print("HTTP GET: ");
  D_println(url);
  url = tehybug.replacePlaceholders(url);
  http.begin(espClient, url); // Specify request destination
  http.setURL(url);
  http.addHeader("Content-Type", "text/plain"); // Specify content-type header

  const int httpCode = http.GET(); // Send the request
  D_println(httpCode);       // Print HTTP return code
  if (httpCode == 200) {
    // Log(F("http_get"), url);
    const String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  http.end(); // Close connection
}
void post(HTTPClient &http, WiFiClient & espClient, String url, String post_json) {
  D_print("HTTP POST: ");
  D_println(url);
  http.begin(espClient, url); // Specify request destination
  http.addHeader("Content-Type",
                 "application/json"); // Specify content-type header
  post_json = tehybug.replacePlaceholders(post_json);
  int httpCode = http.POST(post_json); // Send the request
  D_println(httpCode);                 // Print HTTP return code
  if (httpCode == 200) {
    // Log(F("http_post"), post_json);
    const String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  http.end(); // Close connection
}
}
