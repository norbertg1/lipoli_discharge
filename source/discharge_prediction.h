#ifndef DISCHARGE_PREDICTION_H_INCLUDED
#define DISCHARGE_PREDICTION_H_INCLUDED

#define MEMORY_SIZE             1000
#define VOLTAGE_SIZE            60
#define CALCULATED_VOLTAGE_SIZE 5000
#define SKIP                    20
#define DEGREE                  6

void battery_status( int time_measured, float voltage_measuered, float current_measured);

#endif // DISCHARGE_PREDICTION_H_INCLUDED
