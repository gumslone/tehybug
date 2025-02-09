#pragma once
namespace http {
String get(HTTPClient &http, WiFiClient & espClient, String url) {
  D_print("HTTP GET: ");
  D_println(url);
  url = tehybug.replacePlaceholders(url);
  http.begin(espClient, url); // Specify request destination
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

  const int httpCode = http.GET(); // Send the request
  String payload{};
  D_println(httpCode);       // Print HTTP return code
  if (httpCode == 200) {
    // Log(F("http_get"), url);
    payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  else if(httpCode < 0)
  {
    D_println(http.errorToString(httpCode));
  }
  http.end(); // Close connection
  return payload;
}
String post(HTTPClient &http, WiFiClient & espClient, String url, String post_json) {
  D_print("HTTP POST: ");
  D_println(url);
  http.begin(espClient, url); // Specify request destination
  http.addHeader("Content-Type",
                 "application/json"); // Specify content-type header
  post_json = tehybug.replacePlaceholders(post_json);
  int httpCode = http.POST(post_json); // Send the request
  D_println(httpCode);                 // Print HTTP return code
  String payload{};
  if (httpCode == 200) {
    // Log(F("http_post"), post_json);
    String payload = http.getString(); // Get the response
    // payload
    D_println(payload); // Print request response payload
  }
  else if(httpCode < 0)
  {
    D_println(http.errorToString(httpCode));
  }
  http.end(); // Close connection
  return payload;
}
}
