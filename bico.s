
    @ Global symbol
    .global _start
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

        ldrb r1, [r0]               @ motor ID
        ldrb r2, [r0, #1]           @ motor speed
        stmfd sp!, {r1-r2}          @ empilha os parametros na pilha
        @stmfd sp!, {r1}

        mov r7, #18                 @ syscall #18 set_motor_speed
        svc 0x0                     @ chama a syscall

        add sp, sp, #8              @ desempilha os valores

        ldmfd sp!, {r7, pc}

set_motors_speed:
        stmfd sp!, {r7, lr}

        ldrb r2, [r0, #1]       @ motor0 speed
        ldrb r3, [r1, #1]       @ motor1 speed
        stmfd sp!, {r2-r3}         @ empilha os parametros na pilha
        @stmfd sp!, {r2}

        mov r7, #19             @ syscall #19 set_motors_speed
        svc 0x0                 @ chama a syscall

        add sp, sp, #8              @ desempilha os valores

        ldmfd sp!, {r7, pc}

read_sonar:
        stmfd sp!, {r7, lr}

        stmfd sp!, {r0}         @ empilha os parametros do id do sensor
        mov r7, #16             @ syscall #16 read_sonnar
        svc 0x0                 @ chama a syscall

        ldmfd sp!, {r7, pc}

read_sonars:
        stmfd sp!, {r7, lr}

        mov r3, r2              @ r3 sera o comeco do vetor
        mov r2, r1              @ r2 tera o fim
        mov r1, r0              @ r1 tera o comeco
        mov r7, #16             @ syscall #16 read_sonnar

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

          @mov r7, #17            @ syscall #18 register_proximity_callback
          @svc 0x0

          ldmfd sp!, {r7, pc}

  add_alarm:
          stmfd sp!, {r7, lr}

          @mov r7, #22            @ syscall #18 add_alarm
          @svc 0x0

          ldmfd sp!, {r7, pc}

  get_time:
          stmfd sp!, {r7, lr}

          mov r1, r0
          mov r7, #20            @ syscall #18 get_time
          svc 0x0
          str r0, [r1]

          ldmfd sp!, {r7, pc}

  set_time:
          stmfd sp!, {r7, lr}

          stmfd sp!, {r0}

          mov r7, #21            @ syscall #18 set_time
          svc 0x0

          ldmfd sp!, {r7, pc}
