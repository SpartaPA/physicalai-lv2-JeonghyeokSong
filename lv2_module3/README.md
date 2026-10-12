# lv2_module3

보고서: [report.md](report.md)

## 코드 출처

- 직접 작성: `ros2_ws/src/vision/src/vision_node.cpp`
- RealSense 드라이버: ROS 2 Lyrical 패키지 `realsense2_camera`

## 실행 환경

- Ubuntu 24.04.5, ROS 2 Lyrical, OpenCV 4.6.0
- 입력: Intel RealSense D435, color 640×480@30 (실시간)
- 환경변수 (모든 터미널): `ROS_DOMAIN_ID=8`, `ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST`, `FASTDDS_DEFAULT_PROFILES_FILE=$HOME/ros_config/fastdds_shm_big.xml` (공유 메모리 32MB·UDP 버퍼 4MB 프로필, 640×480 이미지가 끊기지 않게 하는 설정)

## 빌드와 실행

```bash
cd ros2_ws && colcon build --packages-select vision && source install/setup.bash
ros2 launch realsense2_camera rs_launch.py rgb_camera.color_profile:=640,480,30 enable_depth:=false align_depth.enable:=false
ros2 run vision vision_node
rqt --clear-config
ros2 topic echo /target --qos-reliability best_effort
```

## 설정값 변경과 적용

기준값: `lower = [98, 190, 25]`, `upper = [118, 255, 230]`, `min_area = 1000`, `blur_kernel_size = 3`. 파라미터를 프레임마다 읽으므로 노드 재시작 없이 바꾼다.

```bash
ros2 param set /vision_node lower "[98, 10, 25]"   # 실험 1 B
ros2 param set /vision_node min_area 20000         # 실험 2 B
ros2 param get /vision_node lower                  # 적용 확인
```

## 결과 파일

`results/` (보고서 링크 파일)

- `01_topics.txt`: 토픽 목록, 발행·구독 정보, 파라미터
- `02_*`, `04_target.txt`: Raw Image 정보·화면, `/target` 수신 메시지
- `exp1_*`: 실험 1 (S 하한 190 대 10)
- `exp2_*`: 실험 2 (`min_area` 1000 대 20000)
- `exp0_*`: 목표 없는 입력
