# Hexar_3rd_year

상위 PC 로봇제어용 ROS PACKAGE
ROS node 상에서 Bicycle Kinematic Model을 활용해 제어 가능
![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/1e2a70cc-35cc-479e-9970-a44291293941)

--MCU의 USB 마이크로 단자를 통해 상위 제어 보드와 연결

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/d1639376-4028-4437-b5bc-45ba7c334004)

연결 후 SBC의 /dev 디렉토리에서 해당 포트 연결 확인 및 권한 부여 필요

roscore 실행 후, 아래의 명령어 입력
$ rosrun hexar_3rd hexar_3rd_node

(이 때 chmod를 통한 USB 권한 부여 확인필요)

상위 제어 노드의 /cmd_vel 값을 받아와 이를 UART로 전환 및 통신하는 노드

아래 사진의 붉은 색 점선 박스의 부분이 현재 repository에 해당.

![image](https://github.com/leeyj-hy/Hexar_3rd_year/assets/82855221/aad11d8f-0c50-414f-93b2-a0eeb424f96a)
