#include <string.h>
#include "polifitgsl.h"
#include "discharge_prediction.h"

#define filename "input/500g_68_2017.03.13.log.curr"

char get_values( int *time,float *voltage, float *current);

int main()
{
    int time;
    float voltage,current;
    char file[10];
    for(int i=0;i<200;i++)  {sprintf(file, "output/%d", i);  remove(file);}     //remove all files in /output directory
    while(1)
    {
        signed char r=get_values(&time,&voltage,&current);
        battery_status(time,voltage,current,r);
        if(r== -1) break;
    }
    return 0;
}

char get_values( int *time,float *voltage, float *current)
{
    static bool first_run = 0;
    static FILE * file;
    size_t len = 0;
    char temp[100], *line = NULL;
    int comma_pos[4];

    if(first_run == 0)  {file = fopen( filename, "r");  first_run = 1;   }
    char r=getline(&line, &len, file);
    if (r== -1) return r;
    //    printf("%s", line);

    for(int j=0; j<4; j++)
    {
        if(j==0)    comma_pos[0]=strcspn(&line[0],",");
        else        comma_pos[j]=strcspn(&line[comma_pos[j-1]+1],",") + comma_pos[j-1]+1;   //ezzel megtalallom a vesszok helyet
    }
//    for(int j=0;j<4;j++) printf("comma_pos:%d\n", comma_pos[j]);

    strncpy(temp, &line[comma_pos[0]+2], comma_pos[1]-comma_pos[0]-2);  //hova masolja, honnet masolja es mennyit masoljon (a vesszok helye alapjan)
    temp[comma_pos[1]-comma_pos[0]-2] = '\0';
    *time = atoi(temp);                                                 //get time
//    printf("time: %ld\n", *time);

    strncpy(temp, &line[comma_pos[1]+2], comma_pos[2]-comma_pos[1]-2);
    temp[comma_pos[1]-comma_pos[0]] = '\0';
    *voltage = atof(temp);                                              //get voltage
//    printf("voltage: %f\n", *voltage);

    strncpy(temp, &line[comma_pos[2]+2], comma_pos[3]-comma_pos[2]-2);
    temp[comma_pos[1]-comma_pos[0]] = '\0';
    *current = atof(temp);
    return r;                                              //get current
}
