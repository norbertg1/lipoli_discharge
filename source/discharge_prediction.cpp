#include <string.h>
#include "linreg.h"
#include "polifitgsl.h"
#include "discharge_prediction.h"

void print_to_file( int time,float voltage, int nr);
void gnuplot();
void gnuplot_coeff(int loop, double coeff[]);

void battery_status( int time_measured, float voltage_measuered, float current_measured, signed char EF)
{
    static int loop = 0;
    int R_internal;
    double time[MEMORY_SIZE];
    double  a,b,r,r_max,discharge_time, time_left,
            voltage[R_INTERNAL_MAX][MEMORY_SIZE],current[MEMORY_SIZE],                                                  //voltage[R_INTERNAL_MAX][MEMORY_SIZE] Túl nagy lehetseges, hogy optimalizálható!!!!!
            voltage_calculated[CALCULATED_VOLTAGE_SIZE],time_calculated[CALCULATED_VOLTAGE_SIZE], coeff[DEGREE];

    memmove(time+1, time, sizeof(time)-sizeof(*time) );                                         //shift arrays by 1 position and fill the [0] positions with new measured values (I,U,t,mAH, ...etc)
    memmove(current+1, current, sizeof(current)-sizeof(*current) );
    for(int i=0;i<R_INTERNAL_MAX;i++) {
        memmove(&voltage[i][1], &voltage[i][0], sizeof(voltage[i])-sizeof(*voltage[i]) );
        voltage[i][0] = voltage_measuered + current_measured*(float)i/1000;
        }
    time[0] = time_measured/1E6;

    if(loop%SKIP == 0 && loop>=MEMORY_SIZE) {                                                         //when enough data is collected, calculate the internal resistance
        r_max=0;
        for(int i=0;i<R_INTERNAL_MAX; i++){                                                         //Végigmegyek 0mOhm belso ellenálástól R_INTERNAL_MAX (jelenleg 60mOhm) belso ellenállásig 1mOhm lépésenkben.
            linreg(MEMORY_SIZE, time, voltage[i], &coeff[1], &coeff[0], &r);                       //Minden lépésben kiszámolom az adott szakasz regresszióját (r). Az adott szakasz MEMORY_SIZE hosszúságú.
            if(abs(r)>r_max) {                                                                    //ahol a legnagyobb az r az ahhoz tartozó i-t tekintem az ideális belso ellenállásnak (R_internal = i)
                r_max=abs(r);
                R_internal = i;
                voltage_calculated[(loop-MEMORY_SIZE)/SKIP] = voltage[i][0];                    //Itt kiszámolom a belso ellenállással módosított feszültséget
                time_calculated[(loop-MEMORY_SIZE)/SKIP]= time[0];
                /*if(DEGREE == 2){
                    discharge_time = (BATTERY_EMPTY_VOLTAGE-coeff[0])/coeff[1];
                    time_left = (BATTERY_EMPTY_VOLTAGE-coeff[0])/coeff[1]-time_calculated[(loop-MEMORY_SIZE)/SKIP];
                    }*/
           }
        }
        if(/*DEGREE > 2 &&*/ loop>1.5*MEMORY_SIZE)  {
            polynomialfit((loop-MEMORY_SIZE)/SKIP, DEGREE, time_calculated, voltage_calculated, coeff);
            discharge_time = (BATTERY_EMPTY_VOLTAGE-coeff[0])/coeff[1];
            time_left = (BATTERY_EMPTY_VOLTAGE-coeff[0])/coeff[1]-time_calculated[(loop-MEMORY_SIZE)/SKIP];
            print_to_file(time[0], discharge_time, 103);
        }
//Innet ez amolyan debug rész
        print_to_file(time[0], R_internal, 100);                                    //print to file time vs internal R dependence
        print_to_file(time[0], voltage_calculated[(loop-MEMORY_SIZE)/SKIP], 101);   //print to file time vs voltage corrected with internal R dependence
        print_to_file(time[0], voltage_measuered, 102);                             //print to file time vs voltage measured (raw) dependence
        //print_to_file(time[0], discharge_time, 103);                                //print to file time vs discharge_time dependence

        printf("loop: %d R_internal: %d discharge time: %f, time left: %f\n", loop, R_internal, discharge_time, time_left );
    }
    if(loop ==3000){
        for(int i=0;i<R_INTERNAL_MAX;i=i+2){
            for(int j=0;j<MEMORY_SIZE;j++){
                print_to_file(time[j], voltage[i][j], i);       //kiírom az 5000es pontban (tehát ha 5000 mért értéket kapok) a 0 - 60mOhm ellenállásáal módosított feszültséget.
            }                                                   //Ez az a grafikon amibol a regressziot számolom
            linreg(MEMORY_SIZE, time, voltage[i], &a, &b, &r);
            printf("i:%d a:%f b:%f r:%f\n", i, a, b, r);
        }
        polynomialfit((loop-MEMORY_SIZE)/SKIP-1, DEGREE, time_calculated, voltage_calculated, coeff);
        gnuplot_coeff(loop, coeff);
        getchar();
    }
    if(EF == -1){
        gnuplot();
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

void gnuplot()
{
    FILE *gnuplot = popen("gnuplot-x11", "w");
    char file[10];

    for(int i=0;i<=3;i++) {
        sprintf(file,"%d", 100+i);
        //printf("set terminal \"png\"\n set output \"output/pics/%s.png\"\n  plot \"output/%s\"\n ",file ,file);
        fprintf(gnuplot, "set terminal \"png\"\n set output \"output/pics/%s.png\"\n  plot \"output/%s\"\n ",file ,file);
    }
    fflush(gnuplot);
}

void gnuplot_coeff(int loop, double coeff[]){
    FILE *gnuplot = popen("gnuplot-x11", "w");
    for(int i=0; i < DEGREE; i++) {
        printf("coef:%lf\n", coeff[i]);
        fprintf(gnuplot, "%c=%lf\n ", 97+i, coeff[i]);
        printf("%c=%lf\n ", 97+i, coeff[i]);
    }
    for(int i=DEGREE; i<11; i++) {fprintf(gnuplot, "%c=%lf\n ", 97+i, 0.0); printf("%c=%lf\n ", 97+i, 0.0);}
    fprintf(gnuplot, "f(x) = a+b*x+c*x**2+d*x**3+e*x**4+f*x**5+g*x**6+h*x**7+i*x**8+j*x**9+k*x**10\n set xrange [100:200]\n plot f(x), \"output/101\"\n ");
    fflush(gnuplot);
}
