# cudaEnergyProfile
## Description
nvidia gpu의 Energy Consumption을 계산하는 API이다.
## Usage
nvidia-ml library를 추가 하고 사용한다.
측정을 원하는 code line의 시작과 끝에 startPowerQuery("msg");와
stopPowerQuery();를 call한다.
결과는 power.txt에서 확인 가능하다.
