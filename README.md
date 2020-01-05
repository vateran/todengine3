# todengine3

# Installation (Visual Studio 2019)
 * Visual Studio 2019 설치 (Comminity 버전 기준)
 * Qt 설치 (Open Source 버전)
 * Visual Studio > 메뉴 > 확장 > 확장관리 에서 Qt Visual Studio Tools 를 검색하여 설치 
 * Visual Studio > 메뉴 > 확장 > Qt VS Tools > Qt Option 창에서 Add 를 누르고 (QTDIR)/msvc2017_64 폴더를 지정 (2019 는 현재 없음)
 * 동일한 창에서 Default Qt/Win version 을 방금 추가한 항목으로 지정
 * todengine.sln 솔루션 파일 열기
 * todeditor 를 시작 프로젝트로 설정
 * todeditor 프로젝트 설정 > 작업 디렉토리를 bin 디렉토리로 설정 (../../bin)
 * todeditor 의 Qt Project Settings > Qt Modules탭 에서 Core, GUI, Widgets 를 선택후 OK
 
# Installation (XCode)

# Core
 * Object Management
 * Reference count 방식 
 * Reflection
 * Exception
 * Singleton Management
 * Object Pooling
 * Memory Management
 * Component-Based 
 * Serialization
 
# Graphics
 * OpenGL, DirectX 다 하고 싶다 ㅠㅠ
 * SceneGraph 방식의 scene 관리

# Scripting
 * Python : 얘는 tool 만들때 쓴다
 * Lua : 얘는 게임 로직 만들때 쓴다
 * 하지만 둘다 게임 로직 만들때 써도 됨
 
# TodEditor
 * 이 툴에 모든게 다 종합됨
 * SceneView 가 여러개 편집되게
 * WYGYWIC(스펠맞나?ㅋ) 방식 편집
 
# 도전과제
 * SIMD 기반 vector, matrix 연산
 * reactive 방식의 파일 및 비동기 처리
 * MiniZip 아카이브로 packed file 관리
 * 심리스 terrain
 * 테셀레이션 + 디스플레이스먼트 맵핑
 * 패러랙스 맵핑
 * Light Shaft
 * x HDR
 * Ocean 렌더링
 * 인스턴싱
 * x SSAO
 * 오클루젼 컬링
 * Shadow Map
   * x 기초 
   * Cascade ShadowMap
   * Soft Shadow
 * x 노말맵핑
 * x FBX 로딩
 * x Deferred Shading
 * PBR
 * x Depth of Field
 * Multithread-Rendering
 
 # 컴파일
  * Visual Studio 2015
    * Windows 10 SDK 설치(for DirectX12)
    * ~/todengine3/build/msvc 에 있는 todengine.sln
  * XCode8
    * ~/todengine3/build/xcode 에 있는 todengine.xcworkspace

 # TodEditor 실행
  * Visual Studio 2015
    * todeditor 프로젝트 > 속성 > 작업 디렉터리 > $(ProjectDir)..\..\bin 로 변경
    * todeditor 를 '시작 프로젝트'로 변경
    * 실행


