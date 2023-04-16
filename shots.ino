#include <ServoSmooth.h>

// пин подключения сервы
const int servoPin = 13;
const int relePin = 12;

// сервопривод
ServoSmooth servo;

int shots_count = 7;                                // количество рюмок
int shots[7] = {2, 3, 4, 5, 6, 7, 8};               // адреса рюмок
int shots_deg[7] = {180, 151, 116, 80, 47, 18, 0};  // углы для рюмок

bool shots_active[7] = {}; // рюмки в очереди
bool shots_poured[7] = {}; // налитые рюмки

bool filling_up = false; // идет процес заполнения

int filling_up_time = 1500; // время налитя рюмки
int last_filling_time = 0;  // время последнего старта налива
int filling_up_timeout = 1000; // задержка перед налитием
int filling_up_index = 0;

int move_timeout = 2000; // задержка перед перемещением серво

void setup() {

  Serial.begin(9600);

  // установка конопок для рюмок в режим считывания
  for (int i = 0; i < shots_count; i++) {
    pinMode(shots[i], INPUT_PULLUP);
    shots_active[i] = false;
    shots_poured[i] = false;
    Serial.println(shots[i]);
  }

  // настройка серво
  servo.attach(servoPin, 600, 2400);
  servo.setSpeed(10);         // ограничить скорость
  servo.setAccel(0);          // установить ускорение (разгон и торможение)
  servo.setAutoDetach(false); // вкл/выкл автоматического отключения (detach) при достижении угла. По умолч. вкл

  pinMode(relePin, OUTPUT);
}

void loop() {

  int timer = millis();

  // активация очереди  
  Serial.print("a: ");

  for (int i = 0; i < shots_count; i++) {
    int btn = digitalRead(shots[i]);

    // если рюмка до этого не была поставлена, то нужно ее добавить в очередь
    if(btn == 1 && shots_active[i] != true) {
      shots_active[i] = true;
    }

    // если рюмка была в очереди но ее сняли, то нужно удалить ее из очереди
    if(btn == 0 && shots_active[i] != false) {
      shots_active[i] = false;
      shots_poured[i] = false;
    }

    if(shots_active[i] == true) {
      Serial.print(i);
      Serial.print(" ");
    }
    
  }

  Serial.print("p: ");
  for (int i = 0; i < shots_count; i++) {
    if(shots_poured[i] == true) {
      Serial.print(i);
      Serial.print(" ");
    }
  }

  // налив
  bool servoOnPos = servo.tick();

  // если не идет процесс налития
  if(filling_up == false) {

    // если текущая позиция активна и не налита то запускаем процесс налития

    Serial.print(servo.getCurrentDeg());

    for (int i = 0; i < shots_count; i++) {
      if(shots_deg[i] == servo.getCurrentDeg() && shots_active[i] == true && shots_poured[i] == false) {

        Serial.print("filling_up_timeout");
        delay(filling_up_timeout);

        filling_up = true;
        last_filling_time = timer;
        filling_up_index = i;
        digitalWrite(relePin, HIGH);
      }
    }

    // если процесс налития не был запущен то ищем пустую активную рюмку
    if(!filling_up) {

      // беребор активных позиций
      for (int i = 0; i < shots_count; i++) {

        // если рюмка активна и не налита
        if(shots_active[i] == true && shots_poured[i] == false) {
          
          Serial.print("move_timeout");
          delay(move_timeout);
          
          // отправляем серво к ней
          servo.write(shots_deg[i]);
        }
      }
    }
    
  }

  // идет процесс налития
  else {
    
    if(shots_active[filling_up_index] == true) {
      Serial.print("filling_up");

      if(timer - last_filling_time >= filling_up_time) {
        shots_poured[filling_up_index] = true;
        digitalWrite(relePin, LOW);
        filling_up = false;
      }
    }
    else {
      filling_up = false;
      digitalWrite(relePin, LOW);
    }

  }

  Serial.println(" ");

}
