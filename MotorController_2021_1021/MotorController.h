#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H


class MotorController
{
private:
    double delta_t;
    double angle;
    double angle_offset;
    double duty_offset;

    double Cf_;
    double Cb_;
    double Df_;
    double Db_;
    double initial_Df_;
    double initial_Db_;

    double duty_max;
    double omega_max;
    double acceleration_max;
    double pre_target_omega;

    Ec *ec_;
    CalPID *sc_pid;
    CalPID *ac_duty_pid;
    CalPID *ac_omega_pid;

protected:
    PwmOut motor_p;
    PwmOut motor_n;

public:
    MotorController(PinName motor_p_, PinName motor_n_,double dt, Ec &ec, CalPID &sc_, CalPID &ac_duty, CalPID &ac_omega);//引数は下行
    //モーター正転、逆転、周期[s]、エンコーダ、速度制御用のPID、角度制御のPID、速度制御利用の角度制御のPID

//////////////////////////////////////////各クラスのコンストラクタ引数で設定はされているので呼び出しは不要。変更したい場合などに
    void period(double s);//pwm周期設定用関数
    void period_ms(int ms);//pwm周期設定用関数
    void period_us(int us);//pwm周期設定用関数
    void setDutyLimit(double max);//CalPIDによるduty比の最大値設定
    void setOmegaLimit(double max);//CalPIDによる角速度[rad/s]の最大値設定
    void setPDParamSc(double kp, double kd);//速度制御のPDゲイン設定用関数
    void setPDParamAcDuty(double kp, double kd);//（duty比による）角度制御のPDゲイン設定用関数
    void setPDParamAcOmega(double kp, double kd);//（速度制御による）角度制御のPDゲイン設定用関数
    void setDeltaTime(double dt);//制御周期の設定用関数
    void setAccelMax(double a_max);
    void setDutyOffset(double duty_off);

//////////////////////////////////////////実際に使うときに呼び出す関数
    double getAngle();
    void calibration(double angle_calibration);
    void setEquation(double cf, double df, double cb, double db);//速度制御のC値、D値の設定用。設定が必要
    void turn(double duty);//モーターにduty比入力する
    void AcDuty(double target_angle);//PD算出されたdutyによる角度制御
    void AcOmega(double target_angle);//PD算出された角速度による角度制御。速度制御を利用している
    void Sc(double target_omega_input);//速度制御
    double calSc(double target_omega_input);//速度制御
    void stop();//duty=0を入力
    void reset();

};

#endif