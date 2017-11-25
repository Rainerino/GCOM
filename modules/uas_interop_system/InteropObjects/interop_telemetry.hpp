#ifndef INTEROP_TELEMETRY_H
#define INTEROP_TELEMETRY_H

class InteropTelemetry
{

public:

    InteropTelemetry(const double latitude, const double longitude, const double altitudeMsl, const double uasHeading);
    ~InteropTelemetry();

    double getLatitude();
    double getLongitude();
    double getAltitudeMsl();
    double getUasHeading();

private:

    double latitude;
    double longitude;
    double altitudeMsl;
    double uasHeading;
};

#endif // INTEROP_TELEMETRY_H
