#include "api_robot2.h" /* Robot control API */

/*segue-parede.c*/

// void virarEsq();
void virar();
void virarEsq();
void virarDir();
void frente();
void delay();
void parar();

int flag = 0;
int esquerda = 0;
int tempo =0;

/* main function */
void _start(void)
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  // unsigned int distances[16];

  add_alarm(&frente, 0);
  // add_alarm(&frente, 1000);
  // add_alarm(&frente, 500);
  // add_alarm(&frente, 250);
  // add_alarm(&parar, 500);
  // // add_alarm(&parar, 1000);
  // add_alarm(&virar, 750);
  // delay();
  //
  // // register_proximity_callback(1, 1000, &faz_nada);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  register_proximity_callback(3, 1200, &virarDir);
  // register_proximity_callback(4, 1200, &virarEsq);
  // register_proximity_callback(4, 1200, &virar);
  // // register_proximity_callback(5, 2100, &virarDir);
  do
  {

    frente();
    delay();
    // parar();
    // delay();
    // motor0.speed = 31;
    // motor1.speed = 31;
    // set_motors_speed(&motor0, &motor1);
    // delay();
    // int d3 = read_sonar(3);
    // int d4 = read_sonar(4);
    //
    // if (flag == 2) {
    //
    // }
    // else if (flag == 1)
    // {
    //   motor0.speed = 0;
    //   motor1.speed = 5;
    //   set_motors_speed(&motor0, &motor1);
    //
    //   int tempo;
    //   get_time(&tempo);
    //   add_alarm(&parar, tempo + 7);
    //   flag = 2;
    // }
    // else
    // {
    //   if (d3 < 1200 || d4 < 1200) {
    //     flag = 1;
    //   } else {
    //     motor0.speed = 32;
    //     motor1.speed = 32;
    //     set_motors_speed(&motor0, &motor1);
    //   }
    // }
    // delay();

  //
  // d = read_sonar(3);
      // get_time(&i);
      // if (i < 5){
      //     motor0.speed = 63;
      //     motor1.speed = 0;
      // } else {
      //     motor0.speed = 0;
      //     motor1.speed = 0;
      // }
      // set_motors_speed(&motor0, &motor1);
      // if (i > 10)
      //   set_time(0);

  //
  //     // delay();
  //
  //     // while(i == j){
  //     //   get_time(&i);
  //     // }
  //
  //     // if (i  > 25){
  //     //   motor0.speed = 31;
  //     //   motor1.speed = 31;
  //     // }
  //     // else{
  //     //   motor0.speed = 0;
  //     //   motor1.speed = 31;
  //     // }
  //
  //     // if (i > 50){
  //     //   i = 0;
  //     //   set_time(0);
  //     // }
  //     // if (i > 10)
  //     //   i = 0;
  //
  //   // distances[3] = read_sonar(3);
  //   // distances[4] = read_sonar(4);
  //   // delay();
  //   // if (distances[3] < 1000)
  //   // {
  //   //   motor0.speed = 0;
  //   //   motor1.speed = 25;
  //   //   set_motors_speed(&motor0, &motor1);
  //   // }
  //   // else if (distances[4] < 1000)
  //   // {
  //   //   motor0.speed = 25;
  //   //   motor1.speed = 0;
  //   //   set_motors_speed(&motor0, &motor1);
  //   // }
  //   // else{
  //   //   motor0.speed = 25;
  //   //   motor1.speed = 25;
  //   //   set_motors_speed(&motor0, &motor1);
  //   // }
  } while (1);
}

/* Spend some time doing nothing. */
void delay()
{
  int i;
  /* Not the best way to delay */
  for(i = 0; i < 10000; i++ );
}
//
void frente()
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  motor0.speed = 63;
  motor1.speed = 63;
  set_motors_speed(&motor0, &motor1);

  // int time1;
  // get_time(&time1);
  // tempo += 200;
  // add_alarm(&virar, time1 + tempo);
}

void virar()
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  motor0.speed = 0;
  motor1.speed = 15;
  set_motors_speed(&motor0, &motor1);

  // int time2;
  // get_time(&time2);
  // add_alarm(&frente, time2 + 360);
}
//
void parar()
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  motor0.speed = 0;
  motor1.speed = 0;
  set_motors_speed(&motor0, &motor1);
}

void virarEsq()
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  motor0.speed = 15;
  motor1.speed = 0;

  set_motors_speed(&motor0, &motor1);
}

void virarDir()
{
  motor_cfg_t motor0;
  motor0.id = 0;

  motor_cfg_t motor1;
  motor1.id = 1;

  motor0.speed = 0;
  motor1.speed = 15;

  set_motors_speed(&motor0, &motor1);
}
//
// void virar()
// {
//   motor_cfg_t motor0;
//   motor0.id = 0;
//
//   motor_cfg_t motor1;
//   motor1.id = 1;

  // int d3 = read_sonar(3);
  // int d4 = read_sonar(4);
  // if (d3 < 1200 || d4 < 1200)
  // {
  //   if (d3 > d4) {
  //     motor0.speed = 15;
  //     motor1.speed = 0;
  //   } else {
  //     motor0.speed = 0;
  //     motor1.speed = 15;
  //   }
  // }
  // set_motors_speed(&motor0, &motor1);
  // int time;
  // get_time(&time);
  // add_alarm(&frente, time + 3);
// }
//
// void parar() {
//   motor_cfg_t motor0;
//   motor0.id = 0;
//
//   motor_cfg_t motor1;
//   motor1.id = 1;
//
//   motor0.speed = 0;
//   motor1.speed = 0;
//   set_motors_speed(&motor0, &motor1);
// }
