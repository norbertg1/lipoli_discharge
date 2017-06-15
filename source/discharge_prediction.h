#ifndef DISCHARGE_PREDICTION_H_INCLUDED
#define DISCHARGE_PREDICTION_H_INCLUDED

#define MEMORY_SIZE             2500
#define R_INTERNAL_MAX          60          //Ez a feltételezett maximális belso ellenállás
#define CALCULATED_VOLTAGE_SIZE 20000
#define SKIP                    20          //Minden 20. mért érték után fogja csak újraszámolni az akksiidot, belso ellenallast, stb...
#define DEGREE                  2           //Ennyi fokú a polinom amivel közelítjuk
#define BATTERY_EMPTY_VOLTAGE   10.8

void battery_status( int time_measured, float voltage_measuered, float current_measured, signed char EF);

#endif // DISCHARGE_PREDICTION_H_INCLUDED
