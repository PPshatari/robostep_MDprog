#include "mbed.h"
#include "EC.h"
#include "CalPID.h"
#include "MotorController.h"

#define GEER_EC 26.0

#define RESOLUTION 500
#define DELTA_T 0.001
#define DUTY_MAX 0.8
#define OMEGA_MAX 15

#define TIME_STOP 4.0

#include "interface.h"
Serial pc(USBTX, USBRX);
Interface interface(USBTX,USBRX);

DigitalOut led[]= {
    (LED1),
    (LED2),
    (LED3),
    (LED4)
};

//CalPID speed_pid(0.003,0,0.0000130,DELTA_T,DUTY_MAX);    //速度制御のPID
CalPID speed_pid(0.0151,0.00302,0.000001888,DELTA_T,DUTY_MAX);    //速度制御のPID
CalPID angle_duty_pid(0,0,0,DELTA_T,DUTY_MAX);          //
CalPID angle_omega_pid(4,0.03933,0.000001208,DELTA_T,OMEGA_MAX);//角度制御のPID

DigitalOut out(PB_4);
DigitalOut gnd(PB_10);

Ec4multi ec_steer(PB_5,PB_3,RESOLUTION,GEER_EC);
MotorController motor(PA_5,PA_6,DELTA_T,ec_steer,speed_pid,angle_duty_pid,angle_omega_pid);

float angle_steerA=0;
float omega_steerA=0;

/////////////////////////////////////////////////////////////データ記録まわり
#define NUM_DATA 2000
int data_count=0;
float data_angle[NUM_DATA]= {};
float data_omega[NUM_DATA]= {};
float data_duty[NUM_DATA] = {};
void saveData()
{
    if(data_count<NUM_DATA) {
        data_duty[data_count] = motor.check_duty;
        data_angle[data_count]=motor.getAngle();
        data_omega[data_count]=ec_steer.getOmega();
        data_count++;
    }
}
void printData()
{
    motor.stop();
    for(int i=0; i<data_count; i++) {
        pc.printf("%f\t%f\t%f\r\n",data_duty[i],data_angle[i],data_omega[i]);
/*        wait(0.01);*/
    }
    data_count=0;
}
////////////////////////////////////////////////////////////////////////////////

void loop()
{
    //motor.AcOmega(angle_steerA);
    motor.Sc(omega_steerA);

    saveData();
}

Timer timer_loop;
Timer timer_stop;

int main ()
{
    out = 1;
    gnd = 0;
    
    //motor.setEquation(0.001391,0.016545,-0.001861,0.000664);//555 Ec4mulitiで計測
    motor.setEquation(0.000585,0.002436,-0.001167,0.001483);//555 Ec1multiで計測
    motor.setEquation(0.01,0,0.01,0);//pidで頑張る用    
    motor.setAccelMax(400);
    wait(0.5);
    while(1) {
    //    printf("%d\r\n",ec_steer.getCount());
//        wait(0.5);
//    }
    int state=0;
 
    while(1) {
        if(state==0) {
            float input_target=interface.AskNum("put target:");
            angle_steerA=input_target;
            omega_steerA=input_target;

            state++;
            timer_loop.reset();
            timer_loop.start();
            timer_stop.reset();
            timer_stop.start();
        } else if(state==1) {
            if((double)timer_loop.read()>DELTA_T) {//タイマーで制御周期を作っている
                loop();
                timer_loop.reset();
            }
            if((double)timer_stop.read()>TIME_STOP) {//何秒かしたら停止,ログを表示
                motor.stop();
                printData();
                state=0;
            }
        }
        //wait_us(0);
    }
}
}

