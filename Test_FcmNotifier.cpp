#include "Tests.h"
#include "FcmNotifier.h"
#include "Logger.h"
#include <iostream>

void Test_FcmNotifier()
{
    FcmNotifier fcm("C:\\dev\\shserver-alarm-firebase-adminsdk-fbsvc-f566f80912.json");

    std::cout << "Sende Test-Push...\n";

    bool success = fcm.SendToToken(
        "d_FNmei9QXOk283cz3_yc8:APA91bHh3IFRDnrgcwTprL5StdASoHgFO-V2iCd25g9xblyGQaZdRb5UwiCZIbSd-uvCupiS0z0wPZYYeU9BBORJSrN3PuJItubAJWcCg4sDDt2WGuW36tM",
        "ALARM",
        "Test-Nachricht aus dem C++-Backend!"
    );

    std::cout << (success ? "Erfolgreich gesendet!" : "Senden fehlgeschlagen") << "\n";
}