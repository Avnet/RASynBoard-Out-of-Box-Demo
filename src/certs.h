
#ifndef CERTS__H
#define CERTS__H

//***********************CERTIFCATES***********************************************

const char *cert_aws_ca1 =
        "C0,-----BEGIN CERTIFICATE-----\n"
        "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
        "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
        "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
        "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
        "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
        "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
        "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
        "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
        "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
        "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
        "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
        "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
        "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
        "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
        "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
        "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
        "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
        "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
        "-----END CERTIFICATE-----\n";

const char *awsDevicePrivateKey_cert =
        "C2,-----BEGIN RSA PRIVATE KEY-----\n" // Don't change this line
        "MIIEowIBAAKCAQEetyop734BEZSYa8oNJdWqPxjPcOjUyNmflTVl08FHxSfpzHFo\n"
        "OscegStExTO9LRYcc8b2hnTVVVohrdwzRESs8jqS30anTOW8YfKHcIFJEGQu3KCJ\n"
        // Paste in your device private key certificate here.  The lines
        // above and below this comment are just to show the format.  Remove
        // all lines when you add your certificate.
        "EGEL123R3e7E0s8qvqhRQH+EhZsYv2+uqSsI8w63Aqzxv6do62Hg\n"
        "-----END RSA PRIVATE KEY-----\n"; // Don't change this line

const char *awsDevice_cert =
        "C1,-----BEGIN CERTIFICATE-----\n" // Don't change this line
        "MIIEowIBAAKCAQEetyop734BEZSYa8oNJdWqPxjPcOjUyNmflTVl08FHxSfpzHFo\n"
        "OscegStExTO9LRYcc8b2hnTVVVohrdwzRESs8jqS30anTOW8YfKHcIFJEGQu3KCJ\n"
        // Paste in your device certificate here.  The lines
        // above and below this comment are just to show the format.  Remove
        // all lines when you add your certificate.
        "EGEL123R3e7E0s8qvqhRQH+EhZsYv2+uqSsI8w63Aqzxv6do62Hg\n"
        "-----END CERTIFICATE-----\n"; // Don't change this line
#endif // CERTS__H
