/*bài này của Phú, ko dùng module L298 như mình nên chưa define chân*/
#include<Arduino.h>

/*Sử dụng thư viện MPU khác mình*/
#include <Wire.h>
#include <mpu6050.h>
MPU6050 mpu6050;

/*Sử dụng bộ điều khiển PID*/
float i_input;
float d_last;
float target_dir;

/*Chưa define chân cho L298*/

/*băm xung bằng biến speed cho chân 5*/
void motor_A_set( int speed ) {
  if ( speed == 0 ) {
    digitalWrite( 4, HIGH );
    digitalWrite( 5, HIGH );
  } else if ( speed > 0 ) {
    digitalWrite( 4, HIGH );
    analogWrite( 5, speed );

  } else {
    /*chân 5 làm chân băm xung*/
    digitalWrite( 4, LOW );
    analogWrite( 5, 255 - speed );
  }
}


//Set toc do cho DC B
void motor_B_set( int speed ) {
  if ( speed == 0 ) {
    digitalWrite( 8, HIGH );
    digitalWrite( 6, HIGH );
  } else if ( speed > 0 ) {
    digitalWrite( 8, HIGH );
    analogWrite( 6, speed);
  } else {
    /*Chân 6 làm chân băm xung*/
    digitalWrite( 8, LOW );
    analogWrite( 6, 255 - speed  );
  }
}
void motor_forward(int delta)
{
  /*Cái delta này tự đo giống mình, tức là động cơ xe ai cũng ko đều cả*/
  if (delta > 84) delta = 84;
  if (delta < -81)delta = -81;
  motor_A_set(170 + delta);
  motor_B_set(174 - delta);
}

/*MPU 6050 giao tiếp I2C với Arduino, sử dụng 2 chân SDA (Serial Data) và SCL (Serial Clock*/
void mpu6050_begin()  {
  Wire.begin();
  Serial.print("MPU6050: Starting calibration; leave device flat and still ... ");
  int error = mpu6050.begin();
  Serial.println(mpu6050.error_str(error));
}
//doc gia tri truc dz
/*Trục roll: là trục x, trục pitch: là trục y; trục yaw: là trục z*/
float mpu6050_yaw() {
  MPU6050_t data = mpu6050.get();/*Update giá trị trục z*/
  while ( data.dir.error != 0 ) {
    // I suffer from a lot of I2C problems
    Serial.println(mpu6050.error_str(data.dir.error));
    // Reset I2C
    TWCR = 0; Wire.begin();
    // Reread
    data = mpu6050.get();
  }
  return data.dir.yaw;
}
// khoi tao PID
void pid_begin() {
  i_input = 0;
  d_last = 0;
  Serial.println("PID    : ok");
}
/*tính toán PID*/
/*Đầu vào là sai lệch góc z của trục z
Đầu ra là biến steer dùng để băm xung cho động cơ*/
int pid(float error) {
  float p_input;
  float d_input;

  p_input = error;
  i_input = constrain(i_input + error, -50, +50);
  d_input = error - d_last; d_last = error;

  return p_input * 1 + i_input * 0.02 + d_input * 0.02;
  /*Bạn đi mua bánh mì, đưa tiệm bánh mì 10k, người ta đưa bạn ổ bánh mì.
Thì ổ bánh mì là giá trị trả về của hàm tiệm bánh mì… Và 10k là argument (tham số) của hàm.
Từ đó có thể xây dựng lại thành

  int muabanhmi(int vnd) {
 int sobanhmi = vnd/10000; // 10k 1 ổ
 return sobanhmi;
}

int sobanhmidangco = muabanhmi(10000);
-> bây giờ bạn có 1 ổ bánh mì và nó được lưu trong biến sobanhmidangco
*/
}

uint8_t i = 0;
// DOC ZONE /*Ko có gì đặc biệt*/
void zone()
{
  int giatri = analogRead(A0);
  if (analogRead(A0) > 500)
  {
    i++;
    while (analogRead(A0) > 500) {}
    if (i > 5) i = 0;
  }
  Serial.println(i);
}
void setup() {
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.begin(115200);
  Serial.println("\n\nWelcome at PID robot");
  pid_begin();
  mpu6050_begin();
}

void loop() {
  float current_dir;/*Hướng hiện tại*/
  int steer;
  if (i == 3)
  {
    target_dir = 90;/*Hướng muốn xoay, ở đây xoay 90 độ*/
    current_dir = mpu6050_yaw();
    steer = pid( target_dir - current_dir );
    motor_forward(steer);/*băm xung cho động cơ để tiếp tục chạy thẳng*/
  }

  current_dir = mpu6050_yaw();
  Serial.print(" dir="); Serial.print(current_dir, 2);
  Serial.print(" tgt="); Serial.print(target_dir, 2);
  steer = pid( target_dir - current_dir );
  Serial.print(" steer="); Serial.println(steer);
  motor_forward(steer);
  zone();
}
