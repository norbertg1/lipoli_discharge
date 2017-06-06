#include <string.h>

#include "linreg.h"
#include "polifitgsl.h"
#include "discharge_prediction.h"

void print_to_file( int time,float voltage, int nr);

void battery_status( int time_measured, float voltage_measuered, float current_measured)
{
    static int loop = 0;
    int R_internal;
    double time[MEMORY_SIZE];
    double voltage[VOLTAGE_SIZE][MEMORY_SIZE],current[MEMORY_SIZE],
            voltage_calculated[CALCULATED_VOLTAGE_SIZE],time_calculated[CALCULATED_VOLTAGE_SIZE], coeff[DEGREE];
    double a,b,r,r_max;

    memmove(time+1, time, sizeof(time)-sizeof(*time) );
    memmove(current+1, current, sizeof(current)-sizeof(*current) );
    for(int i=0;i<VOLTAGE_SIZE;i++) {
        memmove(&voltage[i][1], &voltage[i][0], sizeof(voltage[i])-sizeof(*voltage[i]) );
        voltage[i][0] = voltage_measuered + current_measured*(float)i/1000;
        }
    time[0] = time_measured/1000000;

    if(loop%SKIP == 0 && loop>=MEMORY_SIZE) {
        r_max=0;
        for(int i=0;i<VOLTAGE_SIZE; i++){
            linreg(MEMORY_SIZE, time, voltage[i], &a, &b, &r);
            if(abs(r)>r_max) {
                r_max=abs(r),
                R_internal = i;
                voltage_calculated[(loop-MEMORY_SIZE)/SKIP] = voltage[i][0];
                time_calculated[(loop-MEMORY_SIZE)/SKIP]= time[0];
           }
        }
        if(loop>1402)  polynomialfit((loop-MEMORY_SIZE)/SKIP, DEGREE, time_calculated, voltage_calculated, coeff);
        if(loop>MEMORY_SIZE)    print_to_file(time[0], voltage_calculated[loop/SKIP], 101); //print to file time vs voltage corrected with internal R dependence
        printf("loop: %d R_internal: %d\n", loop, R_internal);
        print_to_file(loop, R_internal, 100);           //print to file time vs internal R dependence
        print_to_file(loop, voltage_measuered, 102);    //print to file time vs voltage measured (raw) dependence
    }
    if(loop == 5000){
        for(int i=0;i<VOLTAGE_SIZE;i=i+2){
            for(int j=0;j<MEMORY_SIZE;j++){
                print_to_file(time[j], voltage[i][j], i);
            }
            char ret=linreg(MEMORY_SIZE, time, voltage[i], &a, &b, &r);
            printf("i:%d ret:%d a:%f b:%f r:%f\n", i, ret, a, b, r);
        }
        polynomialfit((loop-MEMORY_SIZE)/SKIP-1, DEGREE, time_calculated, voltage_calculated, coeff);
        FILE *gnuplot = popen("gnuplot-x11", "w");


        for(int i=0;i<(loop-MEMORY_SIZE)/SKIP-1;i++) print_to_file(time_calculated[i], voltage_calculated[i], 103);
        for(int i=0; i < DEGREE; i++) {
            printf("coef:%lf\n", coeff[i]);
            fprintf(gnuplot, "%c=%lf\n ", 97+i, coeff[i]);
            printf("%c=%lf\n ", 97+i, coeff[i]);
        }
        for(int i=DEGREE; i<11; i++) {fprintf(gnuplot, "%c=%lf\n ", 97+i, 0.0); printf("%c=%lf\n ", 97+i, 0.0);}
        fprintf(gnuplot, "f(x) = a+b*x+c*x**2+d*x**3+e*x**4+f*x**5+g*x**6+h*x**7+i*x**8+j*x**9+k*x**10\n set xrange [0:1000]\n plot f(x)\n");
        printf("f(x) = a+b*x+c*x**2+d*x**3+e*x**4\n plot f(x)\n");
        fprintf(gnuplot, "set term \"wxt\" 1\nplot \"output/103\"\n");
        fflush(gnuplot);
        getchar();
    }
    loop++;
}

void print_to_file( int time,float voltage, int nr)
{
    FILE * file;
    char filename[25];
    sprintf(filename, "output/%d", nr);
    file = fopen( filename , "a");
    fprintf(file, "%ld %f\n",time, voltage);
    fclose(file);
}
