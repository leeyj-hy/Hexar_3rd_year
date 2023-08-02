# Hexar_3rd_year

## 0. 개요

상위 PC 로봇제어용 ROS PACKAGE
ROS node 상에서 Bicycle Kinematic Model을 활용해 제어 가능
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
- L_Vel
- L_Vel
- A_Vel
- A_Vel
- CRC
- CRC

#### 2.2.2. Example



