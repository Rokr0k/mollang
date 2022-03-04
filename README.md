# 몰?랭

![몰?루](https://ac2.namu.la/20210218/fee9a87d7583362b801ca2415dcfbff6b7d54e6e896cd45d2aced00610bd4fce.png)

## 사용 가능한 리터럴

[상수](#상수), [변수](#변수), [주소수](#주소수)가 있습니다.

### 상수

값 자체를 나타냅니다.

|몰|아|?|루|
|-|-|-|-|
|00|01|10|11|

> 예시: `몰?아루` == `00100111(2)` == `39(10)`

### 변수

값을 저장할 수 있는 공간을 나타냅니다.

"몰"과 "루" 사이에 0~9까지의 숫자를 넣어 나타냅니다.
즉, 총 10개의 변수를 사용할 수 있습니다.

초기값은 랜덤으로 정해져서 몰?룹니다.

> 예시: `몰0루`, `몰1루`, `몰2루`, ...

### 주소수

주소를 나타낼 수 있습니다.

값이 들어있는 주소일 수도 있고, 실행할 수 있는 코드가 있는 위치일 수도 있습니다.

코드 중간에 주소를 나타내는 표시를 넣어 주소수의 값을 정할 수 있습니다.

안 정하고 사용하면 랜덤으로 정해져서 몰?룹니다.

주소를 나타낼 때에는 "ﾌﾄｽﾄ" 뒤에 느낌표 개수로 구별합니다.  
반각 카타카나만 인정합니다. 한글로 적으면 인정하지 않습니다.  
느낌표가 없을 수도 있습니다.

> 예시: `ﾌﾄｽﾄ`, `ﾌﾄｽﾄ!`, `ﾌﾄｽﾄ!!`, ...

#### 주소수의 변수화

"털"과 "자" 사이에 느낌표 개수에 따라 주소수가 가르키는 값을 변수처럼 접근할 수 있습니다.  
느낌표가 없을 수도 있습니다.

> 예시: `털자`, `털!자`, `털!!자`, ...

#### 변수의 주소수화

변수를 주소수로 변환할 수 있습니다.
변수에 주소수를 대입하고 사용하면 됩니다.

##### 주소수화된 변수의 변수화

주소수로 변환된 변수가 가르키는 값을 변수처럼 접근할 때에는 "털"과 "자" 사이에 숫자로 구별합니다.

> 예시: `털0자`, `털1자`, `털2자`, ...

## 명령문

```mollang
<명령어> <파라미터>...
```

## 명령어

주의: "~에 저장합니다"라는 텍스트가 포함된 경우, 상수 또는 주소수가 오면 실행할 수 없습니다.

|사용|설명|
|-|-|
|몰몰몰 A B|A의 값과 B의 값을 NAND 연산한 값을 A에 저장합니다.|
|몰몰아 A B|A의 값을 B의 값만큼 왼쪽으로 시프트한 값을 A에 저장합니다.|
|몰몰? A B|A의 값을 B의 값만큼 오른쪽으로 시프트한 값을 A에 저장합니다.|
|몰몰루 A B|B의 값을 A에 저장합니다.|
|몰아몰 A B|A의 값과 B의 값을 이용하여 상태를 저장합니다.|
|몰아아 C|`몰아몰`의 결과에 따라 A&gt;B인 경우 C의 위치로 이동하여 실행합니다.|
|몰아? A|A의 위치로 이동하여 실행합니다.|
|몰아루 A|현재 위치를 스택에 저장하고 A의 위치로 이동하여 실행합니다.|
|몰?몰|스택에 저장된 위치로 이동하여 실행합니다.|
|몰?아 A|A의 값에 해당하는 [개입](#개입) 를! 실행합니다.|
|몰?? A|A의 값을 스택에 저장합니다.|
|몰?루|아무것도 실행하지 않고, 공간을 1 차지합니다.|
|몰루몰 A|스택에 저장된 값을 A에 저장합니다.|
|아?루|![아?루](https://ac.namu.la/20211108s1/081c232f048c3d9a458f56584df508baa4291e8975c157016255d81ffa129449.png) 치명적인 에러를 강제로 발생시킵니다.|

## 개입

|코드|설명|
|:-:|-|
|0|변수 `몰0루`의 값을 문자로 출력합니다.|
|1|문자로 입력받은 값을 변수 `몰0루`에 저장합니다.|
|2|변수 `몰0루`의 값만큼 프로세스를 정지합니다. (ms 단위)|
|3|프로그램을 종료합니다.|
