#include "mbed.h"
#include "EC.h"
#include "CalPID.h"
#include "MotorController.h"

MotorController::MotorController(PinName motor_p_, PinName motor_n_,double dt, Ec &ec, CalPID &sc_, CalPID &ac_duty, CalPID &ac_omega) : motor_p(motor_p_), motor_n(motor_n_)
{
    ec_ = &ec;
    sc_pid = &sc_;
    ac_duty_pid = &ac_duty;
    ac_omega_pid = &ac_omega;
    motor_p.period_us(50);
    motor_n.period_us(50);
    setDeltaTime(dt);
    angle_offset=0;
    calibration(0);
    setAccelMax(200);
    setDutyOffset(0);
    pre_target_omega=0;
}
double MotorController::getAngle()
{
    angle=ec_->getRad();
    angle+=angle_offset;
    return angle;
}
void MotorController::calibration(double angle_calibration)
{
    angle_offset=angle_calibration;
    ec_->reset();
}
void MotorController::period(double s)
{
    motor_p.period(s);
    motor_n.period(s);
}
void MotorController::period_ms(int ms)
{
    motor_p.period_ms(ms);
    motor_n.period_ms(ms);
}
void MotorController::period_us(int us)
{
    motor_p.period_us(us);
    motor_n.period_us(us);
}
void MotorController::setDutyLimit(double max)
{
    sc_pid->setMaxValue(max);
    ac_duty_pid->setMaxValue(max);
}
void MotorController::setOmegaLimit(double max)
{
    ac_omega_pid->setMaxValue(max);
}
void MotorController::setPDParamSc(double kp, double kd)
{
    sc_pid->setParameter(kp, 0, kd);
}
void MotorController::setPDParamAcDuty(double kp, double kd)
{
    ac_duty_pid->setParameter(kp, 0, kd);
}
void MotorController::setPDParamAcOmega(double kp, double kd)
{
    ac_omega_pid->setParameter(kp, 0, kd);
}
void MotorController::setDeltaTime(double dt)
{
    delta_t=dt;
    sc_pid->setDELTA_T(dt);
    ac_duty_pid->setDELTA_T(dt);
    ac_omega_pid->setDELTA_T(dt);

}
void MotorController::setEquation(double cf,double df,double cb,double db)
{
    Cf_=cf;
    if(cb>0)Cb_=cb;
    else Cb_=-cb;
    initial_Df_=df;
    initial_Db_=-db;
    Df_=initial_Df_;
    Db_=initial_Db_;
}
void MotorController::setDutyOffset(double duty_off)
{
    duty_offset=duty_off;
}
void MotorController::setAccelMax(double a_max)
{
    acceleration_max=a_max*delta_t;
}
void MotorController::turn(double duty)
{
    if (duty<=1.0&&duty >= 0) {
        motor_p = duty;
        motor_n = 0;
    } else if (duty < 0&&duty>=-1.0) {
        motor_p = 0;
        motor_n = -duty;
    }
}
void MotorController::Sc(double target_omega_input)
{
    double duty=calSc(target_omega_input);
    check_duty = duty;
    turn(duty);

}
double MotorController::calSc(double target_omega_input)
{
    ///////////////////////////////////////////////加速度制限
    double target_omega=target_omega_input;
    if((target_omega_input-pre_target_omega)>acceleration_max) {
        target_omega=pre_target_omega+acceleration_max;
    } else if((target_omega_input-pre_target_omega)<-acceleration_max) {
        target_omega=pre_target_omega-acceleration_max;
    }
    ///////////////////////////////////////////////PD計算
    ec_->calOmega();
    double omega = ec_->getOmega();
    double devia = target_omega - omega;
    double pid = sc_pid->calP_D(devia,ec_->getAcceleration());
    double duty_;
    if (target_omega > 0.0) {
        Df_ += Cf_ * pid;
        duty_ = Cf_ * target_omega + Df_;
    } else if (target_omega < -0.0) {
        Db_ += Cb_ * pid;
        duty_ = Cb_ * target_omega + Db_;
    } else{
        duty_ = 0;
    }

//duty_の値の絶対値が1を超えないようにする
    if(1 < duty_){
        duty_ = 1;
    }else if ( duty_ < -1)
    {
        duty_ = -1;
    }
    

    pre_target_omega=target_omega;
    return duty_;
}
void MotorController::AcDuty(double target_angle)
{
    getAngle();
    double devia = target_angle - angle;
    double duty_omega = ac_duty_pid->calP_D(devia,ec_->getOmega());
    turn(duty_omega);
}
void MotorController::AcOmega(double target_angle)
{
    getAngle();
    double devia = target_angle - angle;
    double omega = ac_omega_pid->calP_D(devia,ec_->getOmega());
    double duty_offset_=duty_offset*cos(angle);

    double duty=calSc(omega)+duty_offset_;
    //duty=calSc(omega);

    check_duty = duty;
    turn(duty);
}
void MotorController::stop()
{
    motor_p = 0;
    motor_n = 0;
}
void MotorController::reset()
{
    ec_->reset();
    Df_=initial_Df_;
    Db_=initial_Db_;
}

