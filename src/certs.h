
#ifndef CERTS__H
#define CERTS__H

//#define USE_RASYN01_CONFIG
#define RASynBoardBW
//#define USE_RASYNBW01
//#define RASynBoardBW2

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

#ifdef USE_RASYN01_CONFIG

const char *awsDevicePrivateKey_cert =
        "C2,-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIEowIBAAKCAQEAp5YSdhQBEZSYa8oNJdWqPxjPcOjUyNmflTVl08FHxSfpzHFo\n"
        "OscegStExTO9LRYcc8b2hnTVVVohrdwzRESs8jqS30anTOW8YfKHcIFJEGQu3KCJ\n"
        "P8kYBKxaHG/Ro4qJKaGENuhr7VkXG8MiKAmIzPJOMlt9LHllmHP3mPUxZDZhJ22A\n"
        "324Nkdq+DMXW9NMThHvMvbE95VWhEdka4LXkV4PREXa6Kb+J30ZcMX9hUkiWNxRd\n"
        "tHS9t5P6kp5r4HLnJqxI0hPuepK3QocrXO72j7FdmD2+uTbf8ktsL1ZoNmhOLZzk\n"
        "M19ZWRtP3w6F3pFdvCIRwhIw+BXMkXQ61p0U8wIDAQABAoIBAGBksbm5R+QU1Jcr\n"
        "YOSiQYqJesg3JC1i/6eTTz69rcp5asFtecZVQfaKLMRBPk7wqOzvRF2SViMB9N1M\n"
        "CzD5T4EEr/Y9bX2Npw2vsrfDpYMIHOxKaqeFw4rsl+hDaRHoXXzUqnfquCmNVP1j\n"
        "AAKHeWho9xGADj/Gk+lcys3wOriooEqqyW0H2wsIZae+73pnwDdWSSRmAA/n4vMt\n"
        "9j+iNouk11sxQJ2mLEDdXue22m6veQn+RXVuAFwqPetJxo6hbv1nTou7bE60jJki\n"
        "+mbbdjzZhBUiQrbgJ7Ix8DF+Ha6NwCoIPkQUdRwEefkM+h2eeOeE6wQN/XYfBfzn\n"
        "7XiNVAECgYEA1rUHCfVBaaFD5AKQvMXY+EinrpVXsVN/eWLwFBQ6kt5cDYujp3iO\n"
        "zA0OEy8/cEE3oVVThc3vpE41WoDvEKxDqf9oZcqdZo06Zfrepl2Y85ZE6rUdz5eC\n"
        "Ym3Z+lU3b4E0OMuBRQY6R8yPb8hPg0xcQzs9Mobs0U8CzMo9Z6qYeOkCgYEAx9ET\n"
        "UBILHmDrRl8lu3pyGic/eCnEqAUtmxzCEA0TgddI/jLAMeJ5sWLjUMw1D6u6oQqr\n"
        "AbV9x6GVNk57S9Nsid/VaIzUCBNeps9gPMf9JgO/dS2fHsVTW+0EHilYPYLs67bN\n"
        "+6t/FZDvsSpB7B2O64WwlwVMewO4L9U+y5AE9XsCgYEAw7L3On1gZP5HmlE4Scfv\n"
        "7saSiE+5QXkA7MxgVXzmsq2YrAQGZAQ/6/9OP/BF23SmVxgl7AT8E//xI4/313Oc\n"
        "rRuOtZFLjIjXnNJgFLWlTwk9wczZ4jiP8FV+ZG09/RWAWvAq4JY/kugCKXoj92In\n"
        "imQ5cI+I1XM18yG3fIEDPtkCgYAAxwh7NEbEzH293VrpOtW80lBF6Guyd6qwwtDi\n"
        "fRqJU8wd6RakWww8K3VU+CZwtCtauw7lR86Ae1l4R5/lMKJoeu16pamhimnA3mey\n"
        "dNjAUB1rmEzZMs1bOzXb5C9Ktfq0BBcTFpy8+b5lAxQUlhzbP77rJ4Mfq2kJDpxa\n"
        "8GmEhwKBgCJSc0eqsHdAYhbLHcj2u4L5qYhmv+JRydTnRIDk1zhPIjlEK3rNm2ki\n"
        "YIuiGZ8zGkOfJHWkC8sT24nA3ei5+AuUYeoouVVg7A8y3jlsPfW4dGfXI+05qMZ/\n"
        "EGEL123R3e7E0s8qvqhRQH+EhZsYv2+uqSsI8w63Aqzxv6do62Hg\n"
        "-----END RSA PRIVATE KEY-----\n";

const char *awsDevice_cert =
        "C1,-----BEGIN CERTIFICATE-----\n"
        "MIIDWTCCAkGgAwIBAgIUfSJNkxASZmt1iH1P6OnB2IvxjQIwDQYJKoZIhvcNAQEL\n"
        "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
        "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDUyMjE0NDQ0\n"
        "N1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
        "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKeWEnYUARGUmGvKDSXV\n"
        "qj8Yz3Do1MjZn5U1ZdPBR8Un6cxxaDrHHoErRMUzvS0WHHPG9oZ01VVaIa3cM0RE\n"
        "rPI6kt9Gp0zlvGHyh3CBSRBkLtygiT/JGASsWhxv0aOKiSmhhDboa+1ZFxvDIigJ\n"
        "iMzyTjJbfSx5ZZhz95j1MWQ2YSdtgN9uDZHavgzF1vTTE4R7zL2xPeVVoRHZGuC1\n"
        "5FeD0RF2uim/id9GXDF/YVJIljcUXbR0vbeT+pKea+By5yasSNIT7nqSt0KHK1zu\n"
        "9o+xXZg9vrk23/JLbC9WaDZoTi2c5DNfWVkbT98Ohd6RXbwiEcISMPgVzJF0Otad\n"
        "FPMCAwEAAaNgMF4wHwYDVR0jBBgwFoAU43maRetdsGk1kOAC3CTUhzU/AzIwHQYD\n"
        "VR0OBBYEFDZ4gYucnh+J6NWBhnf19FUIoNmCMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
        "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCsyop/Zxy2LGYy/xGpOBj8xrog\n"
        "wRe55YCpFEtV5RGwLnC3XYHFhkyKbS8wiBUpujKaq9KLACFEJCcK4H5Rr7nlSMZp\n"
        "onwsMnFCTa1P8joh1yHtpv2ilb0KPLV0Exw7xU+2qNwKXTImEDLjUjeay1Xgkynd\n"
        "9xMxjeC4gNKhP0NUn6smFMj/ji+rd9j5eRtyWnDD3drmYF7RmOClwl1lvTYkd1W1\n"
        "A7iRRpldp6wg6hvdnApgoPJOqvBxcztvZKPcoq9iHnavlhh0QmckpFILkAwH6CUl\n"
        "LsfeY33cfxDONpESvpljM2No0C4g+Yx2dW13KQqFJAa00WX94GJf8RyAabZe\n"
        "-----END CERTIFICATE-----\n";
#endif

#ifdef RASynBoardBW
const char *awsDevicePrivateKey_cert =
        "C2,-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIEowIBAAKCAQEAxWDbHiI14pnG58L+zYG0csjTSexNFmr3PFi9mFG+zpF1eSVW\n"
        "Exhqow48jqTwsBoVy7FiE2fYVbfSmT+LcbwkrjeIPWjG0RouInUCXTSeacpoFSuI\n"
        "LF6adCtRwupC01M1TsihEvz0lZ1Rekw6PnnupgER71FacszjqHg6SIciGCj5B5wy\n"
        "gV9ckQQn3T/Z8Jfnypmd4ZaJlUIaugkGkMKhoqzHdNhCvypjBsf6mwa9hI8ubIf9\n"
        "4zorNNSueQRJhtnl0Vk23AI+W3jn+Ok8PFrHWU1cV4WTRBZQUIL7Mn2wIEbNqb5m\n"
        "j9H8gEdwRcDxCL0KGeCmUuHqRdtqmSPJUcV35wIDAQABAoIBAHAhAPoSd0g/Ehdx\n"
        "2G0JJWkcG6RgQL7ej4vR4guZTOtvCSyEgGyir1noaZCjtTRl7UHliJD7qpOvtU1o\n"
        "IyC+wJvUYmp4jpIV7GWY0Xn/vYrd+Nkl/Nsv91DNl0pC6SnCmIMkSRVPbOdeEv6G\n"
        "NYRuopvd+cFaFycmrKJ1gq7XThAondjtq1CWv/aMdWD84cHYj1aEQO/l1Ypmt6D3\n"
        "8EtdPf4IcTajXFnEiPBHEshmEnEl4nJeSl0B77Toq5N6FuRwMAqL4zzrGfktfYxv\n"
        "b7GDWhMadLH+j2+S9odIEO6PwtYiUm8LL/Kp3oemBpF68E/hngy3M6nKZjPpZsTG\n"
        "n36ejKECgYEA+ayTQxvYo96faMCVooQ8l77h33pMZsCRmsXITsfVj+RxUiwxC3S4\n"
        "fMZc+KV/ovRaVAgUvgw1Jx+t7nMKxLmNacaUHGjgW9mVzfUWbvDKYFIYh2K5rqAe\n"
        "XKJ8C7dbdN8lmT7XJqeKwoqtnIdgq9nZBX9t+iEUwSxliVeP/G/t2XcCgYEAymEV\n"
        "DIkGy/fb2JdhALE3sZvSGKgccu86n9/nIPqDEs6WVh8K6ToPaqgwu7Qvq3Wkazg5\n"
        "wtUV23JTSFT7a74BijHW/T+tT1v4P0bEqDogUdxYPNsyBO3h+bqerGF5gXO+Jvc0\n"
        "IVEsN1fLB7yF3p2BV1N45I6c8/kxtOehO/EH8RECgYEA9npR3rxMwGl0QXttvUSC\n"
        "dOOyJGadqgqNM5fxSSSqRiUlYhPSTxbpUfSZr7XiLULBMZfQslVm27rAgFf913u+\n"
        "iR/K88jtRH+nDe3YOalogHJLWMwztprHnnehsgdPOC9a9NA+oiBgMfGpto9G9iBX\n"
        "yL+mGHoz/QgRLObOdE0jJ70CgYAUo+odqSwESXcmk+nUxD5F4vp+gUbFghicjZ1p\n"
        "A6Oym1dB2VCnkMBZxGWT/0CNYCYwuT9K26irBPHxgplR18MYcRTYJ1JpsZuT+Ie+\n"
        "EI+TScHGEyT607gkXCAqvJZD2aYfGz851qmHy3BtjZCpr66tVVoTFWh0iYnERYtw\n"
        "LZj9wQKBgAJg0/PkYqEcBxqWd76tZAtUMQPxKQtJ/RfusDyFbkURgPv7jd2Ze7ID\n"
        "niAbzSj5iDmS5VcLBgtMHPehiu6xSClsOLBtQXFtY1ZHKUxXMmZMZc+x1CZhiv+b\n"
        "Fk03/rvneQxwrsXPE9UiED5t+C24ymqaWqkC2oRgYPlEuAfUhpkE\n"
        "-----END RSA PRIVATE KEY-----\n";

const char *awsDevice_cert =
        "C1,-----BEGIN CERTIFICATE-----\n"
        "MIIDWTCCAkGgAwIBAgIUUs6wuzFf8hkT7Suu+VlacxMkXVAwDQYJKoZIhvcNAQEL\n"
        "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
        "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDcyNDEzMzYy\n"
        "OFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
        "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMVg2x4iNeKZxufC/s2B\n"
        "tHLI00nsTRZq9zxYvZhRvs6RdXklVhMYaqMOPI6k8LAaFcuxYhNn2FW30pk/i3G8\n"
        "JK43iD1oxtEaLiJ1Al00nmnKaBUriCxemnQrUcLqQtNTNU7IoRL89JWdUXpMOj55\n"
        "7qYBEe9RWnLM46h4OkiHIhgo+QecMoFfXJEEJ90/2fCX58qZneGWiZVCGroJBpDC\n"
        "oaKsx3TYQr8qYwbH+psGvYSPLmyH/eM6KzTUrnkESYbZ5dFZNtwCPlt45/jpPDxa\n"
        "x1lNXFeFk0QWUFCC+zJ9sCBGzam+Zo/R/IBHcEXA8Qi9ChngplLh6kXbapkjyVHF\n"
        "d+cCAwEAAaNgMF4wHwYDVR0jBBgwFoAUJRUfVW2bfVNcluvP0GsJYAC0DTEwHQYD\n"
        "VR0OBBYEFBqJ4JJLGUhd1TJrHuDekko3PoOSMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
        "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB4IcOWX5pWsD1lfEjx2qg8aVLX\n"
        "9CjZjx3wRE4MVHxpqKwmfII6gAELK3c5HLMQ2R2W5pPzA0tXeb6zQ6cgr9IozeBI\n"
        "su4FsCNHx4arLGR6os9MGPeHNsDjrcQfvtbQTdlUagEf5KiUhl03YigdUVlf0uzq\n"
        "iWDKjm61+LifRUjfsqs7L6jO6xN0ElSoC0Jm9Sz5cw1p19ZDZQESZcbqmVrWn2IG\n"
        "4fyRpMDzir+qXyzMMZrs6YkxgK/dNXfj8qcSFxrN9r2+yEODztCY3fZ0aOyKZWxo\n"
        "Zu0MK3ig78YoPYVkXZYtVV8HbyuvBeXPOSpIG5A5a93b0RbmIhN9UV57imwM\n"
        "-----END CERTIFICATE-----\n";

#endif

#ifdef USE_RASYNBW02

const char *awsDevicePrivateKey_cert =
        "C2,-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIEogIBAAKCAQEAw5LYchwAKIjhfA/XINKaU12O6GA9cA6ogWmzSSCGCzLDHL9c\n"
        "IVRHhqj4lw6Y7lHjuQBhCtBs7yw36phd5z1iEoHDXAqWNzwStGZRrC0oHf4tFmZ/\n"
        "KNlUcnz60e3m7KZE0k0klc/jWkS16jm5PTiQB/kh8L7KyuYTI6qICxv76RmbmgqR\n"
        "0ssj7hM3GLmnrUT7eJF56HnYrfE5ND85WT0nQRfHdEtskodRGJmP/g2qbTmwTjHI\n"
        "ew4WMHjkS5ampAjsuJuJayC2sFGM6ybUN7rl94x2aMJ5HAUNAT0GOmhifqxZSlKF\n"
        "ygUAwF3YG8QF7q6eUGF7WAUzWKjPZ2IRgBQmEQIDAQABAoIBAQCrkaZKLKBC7xoI\n"
        "sQjD5Z23JqtY1hzvshxHM4gxzdNp/YVHMeTL5Fi/HHrAYfDtmW9R+7AmQYRfKYaA\n"
        "SlyOoL3piGbu1ubC2/N4yQ47In5AGZxKO26wUkAQE+4hvjfIQXC8oSpINtC3fvGr\n"
        "4inMOFO6euc4gNf8uWvu7Qpd3JXY/bDDKJr3OTJiP+eDmGNQLX6/Qd3t19jEAft4\n"
        "8JQYKIPLNrX75vQQrAjAdIHN2xVSWKVGt3wFDBUt/HbmzTw894DSYfa3N4pC0VKs\n"
        "pvM0l9JyvtoY1lKtENVHxbtswxqIGzsC7P3j/+P8I3quTIMxldBQhhlc5r+WbFND\n"
        "N43SDAIBAoGBAPWmjF5sgJRjzsyFLNnhuFyh0x3ZiWTHpW0sw5QEAA5W3FF6QcnV\n"
        "KiIw83AosG3j/WAt6/srbQBrAsA0D/r49ZYXadVyqjyFSNjjHTJHsuXMwC5MRWtd\n"
        "vaJFruX08d4lHMrV8bob90dJlL6vSKQmjSMpGqFhgHJK+yL5MMtPaGwxAoGBAMvQ\n"
        "MdTp6IpVOIPn2EX8GR4KaiUBmrjNM6Jo+HSPIC59VZs3bE23fYxSGCqJQkZEjpr2\n"
        "FG1QBUdvxmbVHPntpRFdHPLOdY2qOklYLMGYEVZ7BW7Yyr8w8WuKU23yX3fqu9Vw\n"
        "aSUxYOfdmeR1ri9s8Y6THYEGP+CYgEnWA0grPT/hAoGBAIHkhUjQmtTcSdd/fA3h\n"
        "+ziQITAhfTMbFnA6QVidHOJ4L5utx+fPna3isHbYLSKOccZPwf5YIhUcgKHdZa3R\n"
        "wxqQWtgQ/89ssu5seV3C5+f3co+IqRRyxvltW2QhN1u9yBHBO0arzv+U+uAxk3mp\n"
        "IOw8qeY3xlGpME538cMvxAmhAoGAH+a8hgj+vvhL2kmlzn6BJ/SVXKl9oENuO5OW\n"
        "oBfu/5famGr6Zse7vQT0h2PonbsJXzP5Sorl6KKUOXBOEsiht7ANYevy9nOYn/ZP\n"
        "OxYkUJb86LbCNV7hyuVh3am8S7bQI75ulsW9100WU24gD4hL3vcj/RgPU4ZLPkNj\n"
        "TB6IfcECfzl++d+J1Pvv11LS5lJDwGVOoJyigkIl0GK4JJuQt6jR9NPFqdvMDk+n\n"
        "3V/hnwAdUyO1HbiMbKORugf0TxDlcWl1EGga1CLuz8XywdJxfgjA4n+87bZ16hl9\n"
        "Cyv01hPxHvUvYsQ7AuS9WUQibtvLyRUKwOs7nTCn/cgRMvXemlY=\n"

        "-----END RSA PRIVATE KEY-----\n";

const char *awsDevice_cert =
        "C1,-----BEGIN CERTIFICATE-----\n"
        "MIIDWTCCAkGgAwIBAgIUdhsTlKhlr40v1M/S61cxeD3A4q8wDQYJKoZIhvcNAQEL\n"
        "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
        "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIzMDcyNjE1MTQ0\n"
        "NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
        "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMOS2HIcACiI4XwP1yDS\n"
        "mlNdjuhgPXAOqIFps0kghgsywxy/XCFUR4ao+JcOmO5R47kAYQrQbO8sN+qYXec9\n"
        "YhKBw1wKljc8ErRmUawtKB3+LRZmfyjZVHJ8+tHt5uymRNJNJJXP41pEteo5uT04\n"
        "kAf5IfC+ysrmEyOqiAsb++kZm5oKkdLLI+4TNxi5p61E+3iReeh52K3xOTQ/OVk9\n"
        "J0EXx3RLbJKHURiZj/4Nqm05sE4xyHsOFjB45EuWpqQI7LibiWsgtrBRjOsm1De6\n"
        "5feMdmjCeRwFDQE9BjpoYn6sWUpShcoFAMBd2BvEBe6unlBhe1gFM1ioz2diEYAU\n"
        "JhECAwEAAaNgMF4wHwYDVR0jBBgwFoAU0xdB/U0a6H8LR8FiVtC/FK1qs+IwHQYD\n"
        "VR0OBBYEFA/mzy63LnDsK5ohrLXdwAx3bmdcMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
        "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAvBqFjlQPt1bl6FU3QMvPgRe3F\n"
        "Ci2VZSQiPyVaCXfSmzeAdRcYuzMlWg/k2aG6J2cuXx9iMlR5nlgY0loCWyM/Pup5\n"
        "4IGvKN0kKF9ZOmgEO4lHJMQn9KWO82CWgbtPCA9w5hCe5E+4WN2WyRbZ3Z3wq9/5\n"
        "rf5ss7X7gKbKFdd+tPXIjJUQLyyOrbMLfgnNnIaMchPW9PUuWrESvnXdB/S8lDDQ\n"
        "PtNP/F7DRPammlFChiDxeQxhcwPXJ++G/Ft5SVd+oDtCjQp7WkPNX/J2BwdLYiIQ\n"
        "vYgtcRQJT8D5Vzjib0m3JlMRVDiOwrFT0Ghz3+lzg39XdHUu/i1F2RGyBpB/\n"
        "-----END CERTIFICATE-----\n";

#endif


#endif // CERTS__H
