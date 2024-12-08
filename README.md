# Smart Zone Client README

## 프로젝트 개요
Smart Zone Client는 보안 및 모니터링을 위한 실시간 비디오 캡처, 객체 감지 및 추적 기능을 통합한 프로젝트입니다. 이 프로젝트는 UI 구성 요소를 위해 Qt를 사용하고, 이미지 처리 및 객체 추적을 위해 OpenCV를 활용합니다.

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

### OpenCV 버전
- **OpenCV**: 4.6.0  
  OpenCV는 비디오 처리, 객체 감지 및 추적을 위한 라이브러리입니다.

## 설치 가이드

### Step 1: Qt 설치
Qt 6.5.3을 설치하려면 [공식 Qt 설치 가이드](https://doc.qt.io/qt-6.5/qtdoc/installation.html)를 참고하세요.  
설치 시 필요한 Qt 모듈(Core, Widgets, Network, Charts, Multimedia, MultimediaWidgets, Concurrent)을 선택하여 설치해 주세요.

### Step 2: OpenCV 설치
OpenCV 4.6.0을 설치하려면, 사용하는 플랫폼에 맞게 아래의 지침을 따르세요:

#### Ubuntu/Linux에서 설치
```bash
sudo apt update
sudo apt install libopencv-dev
