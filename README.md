# Hexar_3rd_year

## 0. 개요

상위 PC 로봇제어용 ROS PACKAGE
ROS node 상에서 Bicycle Kinematic Model을 활용해 제어 가능 [Bicycle Kinematic Model Notion Link](https://www.notion.so/HEXAR-Controller-Design-ee6a99a0626041b8bf96f3e370c73c39?pvs=4)

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/1e2a70cc-35cc-479e-9970-a44291293941)

아래 사진의 붉은 색 점선 박스의 부분이 현재 repository에 해당.

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/aad11d8f-0c50-414f-93b2-a0eeb424f96a)

## 1. 사용

roscore 실행 후, 아래의 명령어 입력
(이 때 chmod를 통한 USB 권한 부여 확인필요)
    
    rosrun hexar_3rd hexar_3rd_node

## 2. Specification

### 2.1. Hardware

--USB to UART 모듈을 통해 PC와 Robot 연결
ex)

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/f3450f29-5f2b-4970-acb1-995dc2de8cc2)

--혹은 SBC의 UART 포트를 통해 연결
ex)

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/0cf28203-413a-43f9-a959-fed1b4bbbcc3)

### 2.2. Software

#### 2.2.1. UART Communication Protocol

현재 버전의 경우, 3.3V 와 5V 레벨의 UART를 모두 지원 가능

통신 프레임의 경우 다음과 같이 구성

- HEAD
- HEAD
- L_Value (h)
- L_Value (l)
- A_Value (h)
- A_Value (l)
- CRC (h)
- CRC (l)

##### 단위
L_Value 의 경우, RPM의 단위를 사용 (1 LSB = 1 RPM)
A_Value 의 경우, DEG의 단위를 사용 (1 LSB = 1 DEG)

#### 2.2.2. UART Example

선속도 50RPM, 각도 30도를 전송

- `0xFF` - Start of UART custom Frame 1
- `0xFF` - Start of UART custom Frame 2
- `0x32` - Linear Value 1
- `0x00` - Linear Value 2 (0x0032 = 50 (DEC))
- `0x1E` - Angular Value 1
- `0x00` - Angular Value 2 (0x001E = 30 (DEC))
- `0x07` - CRC-16 Modbus 1
- `0x3C` - CRC-16 Modbus 2 (Only including Data section of Frame)

#### 2.2.3. ROS Node

    class Converter
    {
        private:
            NodeHandle n;
            Subscriber MsgSub;
            Publisher MsgPub;
    
            cmd cmd_obj;
            cmd cmd_kin;
            geometry_msgs::Twist tmp_pub;

          public:
            Converter()
              {
                MsgSub = n.subscribe("/cmd_vel", 1, &Converter::CmdReqCallback, this);
                MsgPub = n.advertise<geometry_msgs::Twist>("/CurrentValRet", 10);
              }
            geometry_msgs::Twist cmd_vel;
            
            void CmdReqCallback(const geometry_msgs::Twist &vel_msg);
            void CurrentValPub();
            
            void KinematicModel(cmd cmd_input);
    };

ROS Node의 경우 크게 두 부분으로 구성된다.

    MsgSub = n.subscribe("/cmd_vel", 1, &Converter::CmdReqCallback, this);

와

    MsgPub = n.advertise<geometry_msgs::Twist>("/CurrentValRet", 10);

로 구성되어 있다.

subscriber의 경우, 상위 노드에서 Publish 하는 `/cmd_vel` 을 수신하고 Callback 함수를 호출하여 `Geometry_msgs/Twist`로 작성된 로봇 이동 명령을 각각 클래스 구조체의 `cmd.ang_vel` 과 `cmd.lin_vel`에 저장하게 된다.

또한, Sub node에서 호출되는 `Converter::KinematicModel()`은 ros node는 아니지만 Bicycle Kinematic Model 을 활용하여 수신된 선속도, 각속도를 각각 실제 인휠모터와 조향모터를 조작하는 값으로 변환시켜주는 `void KinematicModel(cmd cmd_input);`을 볼 수 있다.

다음으로 `KinematicModel(cmd)` 함수를 거친 후에 `void CurrentValPub()` 함수에서는 앞서 계산된 RPM과 조향각을 각각 UART Frame에 대입시키고 CRC를 계산하여 UART Communication Frame을 완성시킨다.


     buf[0] = 0xFF;
     buf[1] = 0xFF;
     buf[2] = cmd_obj.lin_vel;
     buf[3] = cmd_obj.lin_vel >> 8;
     buf[4] = tmp_data;
     buf[5] = tmp_data >> 8;
     
     crc = crc_modbus(&buf[2], 4);
     memcpy(&buf[6], (char*)&crc, 2);

