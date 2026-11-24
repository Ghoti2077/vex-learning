/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       Ghoti                                                     */
/*    Created:      2026/11/24 21:10:00                                       */
/*    Description:  V5 project - PUSH BACK 手动，自己写                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"

using namespace vex;

// A global instance of vex::brain used for printing to the V5 brain screen
vex::brain       Brain;

// define your global instances of motors and other devices here


controller Controller = controller(primary);

motor left_motor_1 = motor(PORT6,ratio6_1,false);
motor left_motor_2 = motor(PORT7,ratio6_1,true);
motor left_motor_3 = motor(PORT8,ratio6_1,true);
motor left_motor_4 = motor(PORT9,ratio6_1,false);
motor left_motor_5 = motor(PORT10,ratio6_1,true);
motor_group left_drive = motor_group(left_motor_1,left_motor_2,left_motor_3,left_motor_4,left_motor_5);

motor right_motor_1 = motor(PORT14,ratio6_1,true);
motor right_motor_2 = motor(PORT15,ratio6_1,false);
motor right_motor_3 = motor(PORT16,ratio6_1,false);
motor right_motor_4 = motor(PORT17,ratio6_1,true);
motor right_motor_5 = motor(PORT18,ratio6_1,false);
motor_group right_drive = motor_group(right_motor_1,right_motor_2,right_motor_3,right_motor_4,right_motor_5);


inertial imu = inertial(PORT11);//陀螺仪


digital_out hook = digital_out(Brain.ThreeWirePort.E);//钩子
digital_out mark = digital_out(Brain.ThreeWirePort.A);//面罩


motor intake_motor = motor(PORT2,ratio6_1,false);//前吸
motor intake1 = motor(PORT4,ratio6_1,true);//下通道
motor intake2 = motor(PORT3,ratio6_1,true);//中通道
motor intake3 = motor(PORT5,ratio6_1,true);//上通道
motor outputmotor = motor(PORT21,ratio6_1,true);//输出通道
motor sortmotor = motor(PORT12,ratio6_1,false);//筛选电机

optical sort = optical(PORT20);//颜色传感器


//底盘手动控制
void chassis_control(){

}
//吸球
void intake_ball(){

}
//吐高桥
void high_bridge(){

}
//吐中桥
void mid_bridge(){

}
//吐低桥
void low_bridge(){

}
//钩子
void hook_control(){

}
//遥控器屏幕的数据打印
void display(){

}


int main() {

imu.calibrate();//开始校准
while(imu.isCalibrating())wait(20,msec);//等待校准

thread display_thread(display);


while (true) {
    chassis_control();
    intake_ball();
    high_bridge();
    mid_bridge();
    low_bridge();
    hook_control();
    wait(10,msec);
}
}
