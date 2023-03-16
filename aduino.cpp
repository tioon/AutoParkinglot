//명령변수=order
//동작변수=motion ( 수납1=0 , 반납0=1라고 가정
//상태변수=state[4]  <-크기 4 배열
//상태 0 = '0', 상태 1 = '1', 상태 2 = '2'라고 가정
//반납명령=1 , 수납명령='0'라고 가정
//번호변수=number
//목표변수=object
//1.Command  2.State  3.Route  4.Calculator  5.Print  6.Push  7.Floor_Move
#include<Keypad.h>
#include<LiquidCrystal.h>//압력센서

const byte ROWS = 4;//키패드 부분 변수(Command)
const byte COLS = 3;
char key = 0;
char keys[ROWS][COLS] = {
     {'1','2','3'},
     {'4','5','6'},
     {'7','8','9'},
     {'#','0','*'}
};
byte rowPins[ROWS] = { 6,5};
byte colPins[COLS] = { 4,3,2};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // 키패드 읽기

const int sensorpin1 = A0;//압력센서에 연결한 아날로그 핀(state), 핀번호에맞게 수정해야함, 전역변수
const int sensorpin2 = A1;
const int sensorpin3 = A2;
const int sensorpin4 = A3;

//A가 상하 운동,B가 좌우 운동
const int Dir1Pin_A = 8;
const int Dir2Pin_A = 9;
const int SpeedPin_A = 10;

const int Dir1pin_B = 12;
const int Dir2Pin_B = 13;
const int speedpin_B = 11;


//main
void setup()
{
    Serial.begin(9600);

    pinMode(Dir1Pin_A, OUTPUT);
    pInMode(Dir2Pin_A, OUTPUT);
    pinMode(SpeedPin_A, OUTPUT);
    pinMode(Dir1Pin_B, OUTPUT);
    pInMode(Dir2Pin_B, OUTPUT);
    pinMode(SpeedPin_B, OUTPUT);

}
void loop()
{
    int number = 0, order = 0, state[4] = { 0 }, motion=0, object=0;
    State(state);// 압력센서 정보받음
    Serial.println("키패드 입력해주세요.");
    Command(&number, &order);//키패드입력
    Serial.println("number변수");
    Serail.println(number);
    Serial.println("order변수");
    Serial.println(order);
    Route(order, number, state);//수납/반납
  //  Serial.end();//끝

}




void Command(int* number, int* order)
{
    while (1)
    {
        key =kpd.getKey();
        if (key != 0)
        {
            switch (key)
            {
            case '6':
                *order = 0;
                *number = 6;
                Serial.println(key);
                break;
            case '1':
                *order = 1;
                *number = 1;
                Serial.println(key);//출력안할거면 빼도됨
                break;
            case '2':
                *order = 1;
                *number = 2;
                Serial.println(key);
                break;
            case '3':
                *order = 1;
                *number = 3;
                Serial.println(key);
                break;
            case '4':
                *order = 1;
                *number = 4;
                Serial.println(key);
                break;
            default:
                Serial.println("잘못된 key를 입력했습니다.");
            }
        }
    }
}

void State(int *state)
{
    state[0] = analogRead(sensorpin1);
    state[1] = analogRead(sensorpin2);
    state[2] = analogRead(sensorpin3);
    state[3] = analogRead(sensorpin4);

    for (int i = 0;i < 4;i++)
    {
        if (state[i] == 0)
        {
            state[i] = 0;
        }
        else if ((state[i]>=x1)&&(state[i]<=x2) ) //X값은 압력센서에따라 설정해야함.
        {
            state[i] = 1;
        }
        else if (state[i] > X)
        {
            state[i] = 2;
        }
        Serial.print("첫번째");
        Serial.println(state[0]);
        Serial.print("두번째");
        Serial.println(state[1]);
        Serial.print("세번째");
        Serial.println(state[2]);
        Serial.print("네번째");
        Serial.println(state[3]);
        Serial.print("\n");
        delay(3000);

    }
}



void Route(int  order, int number,int* state_p)// 3
{
  
        int object = 0;
        int motion = 0;
        int floor = 0;//층변수(포인터로 바꿈)
        int c_floor = 1;//현재 층
        int direction = 0;//방향변수(포인터로 바꿈)
        if (order == 0)// @@@@@@수납@@@@@@@
        {
       
            for (int i = 0;i < 4;i++)      // 1.물건을 넣고 상태0인 곳에 수납
            {
                if (state_p[i] == 0)//상태0일때 수납
                {
                    object = i;
                    motion = 0;//동작변수=수납;
                    Serial.println(object+1);
                    Calculator(object, &floor, &direction);//층,방향을 리턴해야하므로 포인터를써 floor랑 direction에 저장
                    Floor_Move(floor,&c_floor);
                    Push(motion, direction);//수납
                    state_p[i] = 2;//수납했으니 상태 2
                    Floor_Move(1, &c_floor);
                    break;// 반복문 탈출
                }
            }
            for (int i = 0;i < 4;i++)// 2. 빈 플랫폼 우선순위순으로 다시 가져옴
            {
                if (state_p[i] == 1)
                {
                    object = i;
                    motion = 1;//동작변수=반납;
                    Calculator(object, &floor, &direction);
                    Floor_Move(floor,&c_floor);
                    Push(motion, direction);//가져오기때문에 반납과같은 알고리즘
                    state_p[i] = 0;//빈플랫폼을 뺏으니 상태 0
                    Floor_Move(1, &c_floor);
                    break;//우선순위때문에 반복문 탈출
                }
            }
        }
        else if (order == 1)//@@@@@@반납@@@@@@
        {
            if (state_p[0] != 0 || state_p[1] != 0 || state_p[2] != 0 || state_p[3] != 0)// 1. 중앙에 빈플랫폼이 있는경우
            {
                for (int i = 0;i < 4;i++)      // 상태 0인곳 찾아서 빈플랫폼 수납
                {
                    if (state_p[i] == 0)//상태0일때 수납
                    {
                        object = i;
                        motion = 0;//동작변수=수납;
                        Calculator(object, &floor, &direction);//층,방향을 리턴해야하므로 포인터를써 floor랑 direction에 저장
                        Floor_Move(floor,&c_floor);
                        Push(motion, direction);//수납
                        state_p[i] = 2;//빈 플랫폼 수납했으니 상태 1
                        Floor_Move(1, &c_floor);
                        break;// 시간 단축위해 그다음장소부턴 생략가능하므로 반복문 탈출
                    }
                }
            }
            object = number - 1; // 2. 물품 반납
            motion = 1;
            Calculator(object, &floor, &direction);
            Push(motion, direction);//반납
            Floor_Move(floor,&c_floor);
            state_p[object] = 0;//반납했으니 그곳의 상태는 0
            Floor_Move(1, &c_floor);
        }
}


//목표변수 : target
//층 : floor
//방향 : dir  -1이면 좌 1이면 우
void Calculator(int obj, int* flo, int* dir) // 4
{
    int left_right = -1;
    for (int j = 1; j <= 4; j++)
    {
        if (obj == j)
        {
            *flo = (j + 1) / 2;
            *dir = left_right;
        }
        left_right = left_right * (-1);
    }
}

//혹시 스텝모터와 DC모터를 같이 사용할 순없는지??? 스텝모터는 수납과 반납 신호로만 사용가능한 반면 DC모터는 수납과 반납시에 왼쪽 오른쪽으로 나눠야해서 전체적인 코드수정가능함 
//따라서 DC모터를 엘레베이터에 사용하고, 스텝모터를 양쪽 좌우 PUSH함수에 사용 가능 할 수 있으면 좋겠음
void Push(int motion, int direction) { //5
    if (motion == 0)// 수납
    {
        digitalWrite(Dir1Pin_A, HIGH);//상승
        digitalWrite(Dir2Pin_A, LOW);
        analogWrite(SpeedPin_A, 1);//숫자는 바꾸면됨(1cm 회전)
        
        if (direction == 1)//오른쪽 수납
        {
            digitalWrite(Dir1Pin_B, HIGH);//오른쪽으로 회전
            digitalWrite(Dir2Pin_B, LOW);
            analogWrite(SpeedPin_B, 255);
            delay(1000);

            digitalWrite(Dir1Pin_A, LOW);//하강
            digitalWrite(Dir2Pin_A, HIGH);
            analogWrite(SpeedPin_A, 2);//(2cm 회전)
            delay(1000);

            digitalWrite(Dir1Pin_B, LOW);//왼쪽으로 회전(가운데로 회전)
            digitalWrite(Dir2Pin_B, HIGH);
            analogWrite(SpeedPin_B, 255);
            delay(1000);
        }
        else//왼쪽수납
        {
            digitalWrite(Dir1Pin_B, LOW);//왼쪽으로 회전
            digitalWrite(Dir2Pin_B, HIGH);
            analogWrite(SpeedPin_B, 255);
            delay(1000);

            digitalWrite(Dir1Pin_A, LOW);//하강
            digitalWrite(Dir2Pin_A, HIGH);
            analogWrite(SpeedPin_A, 2);//(2cm 회전)
            delay(1000);

            digitalWrite(Dir1Pin_B, HIGH);//오른쪽으로 회전
            digitalWrite(Dir2Pin_B, LOW);
            analogWrite(SpeedPin_B, 255);
            delay(1000);
        }
    else// 반납일때
    {
        digitalWrite(Dir1Pin_A, LOW);//하강
        digitalWrite(Dir2Pin_A, HIGH);
        analogWrite(SpeedPin_A, 1);//(2cm 회전)
        delay(1000);

        if (direction == 1)//오른쪽 반납
        {
            digitalWrite(Dir1Pin_B, HIGH);//오른쪽으로 회전
            digitalWrite(Dir2Pin_B, LOW);
            analogWrite(SpeedPin_B, 255);
            delay(1000);

            digitalWrite(Dir1Pin_A, HIGH);//상승
            digitalWrite(Dir2Pin_A, LOW);
            analogWrite(SpeedPin_A, 2);//숫자는 바꾸면됨(2cm 회전)

            digitalWrite(Dir1Pin_B, LOW);//왼쪽으로 회전
            digitalWrite(Dir2Pin_B, HIGH);
            analogWrite(SpeedPin_B, 255);
            delay(1000);
        }
        else//왼쪽반납
        {
            digitalWrite(Dir1Pin_B, LOW);//왼쪽으로 회전
            digitalWrite(Dir2Pin_B, HIGH);
            analogWrite(SpeedPin_B, 255);
            delay(1000);

            digitalWrite(Dir1Pin_A, HIGH);//상승
            digitalWrite(Dir2Pin_A, LOW);
            analogWrite(SpeedPin_A, 2);//숫자는 바꾸면됨(2cm 회전)

            digitalWrite(Dir1Pin_B, HIGH);//오른쪽으로 회전
            digitalWrite(Dir2Pin_B, LOW);
            analogWrite(SpeedPin_B, 255);
            delay(1000);
        }
    }
    else if (motion == 1)//반납
    {
        /*stepper1.step(step1 * (-1));//하강
        delay(1000);
        stepper2.step(direction * step2);
        delay(1000);
        stepper1.step(step1 * 2);
        delay(1000);
        stepper2.step(direction * step2 * (-1));
        delay(1000);*/
        digitalWrite(Dir1Pin_A, LOW);//오른쪽으로 회전
        digitalWrite(Dir2Pin_A, HIGH);
        analogWrite(SpeedPin_A, 255);
        delay(1000);

        digitalWrite(Dir1Pin_A, HIGH);//왼쪽으로 회전
        digitalWrite(Dir2Pin_A, LOW);
        analogWrite(SpeedPin_A, 255);
        delay(1000);
    }
}

void Floor_Move(int floor,int *c_floor)
{
    if (floor == 1 && *c_floor != 1)
    {
        /*stepper1.step(-4074); //숫자는 정확한 값으로 수정해야한다.
        *c_floor = 1;*/
        digitalWrite(Dir1Pin_A, HIGH);//상승
        digitalWrite(Dir2Pin_A, LOW);
        analogWrite(SpeedPin_A, 255);//숫자는 바꾸면됨
    }
    else if (floor == 2 && *c_floor != 2)
    {
        /*stepper1.step(4074); //숫자는 정확한 값으로 수정해야한다.
        *c_floor = 2;*/
        digitalWrite(Dir1Pin_A, LOW);//하강
        digitalWrite(Dir2Pin_A,HIGH);
        analogWrite(SpeedPin_A, 255);//숫자는 바꾸면됨
    }
    
}












#include<Keypad.h>

const byte ROWS = 4;//키패드 부분 변수(Command)
const byte COLS = 3;
char keys[ROWS][COLS] = {
     {'1','2','3'},
     {'4','5','6'},
     {'7','8','9'},
     {'#','0','*'}
};
byte rowPins[ROWS] = { 6,5 };
byte colPins[COLS] = { 4,3,2 };

const int sensorpin1 = A0;//압력센서에 연결한 아날로그 핀(state), 핀번호에맞게 수정해야함, 전역변수
const int sensorpin2 = A1;
const int sensorpin3 = A2;
const int sensorpin4 = A3;
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // 키패드 읽기
void setup()
{
    Serial.begin(9600);
    //stepper1.setSpeed(200); //모터 1 스피드 설정
    //stepper2.setSpeed(200); //모터 2 스피드 설정
   /* pinMode(Dir1Pin_A, OUTPUT);
    pInMode(Dir2Pin_A, OUTPUT);
    pinMode(SpeedPin_A, OUTPUT);
    pinMode(Dir1Pin_B, OUTPUT);
    pInMode(Dir2Pin_B, OUTPUT);
    pinMode(SpeedPin_B, OUTPUT);*/

}
void loop()
{
    int number = 0, order = 0, state[4] = { 0 }, motion = 0, object = 0;
    State(state);// 압력센서 정보받음
    Serial.println("키패드 입력해주세요.");
    key = kpd.getKey();
    if (key)
        Command(&key, &number, &order);//키패드입력
    Serial.println("number변수");
    Serial.println("number");
    Serial.println("order변수");
    Serial.println("order");
    //Route(order, number, state);//수납/반납
  //  Serial.end();//끝

}




void Command(char* key, int* number, int* order)
{
    if (*key != 0)
    {
        switch (*key)
        {
        case '6':
            *order = 6;
            break;
        case '1':
            *order = 1;
            *number = 1;
            Serial.println(*key);//출력안할거면 빼도됨
            break;
        case '2':
            *order = 1;
            *number = 2;
            Serial.println(*key);
            break;
        case '3':
            *order = 1;
            *number = 3;
            Serial.println(*key);
            break;
        case '4':
            *order = 1;
            *number = 4;
            Serial.println(*key);
            break;
        default:
            Serial.println("잘못된 key를 입력했습니다.");
        }
    }
}

void State(int* state)
{
    state[0] = analogRead(sensorpin1);
    state[1] = analogRead(sensorpin2);
    state[2] = analogRead(sensorpin3);
    state[3] = analogRead(sensorpin4);

    /* for (int i = 0;i < 4;i++)
     {
         if (state[i] == 0)
         {
             state[i] = 0;
         }
         else if ((state[i]>=15)&&(state[i]<=100) ) //X값은 압력센서에따라 설정해야함.
         {
             state[i] = 1;
         }
         else if (state[i] > 150)
         {
             state[i] = 2;
         }
     }*/
    Serial.print("첫번째");
    Serial.println(state[0]);
    Serial.print("두번째");
    Serial.println(state[1]);
    Serial.print("세번째");
    Serial.println(state[2]);
    Serial.print("네번째");
    Serial.println(state[3]);
    Serial.print("\n");
    delay(3000);

}
