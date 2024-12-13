# Smart Zone Client README

## 프로젝트 개요
Smart Zone 프로젝트는 라즈베리 파이 5와 카메라 모듈을 이용하여 실시간 모니터링 및 의미 있는 데이터를 모아 그래프로 보여 주는 프로젝트 입니다. Client는 UI를 요소를 위해 Qt 프레임워크를 사용합니다. 

## 의존성

### Qt 버전
- **Qt**: 6.5.3

### Qt 패키지 목록
프로젝트에 필요한 Qt 패키지는 다음과 같습니다:
- **Core**: 이벤트 처리, 데이터 구조 등 핵심 기능을 제공합니다.
- **Widgets**: 그래픽 사용자 인터페이스(GUI)를 생성하는 데 사용됩니다.
- **Network**: TCP/IP와 같은 네트워크 통신 프로토콜을 구현하는 데 사용됩니다.
- **Charts**: 통계 데이터를 차트로 시각화하는 데 사용됩니다.
- **Multimedia**: 비디오 캡처 및 재생과 같은 멀티미디어 처리를 위한 클래스가 포함됩니다.
- **MultimediaWidgets**: 멀티미디어 관련 위젯을 제공합니다.
- **Concurrent**: 멀티스레딩 및 동시 프로그래밍을 지원합니다.

## 설치 가이드

### Step 1: Qt 설치
Qt 6.5.3을 설치하려면 [공식 Qt 설치 가이드](https://doc.qt.io/qt-6.5/qtdoc/installation.html)를 참고하세요.  
설치 시 필요한 Qt 모듈(Core, Widgets, Network, Charts, Multimedia, MultimediaWidgets, Concurrent)을 선택하여 설치해 주세요.

## 기능

### 스트리밍 
![image](https://github.com/user-attachments/assets/68a1485d-0d83-4cda-80f2-1d95a408bd5f)
최대 4대까지 동시 스트리밍 가능합니다.

### 영역 추가
![image](https://github.com/user-attachments/assets/3776334c-1142-49ea-8d93-bd19f68fffda)
마우스 드래그로 객체 인식할 영역을 추가할 수 있습니다.

### 통계 그래프 보기
![image](https://github.com/user-attachments/assets/c75d5f39-7a1e-4f53-902e-264d1c93c8a0)
영역 별 사람 수는 라인 그래프로 시간 별 변화를 알 수 있습니다.

![image](https://github.com/user-attachments/assets/d61f19bf-bdf9-415e-922a-67ac4e82b0e1)
영역 별 머문 시간은 박스 플롯으로 그래프를 그립니다. 이 그래프를 통해 영역별 머문 시간의 분포를 알 수 있습니다.

![image](https://github.com/user-attachments/assets/333905a4-8d70-4ed3-b9c3-6a9e58a008be)
영역 간 이동한 사람 수는 Sankey 다이어그램으로 그립니다. 이 다이어그램을 통해 영역간 관계를 알 수 있습니다.

