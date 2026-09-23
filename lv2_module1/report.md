## 문제 1. 목표 입력과 응답 확인

### 1. 원격 수행 환경 구성

os 환경
```bash
PRETTY_NAME="Ubuntu 22.04.5 LTS"
NAME="Ubuntu"
VERSION_ID="22.04"
VERSION="22.04.5 LTS (Jammy Jellyfish)"
VERSION_CODENAME=jammy
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=jammy
```

머신 환경
```bash
Linux pa09 5.15.0-1106-raspi #109-Ubuntu SMP PREEMPT Wed Jul 1 19:54:39 UTC 2026 aarch64 aarch64 aarch64 GNU/Linux
```

라즈베리 모델
```
Raspberry Pi 4 Model B Rev 1.5
```

포트 권한,그룹 확인
```bash
pa09@pa09:~/pa-opencr-build/uploader-src$ ls -l /dev/ttyACM*
crw-rw---- 1 root dialout 166, 0 Sep 23 10:30 /dev/ttyACM0
pa09@pa09:~/pa-opencr-build/uploader-src$ groups
pa09 adm dialout cdrom floppy sudo audio dip video plugdev netdev lxd
```

### 2. OpenCR 펌웨어 업로드

```bash
make: Leaving directory '/home/pa09/pa-opencr-build/uploader-src/arduino/opencr_develop/opencr_ld'
arduino/opencr_develop/opencr_ld/opencr_ld: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux-aarch64.so.1, BuildID[sha1]=f65e9a7cc0cfd3e2f0705c840ded1561027c291b, for GNU/Linux 3.7.0, not stripped
```

### 3. 목표 입력과 응답 기록

```
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:4.500
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:4.600
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:4.700
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:4.800
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:4.900
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:5.000
xSTOP: user

--- exit ---
```

### 4. 결과 설명

|  | |
| --- | --- |
| 장치 | 휠 모터 |
| protocol | 2 | 
| id | 1 |
| baud | 1000000 |
| model | 1030 |

kp : 5 
speed : 40
deg : 30

error_deg 가 -0.146 까지 근접 했으며 실제로 눈에 띄는 오버 슈트나 발산,진동은 발생 하지 않았기 때문에 안정적으로 도달 했다고 볼 수 있다.

목표값 = `target_deg:30.000`
현재 위치 = `position_deg:30.674`
게인 출력 = `p_deg_s:-3.369`

## 문제 2. 오차와 피드백 해석

### 1. 오차 계산

초반
```
target_deg:30.000       position_deg:0.264      error_deg:29.736        p_deg_s:148.682 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:148.682       speed_deg_s:2.748       u_deg_s:28.854  v_limit_deg_s:30.000    dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.100
```
30.000 - 0.264 = 29.736


중간
```
target_deg:30.000       position_deg:14.590     error_deg:15.410        p_deg_s:77.051  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:77.051        speed_deg_s:39.846      u_deg_s:39.846  v_limit_deg_s:40.000    dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.600
```
30.000 - 14.590 = 15.410


마지막
```
target_deg:30.000       position_deg:30.146     error_deg:-0.146        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:40.000     dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:5.000
```
30.000 - 30.146 = -0.146

### 2. 오차 크기 변화 분석

양수의 오차값을 가진상태로 줄어들다 음수로 전환 된다. 속도에 의한 관성으로 목표를 지나친다.

### 3. 위치 측정값을 얻는 센서와 OpenCR 까지의 통신 경로

모터의 내부에서 MCU 가 엔코더의 값을 읽는다. -> 값이 opencr 로 이동 하고 거기서 모터 로 명령을 내린다. -> 모터안에 MCU 가 명령을 받고 움직인다.

### 4. 

목표 30, 현재각 35 일때 + 방향으로 5 만큼 더 갔기때문에 -방향으로 5 보정 해야 한다.
30 - 35 = -5


## 문제 3. P 게인 변경에 따른 응답 비교

### 1. A·B 설정 및 비교표

**공통 조건**

| 항목 | 값 |
| --- | --- |
| 목표각 | 90 |
| 속도 상한 | max |
| 측정 주기 | 10 |
| 시작 자세 | 0|


**A·B 게인 설정**

| 설정 | Kp | 
| --- | --- |
| 실행 A | 5.0 |
| 실행 B | 10 |

**같은 경과 시간에서 비교표**

| t_s | A: position_deg | A: 목표초과 | B: position_deg | B: 목표초과 |
| --- | --- | --- | --- | --- |
| 2.1 | 28.564| 61.436| 30.938|59.063 |
| 2.6 | 87.715 |2.285 |90.176 | -0.176|
| 3.0 | 89.736|0.264 | 90.176 | -0.176|
| 4.0 | 89.824| 0.176| 90.176| -0.176|


### 2. 두 실행 기록

**실행 A** Kp=5
```
s 5 max 90
START: current position = 0 deg.
P SET Kp=5.0000, Ki=0.0000, Kd=0.0000, speed_limit_deg_s=max, angle_deg=90.000
Run timeout [s]: 60.0
...
target_deg:0.000        position_deg:0.000      error_deg:0.000 p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000   v_limit_deg_s:-1.000    dt_ms:10.002    kp:5.0000       ki:0.0000       kd:0.0000       t_s:1.900
target_deg:90.000       position_deg:0.000      error_deg:90.000        p_deg_s:450.000 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:450.000       speed_deg_s:0.000       u_deg_s:450.672 v_limit_deg_s:-1.000    dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.000
target_deg:90.000       position_deg:28.564     error_deg:61.436        p_deg_s:307.178 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:307.178       speed_deg_s:346.248     u_deg_s:307.776 v_limit_deg_s:-1.000    dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.100
target_deg:90.000       position_deg:56.953     error_deg:33.047        p_deg_s:165.234 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:165.234       speed_deg_s:256.938     u_deg_s:164.880 v_limit_deg_s:-1.000    dt_ms:10.002    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.200
target_deg:90.000       position_deg:73.125     error_deg:16.875        p_deg_s:84.375  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:84.375        speed_deg_s:140.148     u_deg_s:83.814  v_limit_deg_s:-1.000    dt_ms:10.000    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.300
target_deg:90.000       position_deg:81.475     error_deg:8.525 p_deg_s:42.627  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:42.627        speed_deg_s:71.448      u_deg_s:42.594 v_limit_deg_s:-1.000     dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.400
target_deg:90.000       position_deg:85.605     error_deg:4.395 p_deg_s:21.973  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:21.973        speed_deg_s:35.724      u_deg_s:21.984 v_limit_deg_s:-1.000     dt_ms:10.002    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.500
target_deg:90.000       position_deg:87.715     error_deg:2.285 p_deg_s:11.426  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:11.426        speed_deg_s:17.862      u_deg_s:10.992 v_limit_deg_s:-1.000     dt_ms:10.003    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.600
target_deg:90.000       position_deg:88.857     error_deg:1.143 p_deg_s:5.713   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:5.713 speed_deg_s:9.618       u_deg_s:5.496   v_limit_deg_s:-1.000    dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.700
target_deg:90.000       position_deg:89.385     error_deg:0.615 p_deg_s:3.076   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:3.076 speed_deg_s:4.122       u_deg_s:2.748   v_limit_deg_s:-1.000    dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.801
target_deg:90.000       position_deg:89.648     error_deg:0.352 p_deg_s:1.758   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:1.758 speed_deg_s:4.122       u_deg_s:1.374   v_limit_deg_s:-1.000    dt_ms:10.002    kp:5.0000       ki:0.0000       kd:0.0000       t_s:2.901
target_deg:90.000       position_deg:89.736     error_deg:0.264 p_deg_s:1.318   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:1.318 speed_deg_s:1.374       u_deg_s:1.374   v_limit_deg_s:-1.000    dt_ms:10.001    kp:5.0000       ki:0.0000       kd:0.0000       t_s:3.001
target_deg:90.000       position_deg:89.824     error_deg:0.176 p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000   v_limit_deg_s:-1.000    dt_ms:10.002    kp:5.0000       ki:0.0000       kd:0.0000       t_s:3.101
```

**실행 B** Kp=10
```
s 10 max 90
START: current position = 0 deg.
P SET Kp=10.0000, Ki=0.0000, Kd=0.0000, speed_limit_deg_s=max, angle_deg=90.000
Run timeout [s]: 60.0
...
target_deg:90.000       position_deg:-0.088     error_deg:90.088        p_deg_s:900.879 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:900.879       speed_deg_s:0.000       u_deg_s:453.420 v_limit_deg_s:-1.000    dt_ms:10.003    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.000
target_deg:90.000       position_deg:30.938     error_deg:59.063        p_deg_s:590.625 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:590.625       speed_deg_s:384.720     u_deg_s:453.420 v_limit_deg_s:-1.000    dt_ms:10.001    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.100
target_deg:90.000       position_deg:71.982     error_deg:18.018        p_deg_s:180.176 i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:180.176       speed_deg_s:394.338     u_deg_s:179.994 v_limit_deg_s:-1.000    dt_ms:10.000    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.200
target_deg:90.000       position_deg:88.594     error_deg:1.406 p_deg_s:14.062  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:14.062        speed_deg_s:120.912     u_deg_s:13.740 v_limit_deg_s:-1.000     dt_ms:10.002    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.300
target_deg:90.000       position_deg:90.615     error_deg:-0.615        p_deg_s:-6.152  i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:-6.152        speed_deg_s:6.870       u_deg_s:-5.496  v_limit_deg_s:-1.000    dt_ms:10.000    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.400
target_deg:90.000       position_deg:90.176     error_deg:-0.176        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:-2.748      u_deg_s:0.000  v_limit_deg_s:-1.000     dt_ms:10.001    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.500
target_deg:90.000       position_deg:90.176     error_deg:-0.176        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:-1.000     dt_ms:10.001    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.600
target_deg:90.000       position_deg:90.176     error_deg:-0.176        p_deg_s:0.000   i_deg_s:0.000   d_deg_s:0.000   pid_deg_s:0.000 speed_deg_s:0.000       u_deg_s:0.000  v_limit_deg_s:-1.000     dt_ms:10.001    kp:10.0000      ki:0.0000       kd:0.0000       t_s:2.700
xSTOP: user
```

### 3. 해석

- 같은 경과 시간 비교: 조건B 가 좀더 빠른 속도로 목표치에 도달 한다.
- 목표 초과(오버슈트) 차이: B 는 순간 오버 슈트가 -0.176 에 달하는 반면 A 는 오버슈트 없이 서서히 더 가까워진다.
- 차이의 근거: P는 현재 오차에 대한 가중치를 뜻 하기 때문에 높을 수록 강하게 목표에 가까워 지기 때문에 목표를 지나칠 수 있다.

### 4. 게인 위치 및 I·D 설명

- 변경한 게인 위치: OpenCR 위치 제어 게인 - 라즈베리에서 빌드된 아두이노 파일으로 모터를 제어 했기 때문에
- I : 누적 오차를 이용 해서 보정
- D : dt 기준 이전의 오차와 현재 오차를 사용해 다음 위치를 계산하여 보정


## 문제 4. 제어와 통신의 역할 해석

### 1. 구조도 (목표 전달·측정값 반환 방향)

```
[PC ROS2 노드] -/motor/target(목표 30.0°)-> [micro-ROS Agent] > [OpenCR] > [다이나믹셀]
[PC ROS2 노드] <-/motor/state(측정값)- [micro-ROS Agent] < [OpenCR] < [다이나믹셀]
```


### 2. 송수신 역할

| 신호 | 보내는 주체 | 받는 주체 | 토픽 |
| --- | --- | --- | --- |
| 목표각 | PC ROS2 노드 | OpenCR | /motor/target |
| 측정값 | OpenCR | PC ROS2 노드 | /motor/state |

### 3. 주기 비교 (상태 발행 한 주기당 제어 계산 횟수)

100 / 10 = **10회**

### 4. 통신 단절 시 오래된 명령 처리 정책

- 정책: 버린다. Durability:Volatile
- 이유: 당장의 상태가 필요하지 이전은 필요 없다.