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


inertial imu = inertial(PORT1);//陀螺仪


digital_out hook = digital_out(Brain.ThreeWirePort.E);//钩子
digital_out mark = digital_out(Brain.ThreeWirePort.A);//面罩


motor intake_motor = motor(PORT2,ratio6_1,false);//前吸
motor intake1 = motor(PORT4,ratio6_1,false);//下通道
motor intake2 = motor(PORT3,ratio6_1,true);//中通道
motor intake3 = motor(PORT5,ratio6_1,true);//上通道
motor outputmotor = motor(PORT21,ratio6_1,true);//输出通道
motor sortmotor = motor(PORT12,ratio6_1,false);//筛选电机

optical sort = optical(PORT20);//颜色传感器

//底盘手动控制
void chassis_control(){
        float straight     = Controller.Axis3.position();   //摇杆前后
        float turn         = Controller.Axis1.position();   //摇杆左右

        //死区
        if(straight< 5 && straight > -5)
        {
            straight = 0;
        }
        if(turn < 6 && turn > -6)
        {
            turn = 0;
        }

        //计算前进和转向
        float left_output  = straight + turn;
        if(left_output > 100.0) left_output =  100.0;
        if(left_output < -100.0)left_output = -100.0;
        float right_output = straight - turn;
        if(right_output > 100.0) right_output =  100.0;
        if(right_output < -100.0)right_output = -100.0;
        //电机执行
        left_drive.spin(fwd,left_output,pct);
        right_drive.spin(fwd,right_output,pct);
        if( (straight == 0)&& (turn == 0 ) ){
            left_drive.stop();
            right_drive.stop();
        }

}

const double IN_PER_DEG = 3.14159266 * 2.75 * (43.0/50.0)/360.0; // = 0.020639  电机转一度走过的距离


double dist_in = 0;  //累积里程，单位英寸

void update_distance(){
    static double last_l = 0;   //上次左轮角度读数
    static double last_r = 0;   //上次右轮角度读数

    double l = left_motor_1.position(degrees);  //现在左轮角度
    double r = right_motor_1.position(degrees); //现在右轮角度

    dist_in +=((l - last_l) + (r - last_r))/2.0 * IN_PER_DEG;
    last_l = l;
    last_r = r;
}


//右边电机平均温度
double temp_right(){
    double m =  ( right_motor_1.temperature(celsius)+ right_motor_2.temperature(celsius)+right_motor_3.temperature(celsius)+right_motor_4.temperature(celsius)+right_motor_5.temperature(celsius))/5;

   
    return m;
}
//左边电机平均温度
double temp_left(){
     double m =  ( left_motor_1.temperature(celsius)+ left_motor_2.temperature(celsius)+left_motor_3.temperature(celsius)+left_motor_4.temperature(celsius)+left_motor_5.temperature(celsius))/5;

    return m;
}

//筛选
//bool detect = sort.isNearObject();
// bool color_sort(){
//     double hue = sort.hue();
//     //double bri = sort.brightness();
   
//     static int time = 0; //延时，防止球还没有吐出去就继续吸(不能放在detect里，否则time不减)
//     if(time > 0)
//     {
//         time--;
//         return false;
//     }

//     if(hue >= 180 && hue <= 240)
//     {
//         time = 40;
//         return false;
//     }

//     return true;

// }
bool color_sort(){
    double hue = sort.hue();
    static int time = 0;
    static int blue_cnt = 0;

    bool is_blue = (hue >= 160 && hue <= 220);

    if(is_blue){
        blue_cnt++;
    } else {
        blue_cnt = 0;        // 只要有一帧不是蓝，立刻清零
    }

    if(blue_cnt >= 4){       // 连续 4 帧(40ms)都蓝，才认
        time = 30;           // 球还在，继续刷 → 一直吐
        return false;
    }

    if(time > 0){            // 球走了，再吐 300ms 送出门
        time--;
        return false;
    }

    return true;
}
//吸球 L1
void intake_ball(){
    if(color_sort())
    {
        intake_motor.spin(fwd,100,pct);
        intake1.spin(fwd,50,pct);
        intake2.spin(reverse,100,pct);
        intake3.spin(reverse,100,pct);
        outputmotor.stop();
        sortmotor.spin(fwd,1,pct);
        mark.set(true);
    }
    else{
        intake_motor.stop();
        intake1.spin(fwd,50,pct);
        intake2.stop();
        intake3.stop();
        outputmotor.stop();
        sortmotor.spin(reverse,100,pct);
        mark.set(true);
    }
    
    
}
//吐高桥 R1
void high_bridge(){
    intake_motor.stop();
    intake1.stop();
    intake2.spin(reverse,60,pct);
    intake3.spin(reverse,100,pct);
    outputmotor.spin(reverse,100,pct);
    sortmotor.stop();
    mark.set(false);

}
//吐中桥 L2
void mid_bridge(){
    intake_motor.stop();
    intake1.stop();
    intake2.spin(reverse,100,pct);
    intake3.spin(fwd,100,pct);
    outputmotor.spin(fwd,100,pct);
    sortmotor.stop();
    mark.set(true);
    
   

}
//吐低桥 A
void low_bridge(){
    intake_motor.spin(reverse,100,pct);
    intake1.spin(reverse,100,pct);
    intake2.spin(fwd,100,pct);
    intake3.spin(fwd,100,pct);
    outputmotor.spin(fwd,100,pct);
    sortmotor.stop();
    mark.set(true);
    
}



//吐吸球函数函数统筹
void ball_mechanisms(){
    if(Controller.ButtonR1.pressing()) {
        high_bridge();
    }  
    else if(Controller.ButtonL2.pressing()){
         mid_bridge();
    }
    else if(Controller.ButtonA.pressing()) {
        low_bridge();
    }
    else if(Controller.ButtonL1.pressing()) {
        intake_ball();
    }
    else{
        intake_motor.stop();
        intake1.stop();     
        intake3.stop();       
        intake2.stop();    
        outputmotor.stop();
        sortmotor.stop();
    }
}
//钩子
void hook_control(){
    static bool last = false;//上一次按键状态
    bool now  = Controller.ButtonR2.pressing();//这一次按键状态

    static bool on = false;//钩子状态

    //状态翻转
    if(now && !last){
        on = !on;
        hook.set(on);
    }
    last = now;
}
//遥控器屏幕的数据打印
void display(){
    while(true){
        Controller.Screen.clearScreen();
        Controller.Screen.setCursor(1,1);   //移动光标
        Controller.Screen.print("TR: %.0fC TL:%.0fC", temp_right(),temp_left());

        Controller.Screen.setCursor(2,1);
        Controller.Screen.print("D: %.1fin",dist_in);

        Controller.Screen.setCursor(3,1);
        Controller.Screen.print("H:%.0f",imu.heading());
        //Controller.Screen.print("H%d C%u", (int)sort.hue(), (uint32_t)sort.color());
        //Controller.Screen.print("A1 %d A3 %d", Controller.Axis1.position(), Controller.Axis3.position());
        //Brain.Screen.setCursor(3,1);
       //Brain.Screen.print("IMU %d %d %.1f %.1f", imu.installed(), imu.isCalibrating(), imu.rotation(), imu.heading());
        

        wait(50,msec);


    }
}





int main() {
//第一次自检
 Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1,1);
    Brain.Screen.print("L %d%d%d%d%d R %d%d%d%d%d",
        left_motor_1.installed(), left_motor_2.installed(), left_motor_3.installed(),
        left_motor_4.installed(), left_motor_5.installed(),
        right_motor_1.installed(), right_motor_2.installed(), right_motor_3.installed(),
        right_motor_4.installed(), right_motor_5.installed());
    Brain.Screen.setCursor(2,1);
    Brain.Screen.print("M %d%d%d%d%d%d",
        intake_motor.installed(), intake1.installed(), intake2.installed(),
        intake3.installed(), outputmotor.installed(), sortmotor.installed());

imu.calibrate();//开始校准
while(imu.isCalibrating())wait(20,msec);//等待校准

thread display_thread(display);


while (true) {
    //底盘手动控制
    chassis_control();
    //里程数计算
    update_distance();
    ball_mechanisms();
    hook_control();
    wait(10,msec);
}
}
