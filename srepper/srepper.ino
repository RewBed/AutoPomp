#include <GyverStepper.h>


// шаговик
GStepper<STEPPER4WIRE> stepper(2048, 2, 4, 3, 5);

// концевик найден или нет
bool searched_null = false;

// концевик
byte stepper_btn = A0;

void setup() {
  Serial.begin(9600);

  // режим следования к целевй позиции
  stepper.setRunMode(FOLLOW_POS);

  // установка макс. скорости в шагах/сек
  stepper.setMaxSpeed(400);

  // установка ускорения в шагах/сек/сек
  stepper.setAcceleration(500);

  // отправляем мотор сделать полный оборот, для поиска концевика
  stepper.setTargetDeg(360);

}
void loop() {

  // штатная работа
  if(searched_null) {
    
  }

  // колибровка
  else {

    stepper.tick();

    // если сработал концевик
    if(digitalRead(stepper_btn) == 0) {
      Serial.println("reset");
      stepper.reset();
      searched_null = true;
    }
  }

}