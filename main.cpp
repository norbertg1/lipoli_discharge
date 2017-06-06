#include <string.h>
#include "polifitgsl.h"
#include "discharge_prediction.h"

void get_values( int *time,float *voltage, float *current);

int main()
{
    int time;
    float voltage,current;
    while(1)
    {
        get_values(&time,&voltage,&current);
        //printf("time:%d voltage:%f current:%f\n",time,voltage,current);
        battery_status(time,voltage,current);
    }

    return 0;
}

void get_values( int *time,float *voltage, float *current)
{
    static bool first_run = 0;
    static FILE * file;
    size_t len = 0;
    char *line = NULL;
    int comma_pos[4];
    if(first_run == 0)
    {
        file = fopen( "test3.curr", "r");
        first_run = 1;
    }
    getline(&line, &len, file);
//    printf("%s", line);
    char temp[100];

    for(int j=0; j<4; j++)
    {
        if(j==0)    comma_pos[0]=strcspn(&line[0],",");
        else        comma_pos[j]=strcspn(&line[comma_pos[j-1]+1],",") + comma_pos[j-1]+1;   //ezzel megtalallom a vesszok helyet
    }
//    for(int j=0;j<4;j++) printf("comma_pos:%d\n", comma_pos[j]);

    strncpy(temp, &line[comma_pos[0]+2], comma_pos[1]-comma_pos[0]-2);  //hoca kopirozza, honnet koporizzon es mennyit koporozzon (a vesszok helye alapjan)
    temp[comma_pos[1]-comma_pos[0]-2] = '\0';
    *time = atoi(temp);
//    printf("time: %ld\n", *time);

    strncpy(temp, &line[comma_pos[1]+2], comma_pos[2]-comma_pos[1]-2);  //get voltage
    temp[comma_pos[1]-comma_pos[0]] = '\0';
    *voltage = atof(temp);
//    printf("voltage: %f\n", *voltage);

    strncpy(temp, &line[comma_pos[2]+2], comma_pos[3]-comma_pos[2]-2);  //get current
    temp[comma_pos[1]-comma_pos[0]] = '\0';
    *current = atof(temp);
}

