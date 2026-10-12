# lv2_module3: 설정값에 따른 Mask와 검출 결과 변화 분석

보고서는 [report.md](report.md)에 있다.

## 코드 출처

`ros2_ws/src/vision/src/vision_node.cpp`는 이 과제에서 직접 작성한 C++ 노드다. 제공된 수업 코드는 쓰지 않았다. RealSense 드라이버(`realsense2_camera`)는 ROS 2 Lyrical 패키지를 그대로 사용했다.

노드 동작: `/camera/camera/color/image_raw`(rgb8)를 구독해 BGR로 변환한 뒤 `GaussianBlur` → HSV 변환 → `inRange` → `morphologyEx`(OPEN 1회, CLOSE 2회)로 Mask를 만들고 `/vision/mask`로 발행한다. 그 Mask에서 `findContours`로 컨투어를 찾고 면적이 `min_area` 이상인 것 중 가장 큰 하나의 무게중심과 면적을 `/target`(`geometry_msgs/msg/PointStamped`)으로 발행한다. `/target`의 `x`, `y`는 화면 중앙 기준 정규화 오차(-1~1), `z`는 이미지 전체 면적 대비 면적비다. 후보가 없는 프레임에서는 발행하지 않는다.

## 실행 환경

- Ubuntu 24.04.5, ROS 2 Lyrical (rclcpp 32.0.2, cv_bridge 4.1.0), OpenCV 4.6.0, g++ 13.3.0
- 입력: Intel RealSense D435, color 640×480@30 (실시간 Camera 입력)
- 모든 터미널에서 같은 환경변수를 사용한다.

```bash
export ROS_DOMAIN_ID=8
export ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST
export FASTDDS_DEFAULT_PROFILES_FILE=$HOME/ros_config/fastdds_shm_big.xml
```

`fastdds_shm_big.xml`은 Fast DDS의 공유 메모리 세그먼트(기본 약 0.5MB)를 32MB로, UDP 버퍼를 4MB로 키운 설정이다. 640×480 `rgb8` 이미지 한 장(약 0.9MB)이 기본 세그먼트보다 커서 이미지가 끊기는 문제를 막는다. 발행자(RealSense)와 구독자(`vision_node`, rqt) 모두에 적용해야 한다.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<dds xmlns="http://www.eprosima.com">
  <profiles>
    <transport_descriptors>
      <transport_descriptor>
        <transport_id>shm_big</transport_id>
        <type>SHM</type>
        <segment_size>33554432</segment_size>
      </transport_descriptor>
      <transport_descriptor>
        <transport_id>udp_big</transport_id>
        <type>UDPv4</type>
        <sendBufferSize>4194304</sendBufferSize>
        <receiveBufferSize>4194304</receiveBufferSize>
      </transport_descriptor>
    </transport_descriptors>
    <participant profile_name="shm_big_profile" is_default_profile="true">
      <rtps>
        <useBuiltinTransports>false</useBuiltinTransports>
        <userTransports>
          <transport_id>shm_big</transport_id>
          <transport_id>udp_big</transport_id>
        </userTransports>
      </rtps>
    </participant>
  </profiles>
</dds>
```

## 빌드와 실행

```bash
cd ros2_ws
colcon build --packages-select vision
source install/setup.bash
```

터미널을 3개 연다.

```bash
# 1. 카메라
ros2 launch realsense2_camera rs_launch.py rgb_camera.color_profile:=640,480,30 enable_depth:=false align_depth.enable:=false
```

```bash
# 2. 비전 노드
ros2 run vision vision_node
```

```bash
# 3. 수신 확인: Raw Image(/camera/camera/color/image_raw), Mask(/vision/mask)는 rqt의 Image View로, 검출 결과는 CLI로
rqt --clear-config
ros2 topic echo /target --qos-reliability best_effort
```

모터 제어 노드는 실행하지 않는다.

## 설정값 변경과 적용 방법

노드가 파라미터(`lower`, `upper`, `min_area`, `blur_kernel_size`)를 프레임마다 읽으므로 노드를 재시작하지 않고 실행 중에 바꿀 수 있다.

기준값은 코드 기본값이다: `lower = [98, 190, 25]`, `upper = [118, 255, 230]`, `min_area = 1000`, `blur_kernel_size = 3`.

```bash
# 실험 1: S 하한 190 -> 10 (배열은 3개를 통째로 지정)
ros2 param set /vision_node lower "[98, 10, 25]"

# 실험 2: min_area 1000 -> 20000
ros2 param set /vision_node min_area 20000

# 적용 확인
ros2 param get /vision_node lower
ros2 param get /vision_node min_area

# 기준값으로 복원
ros2 param set /vision_node lower "[98, 190, 25]"
ros2 param set /vision_node min_area 1000
```

## 결과 파일 위치

`results/` 폴더에 보고서가 링크하는 증거가 있다.

| 파일 | 내용 |
|---|---|
| `01_topics.txt` | 토픽 목록, 세 토픽의 발행·구독 정보와 QoS, 실행 중인 파라미터 전체 |
| `02_raw_info.txt`, `02_raw_mask_baseline.png` | Raw Image의 해상도·인코딩, 기준 설정의 Raw·Mask 수신 화면 |
| `04_target.txt` | `/target` 실제 수신 메시지 |
| `exp1_A.png`, `exp1_B.png`, `exp1_A_param.txt`, `exp1_B_param.txt`, `exp1_A_target.txt`, `exp1_B_target.txt` | 실험 1 (S 하한 190 대 10): 수신 화면, 적용값, 약 5초 `/target` 샘플 |
| `exp2_A.png`, `exp2_B.png`, `exp2_A_param.txt`, `exp2_B_param.txt`, `exp2_A_target.txt`, `exp2_B_target.txt` | 실험 2 (`min_area` 1000 대 20000): 수신 화면, 적용값, 6초 `/target` 샘플 |
| `exp0_notarget.png`, `exp0_node_log.png`, `exp0_param.txt`, `exp0_target.txt` | 목표 없는 입력: Raw·Mask 화면, 노드 로그 화면, 적용값, `/target` 6초 수신 결과 |
