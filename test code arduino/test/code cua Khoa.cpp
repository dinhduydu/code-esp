#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
//#include <Servo.h>
//#include <NewPing.h>
#include <Queue.h>

/*Khai báo thư viện MPU tockn*/
#include <MPU6050_tockn.h>
#include <Wire.h>

//#define BLYNK_PRINT Serial
////#include <ESP8266WiFi.h>
////#include <BlynkSimpleEsp8266.h>

MPU6050 mpu6050(Wire);
int z,z0,z1;
// khai bao dong co
// motor A
#define enA 4
#define in1 12
#define in2 11
//motor B
#define in3 13
#define in4 10
#define enB 3
#define hongngoai 7

//#include <Servo.h>
//Servo myservo;  // create servo object to control a servo
//int pos = 0;// variable to store the servo position
int co_vat_can;

/******** khai báo chân input/output**************/
#define trig 6    // chân trig của SRF-05.
#define echo 5 
unsigned long t;
//int thoigian;
int kc; // biến đo thời gian         // biến lưu khoảng cách 
int a,speed_A,speed_B;
int i,n,g;
int zone,K=80;


void dithang(void *para);
void dokhoangcach(void *para);
void dogoc(void *para);
void tranhvatcan(void *para);
void nhanbietzone(void *para);

 TaskHandle_t Task_Handle_1;
 TaskHandle_t Task_Handle_2;
 TaskHandle_t Task_Handle_3;
 TaskHandle_t Task_Handle_4;
 TaskHandle_t Task_Handle_5;
// int khoangcach;                      //Biến giá trị khoang cach
// unsigned int queue; 


 float z2;
 int PID_value,PID_value0;
 
QueueHandle_t queue_vat_can;
QueueHandle_t queue_goc;
QueueHandle_t queue_n;

/*Chỉ sử dụng 2 Task đi thẳng với đo góc*/
void setup() 
      {
        // put your setup code here, to run once:
    xTaskCreate (dithang, "thang", 128,NULL, 1,&Task_Handle_1);
//      xTaskCreate (dokhoangcach, "do_kc", 128,NULL, 3,&Task_Handle_2);
//      xTaskCreate (tranhvatcan, "vat can", 128,NULL, 2,&Task_Handle_3);
    xTaskCreate (dogoc, "goc", 128,NULL, 2,&Task_Handle_4);
//      xTaskCreate (nhanbietzone, "bao zone", 128,NULL,1 ,&Task_Handle_5);
// vTaskStartScheduler();
      Serial.begin(9600);
    
    /*Dùng 3 communication object --> khá tốn RAM, tốn bao nhiêu?*/
    queue_vat_can=xQueueCreate (1, sizeof(int) );
    queue_goc=xQueueCreate (1, sizeof(int) );
    queue_n=xQueueCreate (1, sizeof(int) );  


    pinMode(enA,OUTPUT);
    pinMode(in1,OUTPUT);
    pinMode(in2,OUTPUT);
    pinMode(enB,OUTPUT);
    pinMode(in3,OUTPUT);
    pinMode(in4,OUTPUT);
    pinMode(hongngoai,INPUT);
    pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
    pinMode(echo,INPUT); 
    
    /*Đo cảm biến góc xoay*/
    Wire.begin();
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
//Serial.println("Free Mem: ");
//    Serial.println(getMemoryFree());
   
   vTaskStartScheduler();
}
      

void loop() {
  // put your main code here, to run repeatedly:

}


void dithang (void*para)
    {       
    while(1) {  
//        Serial.print("stack");
//     Serial.println(uxTaskGetStackHighWaterMark(Task_Handle_1));      
        if (i==0){   
            speed_A=80;
            speed_B=80;
            //motor A
            digitalWrite(in1,LOW);
            analogWrite(in2,speed_A); //trai
            digitalWrite(enA,HIGH);
            //motor B
            digitalWrite(in3,LOW);
            analogWrite(in4,speed_B); // phai
            digitalWrite(enB,HIGH);  
            i=1;  
        }
    /*Đoạn này chưa lấy z ở đâu ra để tính ông ơi*/

    /*Vừa thêm lệnh xQueueReceive để nhận giá trị, đúng ra ko nên lưu vào biến z 
    do nó trùng với địa chỉ góc z được lấy ở 1 task đọc giá trị z
    */
    xQueueReceive(queue_goc , &z,1); 
    Serial.println("z ");Serial.println(z);
//    z=z-n;
    z2=z*14.6*3.14/180/630*255;
    float P=(z2-z0);
    float I = 0.5*1*(z2 + z);
    float D = (z2-2*z0+z1);  
    int PID_value = PID_value0+(7.1*P) + (2.4*I) + (0.04*D);
    z1=z0;
    z0=z2;
    PID_value0=PID_value;
    speed_B = speed_B + PID_value/17;
    speed_A = speed_A - PID_value/17;
                  
//               motor A
                digitalWrite(in1,LOW);
                analogWrite(in2,speed_A); //trai // da test thanh cong
                digitalWrite(enA,HIGH);
                //motor B
                digitalWrite(in3,LOW);
                analogWrite(in4,speed_B); // phai
                digitalWrite(enB,HIGH); 
//                            
    }
}


/*Không có rằng buộc thời gian Task này lấy giá trị trong bao lâu
--> Task này gửi giá trị góc z liên tục lên hàng chờ tên queue_goc
*/
void dogoc(void*para){  
  
    while(1){
     mpu6050.update();
    z = mpu6050.getAngleZ()+mpu6050.getGyroZ();
    xQueueSend(queue_goc , &z,1); 
//    Serial.println("z ");Serial.println(z);
//     Serial.println("Free Mem: ");
//    Serial.println(getMemoryFree());
//    Serial.print("stack 2:");
//     Serial.println(uxTaskGetStackHighWaterMark(Task_Handle_4));             
    }
    }
//    void nhanbietzone (void*para)
//{
//  while(1){

  /*a=digitalRead(hongngoai);  
  if (a==HIGH)
  {
  g=1;}
  if ((a==LOW) & (g==1))           // da test ok
  {zone++;
  g=2;
    }
//   Serial.print("zone:");Serial.println(zone);
//void dokhoangcach(void*para); 
//  }}
//void dokhoangcach(void*para)
//{
//  for(;;)
  Serial.print("stack 3:");
     Serial.println(uxTaskGetStackHighWaterMark(Task_Handle_4));  

    digitalWrite(trig,0);   // tắt chân trig
    delayMicroseconds(2);
    digitalWrite(trig,1);   // phát xung từ chân trig
    delayMicroseconds(5);   // xung có độ dài 5 microSeconds
    digitalWrite(trig,0);   // tắt chân trig
    
//     Tính toán thời gian 
    // Đo độ rộng xung HIGH ở chân echo. 
    t = pulseIn(echo,HIGH);  
    // Tính khoảng cách đến vật.
    kc = int(t/2/29.412); // da test thanh cong
    Serial.println("kc");Serial.println(kc);
    if (kc<=20)
  {
  co_vat_can=1;
  }
  xQueueSend(queue_vat_can ,&co_vat_can,1);
//  Serial.print("z=");Serial.println(z);
//   Serial.print("n=");Serial.println(n); 
//  }
//}
// void tranhvatcan(void*para) 
// {
// for(;;){
////  Serial.println("co vat can ");Serial.println(can);
  if(co_vat_can==1)
  {       
    vTaskSuspend(Task_Handle_1);
//        Serial.println("xe dung ");
         if (z<90)
         {
         
         //motor A
//         digitalWrite(in1,LOW);
//         analogWrite(in2,140);
//         digitalWrite(enA,HIGH);//RE phai
//         //motor B
//         digitalWrite(enB,LOW);
         
         digitalWrite(in3,LOW);
         analogWrite(in4,100);
         digitalWrite(enB,HIGH);//RE TRAI
         //motor A
         digitalWrite(enA,LOW);
         }
          if(z>=90)
         {
          digitalWrite(enA,LOW);
          digitalWrite(enB,LOW);
           n=90;
         xQueueSend(queue_n , &n,1);
         vTaskResume(Task_Handle_1);
//         vTaskSuspend(Task_Handle_3); 
//         vTaskDelete(Task_Handle_2);
          }      
//  xQueueSend(queue_vat_can , &co_vat_can,1);
  
  }
 }
 } */
//  if (co_vat_can==1)
//        {
//         vTaskSuspend(Task_Handle_1);
//         //motor A
////         digitalWrite(in1,LOW);
////         analogWrite(in2,140);
////         digitalWrite(enA,HIGH);//RE phai
////         //motor B
////         digitalWrite(enB,LOW);
//         
//         digitalWrite(in3,LOW);
//         analogWrite(in4,140);
//         digitalWrite(enB,HIGH);//RE TRAI
//         //motor A
//         digitalWrite(enA,LOW);
//         if(z==90)
//         {
//         vTaskResume(Task_Handle_1);
//         co_vat_can=0;
//         n=90;
//         xQueueSend(queue_n , &n,1);
//         vTaskSuspend(Task_Handle_3); 
//        }      
//   
//}
//}
////}
// 
////}
////}
////void tranhvatcan (void*para)
////{ 
////  while(1)
////  {
//////    if(xQueueReceive(queue_vat_can  ,&co_vat_can,2) != NULL)
//////     if(xQueueReceive(queue_goc  ,&z,2) != NULL)
////    if (co_vat_can==1)
////        {
////         vTaskSuspend(Task_Handle_1);
////         //motor A
//////         digitalWrite(in1,LOW);
//////         analogWrite(in2,140);
//////         digitalWrite(enA,HIGH);//RE phai
//////         //motor B
//////         digitalWrite(enB,LOW);
////         
////         digitalWrite(in3,LOW);
////         analogWrite(in4,140);
////         digitalWrite(enB,HIGH);//RE TRAI
////         //motor A
////         digitalWrite(enA,LOW);
////         if(z==90)
////         {
////         vTaskResume(Task_Handle_1);
////         co_vat_can=0;
////         n=90;
////         xQueueSend(queue_n , &n,1);
////         vTaskSuspend(Task_Handle_3); 
////        }      
////   
////}
////}
////}