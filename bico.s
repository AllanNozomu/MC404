
    @ Global symbol
    .global set_motor_speed
    .global set_motors_speed
    .global read_sonar
    .global read_sonars
    .global register_proximity_callback
    .global add_alarm
    .global get_time
    .global set_time

	.align 4
set_motor_speed:
        stmfd sp!, {r7, lr}

        ldrb r1, [r0]           @ motor ID
        ldrb r2, [r0, #1]       @ motor speed
        stmfd sp!, {r1, r2}     @ empilha os parametros na pilha

        mov r7, #18             @ codigo set_motor_speed
        svc 0x0                 @ chama a syscall

        ldmfd sp!, {r7, pc}

set_motors_speed:
        stmfd sp!, {r7, lr}

        ldrb r1, [r0, #2]       @ motor2 ID
        ldrb r2, [r0, #3]       @ motor2 speed
        stmfd sp!, {r1, r2}     @ empilha os parametros do motor 1

        ldrb r1, [r0]         @ motor1 ID
        ldrb r2, [r0, #1]       @ motor1 speed
        stmfd sp!, {r1, r2}     @ empilha os parametros do motor 2

        mov r7, #19             @ codigo set_motors_speed
        svc 0x0                 @ chama a syscall

        ldmfd sp!, {r7, pc}

read_sonar:
        stmfd sp!, {r7, lr}

        stmfd sp!, {r0}         @ empilha os parametros do id do sensor
        mov r7, #16             @ codigo read_sonnar
        svc 0x0                 @ chama a syscall

        ldmfd sp!, {r7, pc}

read_sonars:
        stmfd sp!, {r7, lr}

        mov r3, r2              @ r3 sera o comeco do vetor
        mov r2, r1              @ r2 tera o fim
        mov r1, r0              @ r1 tera o comeco
        mov r7, #16             @ codigo read_sonnar

    loop_sonars:
        cmp r1, r2
        bgt end_loop_sonars

        stmfd sp!, {r1}         @ empilha os parametros do id do sensor
        svc 0x0                 @ chama a syscall
        str r0, [r3]            @ salva o valor no vetor

        add r1, r1, #1
        add r3, r3, #4

        b loop_sonars
    end_loop_sonars:

        ldmfd sp!, {r4, r7, pc}


  register_proximity_callback:
          stmfd sp!, {r7, lr}
          @mov r7, #125            @ Identifica a syscall 125 (read_sonar).
          @svc 0x0
          ldmfd sp!, {r7, pc}

  add_alarm:
          stmfd sp!, {r7, lr}
          @mov r7, #125            @ Identifica a syscall 125 (read_sonar).
          @svc 0x0
          ldmfd sp!, {r7, pc}

  get_time:
          stmfd sp!, {r7, lr}
          @mov r7, #125            @ Identifica a syscall 125 (read_sonar).
          @svc 0x0
          ldmfd sp!, {r7, pc}

  set_time:
          stmfd sp!, {r7, lr}
          @mov r7, #125            @ Identifica a syscall 125 (read_sonar).
          @svc 0x0
          ldmfd sp!, {r7, pc}
