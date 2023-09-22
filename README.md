# McCmd
학교 C 언어 프로젝트 제출용으로 제작된 인터프리터 언어입니다. 발표에 쓸 수 있을 정도로만 개발하였습니다.

## 기본 문법
'`/`' 함수 호출<br>
'`@<변수명>*`' 변수 호출<br>
'`@<띄어쓰기>`' 변수 선언<br> 
'`@<띄어쓰기>`/' 함수 선언<br>
'`##`' 주석<br>
'`#`' 코드 블럭<br>

<span style="color:orange">변수 스코프 구현을 못했기 떄문에 모든 변수는 전역변수 취급을 받습니다.</span>
<br><br><span style="color:yellow">변수 선언시 반드시 초기화가 필요합니다.</span>
<br><br><span style="color:orange">모든 요소들은 띄어쓰기로 구분합니다.</span>
<br><br><span style="color:yellow">선언문 호출문 관계 없이 반드시 세미콜론(;)이 필요합니다</span>

## 변수 / 함수 선언
선언문은 @ + `띄어쓰기`로 시작합니다. 
### 변수 선언
```
@ <변수 이름>[type=<자료형>,value=<값>];
```
### 함수 선언 - (미구현)
```
@ /<함수 이름> <매개변수명>@<매개변수형식>... #
    ## 구현
#;
```
매개변수와 구현(코드블럭)의 차이가 없고 가장 마지막 매개변수로 코드 블럭을 인식합니다.
## 기본 함수
<b> `/say <string>/<variable>;` - 문자열 혹은 변수를 출력합니다.
<br>`/replaceitem <variable> <memorySize|int|optional>;` - 입력 받은 정보를 변수에 저장합니다.</b>
## 변수 / 함수 호출
### 변수 호출
```
@ age[type=int,value=5];
@ name[type=string,value="steve"];
/say "내 나이는 @age*입니다.";
/say @name*;

## 출력:
## 내 나이는 5입니다.
## steve
```
### 함수 호출
```
@ /kill name@string #
    /say "@name* is dead";
#;

/kill "beomgeun";

## 출력:
## beomgeun is dead
```

### 언어 사용하기
솔루션 빌드 후 콘솔에서 exe파일을 실행합니다. 이때 mcCmd의 파일 경로를 담아야합니다.
### ex)
```sh
./Release/McCmd.exe ./test/ex1.mcCmd
```