.org 0x0
.section .iv,"a"

_start:

@---------------------------------------------------------------------------
@ VETOR DE INTERRUPCOES
@---------------------------------------------------------------------------

interrupt_vector:
    b RESET_HANDLER
  .org 0x08
    b SYSCALL_HANDLER
  .org 0x18
    b IRQ_HANDLER

  .org 0x100
.text

@---------------------------------------------------------------------------
@ RESET HANDLER
@---------------------------------------------------------------------------

RESET_HANDLER:
	@ Zera o tempo do sistema
    ldr r2, =SYSTEM_TIME  @lembre-se de declarar esse contador em uma secao de dados!
    mov r0,#0
    str r0,[r2]

    ldr r2, =CALLBACKS_QTY  @lembre-se de declarar esse contador em uma secao de dados!
    mov r0,#0
    str r0,[r2]

    ldr r2, =ALARM_QTY  @lembre-se de declarar esse contador em uma secao de dados!
    mov r0,#0
    str r0,[r2]

    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

@---------------------------------------------------------------------------
@ CONFIGURACOES
@---------------------------------------------------------------------------

GPT_CONFIG:
    .set GPT_BASE,            0x53FA0000
    .set GPT_CR,             	0x0
    .set GPT_PR,          		0x4
    .set GPT_SR,				      0x8
    .set GPT_OCR1,          	0x10
    .set GPT_IR,           		0xC
    .set TIME_SZ,  			      512

  	ldr r1, =GPT_BASE

  	mov r0, #0x00000041
  	str r0, [r1, #GPT_CR]

  	mov r0, #0
  	str r0, [r1, #GPT_PR]

  	mov r0, #TIME_SZ
  	str r0, [r1, #GPT_OCR1]

  	mov r0, #1
  	str r0, [r1, #GPT_IR]

@---------------------------------------------------------------------------

GPIO_CONFIG:
    .set GPIO_BASE,             0x53F84000
    .set GPIO_DR,               0x0
    .set GPIO_GDIR,          		0x4
    @.set GPIO_PSR,				      0x8

  	ldr r1, =GPIO_BASE

  	ldr r0, =0xFFFC003E           @ configuracao dos pinos de entrada e saida
  	str r0, [r1, #GPIO_GDIR]

    ldr r0, =0x0                  @ zera as os valores de cada pino
    str r0, [r1, #GPIO_DR]

@---------------------------------------------------------------------------

SET_TZIC:
    @ Constantes para os enderecos do TZIC
    .set TZIC_BASE,             0x0FFFC000
    .set TZIC_INTCTRL,          0x0
    .set TZIC_INTSEC1,          0x84
    .set TZIC_ENSET1,           0x104
    .set TZIC_PRIOMASK,         0xC
    .set TZIC_PRIORITY9,        0x424

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE

    ldr	r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_ENSET1]

    @ Configure interrupt39 priority as 1
    @ reg9, byte 3

    ldr r0, [r1, #TZIC_PRIORITY9]
    bic r0, r0, #0xFF000000
    mov r2, #1
    orr r0, r0, r2, lsl #24
    str r0, [r1, #TZIC_PRIORITY9]

    @ Configure PRIOMASK as 0
    eor r0, r0, r0
    str r0, [r1, #TZIC_PRIOMASK]

    @ Habilita o controlador de interrupcoes
    mov	r0, #1
    str	r0, [r1, #TZIC_INTCTRL]

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled

@---------------------------------------------------------------------------

    msr  CPSR_c, #0x12       @ IRQ mode
    ldr sp, =IRQ_SP

    msr  CPSR_c, #0x1F       @ SYSTEM mode
    ldr sp, =SYSTEM_USER_SP

    msr  CPSR_c, #0x13       @ SUPERVISOR mode
    ldr sp, =SUPERVISOR_SP

    msr  CPSR_c, #0x10       @ USER mode, IRQ/FIQ enabled
    ldr r0, =0x77802000
    mov pc, r0

@---------------------------------------------------------------------------
@ HANDLERS
@---------------------------------------------------------------------------

SYSCALL_HANDLER:
    stmfd sp!, {lr}

    mrs r1, SPSR
    stmfd sp!, {r1}

    cmp r7, #16
    bleq READ_SONAR
    cmp r7, #17
    bleq REGISTER_PROXIMITY_CALLBACK
    cmp r7, #18
    bleq SET_MOTOR_SPEED
    cmp r7, #19
    bleq SET_MOTOR_SPEEDS
    cmp r7, #20
    bleq GET_TIME
    cmp r7, #21
    bleq SET_TIME
    cmp r7, #22
    bleq SET_ALARM

    cmp r7, #30
    bne SYSCALL_HANDLER_END
    add sp, sp, #4
    ldmfd sp!, {lr}
    mov pc, lr

  SYSCALL_HANDLER_END:
    ldmfd sp!, {r1}
    msr SPSR, r1

    ldmfd sp!, {lr}
    @msr  CPSR_c, #0x13            @ Gambs
    movs pc, lr

@---------------------------------------------------------------------------

IRQ_HANDLER:
    stmfd sp!, {r0-r7, lr}

    mrs r0, SPSR                     @ SUPOSTAMENTE SALVA MODE DESCONHECIDO
    stmfd sp!, {r0}                  @ EMPILHA MODE DESCONHECIDO

    msr CPSR_c, #0xD2                  @ IRQ mode

    @ Configura para nao receber interrupcoes durante o processo
    ldr r1, =GPT_BASE
    mov r0, #1
    str r0, [r1, #GPT_SR]

	  @ Aumenta o tempo do sistema
    ldr r0, =SYSTEM_TIME
	  ldr r1, [r0]
    add r1, r1, #1
    str r1,[r0]

    @ Verificar callbacks

    mov r4, #0                        @ indice das callbacks
    msr CPSR_c, #0xD0                   @ USER mode


  LOOP_CALLBACKS_CHECK:
    ldr r3, =CALLBACKS_QTY
    ldr r3, [r3]
    cmp r4, r3
    bge END_CALLBACKS_CHECK

    ldr r3, =CALLBACKS_VET

    mov r1, #16
    mul r2, r4, r1                  @ arruma o endereco do vetor de callbacks
    add r4, r4, #1                  @ soma um no indice das callbacks

    add r3, r3, r2
    ldr r1, [r3]                    @ Id do sonar da callback

    stmfd sp!, {r2-r3}              @ empilha o id do sonar para ser chamada a
    stmfd sp!, {r1}                 @ empilha o id do sonar para ser chamada a
    mov r7, #16
    svc 0x0                         @ funcao read_sonar
    add sp, sp, #4                  @ desempilha
    ldmfd sp!, {r2-r3}              @ empilha o id do sonar para ser chamada a

    ldr r1, [r3, #4]                @ limiar de distancia do callbacks
    cmp r0, r1                      @ compara a distancia com a limiar
    bge LOOP_CALLBACKS_CHECK
    ldr r5, [r3, #12]
    cmp r5, #1
    beq LOOP_CALLBACKS_CHECK

    ldr r2, [r3, #8]
    mov r5, #1
    str r5, [r3, #12]

    @msr CPSR_c, #0x10               @ USER mode
    stmfd sp!, {r3, lr}
    blx r2
    ldmfd sp!, {r3, lr}

    mov r5, #0
    str r5, [r3, #12]

    b LOOP_CALLBACKS_CHECK

  END_CALLBACKS_CHECK:

  mov r4, #0
  LOOP_ALARMS_CHECK:
    ldr r3, =ALARM_QTY
    ldr r3, [r3]
    cmp r4, r3
    bge END_ALARMS_CHECK

    ldr r3, =ALARM_VET

    mov r1, #8
    mul r2, r4, r1                  @ arruma o endereco do vetor de callbacks
    add r4, r4, #1                  @ soma um no indice das callbacks

    add r3, r3, r2
    ldr r1, [r3, #4]                @ tempo

    ldr r0, =SYSTEM_TIME
    ldr r0, [r0]

    cmp r0, r1                      @ compara o tempo dos alarmes
    ble LOOP_ALARMS_CHECK

    ldr r2, [r3]
    stmfd sp!, {r3, lr}
    blx r2
    ldmfd sp!, {r3, lr}

    mov r1, #8
    ldr r0, =ALARM_QTY
    ldr r5, [r0]
    sub r5, r5, #1
    str r5, [r0]
    mul r2, r5, r1                  @ arruma o endereco do vetor de callbacks

    ldr r1, =ALARM_VET
    add r1, r1, r2

    ldr r0, [r1]
    str r0, [r3]
    ldr r0, [r1, #4]
    str r0, [r3, #4]

    sub r4, r4, #1

    @msr CPSR_c, #0x10               @ USER mode

    b LOOP_ALARMS_CHECK

  END_ALARMS_CHECK:

    @ Verificar alarms

    mov r7, #30
    svc 0x0

    msr CPSR_c, #0xD2                  @ IRQ mode

    ldmfd sp!, {r0}
    msr SPSR, r0

    ldmfd sp!, {r0-r7, lr}

	  sub lr, lr, #4
	  movs pc, lr

@---------------------------------------------------------------------------
@ FUNCOES
@---------------------------------------------------------------------------

READ_SONAR:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0xDF                @ SYSTEM mode
    ldr r4, [sp]                      @ Id sonar

    cmp r4, #15
    bls READ_SONAR_INI
    mov r0, #-1

    b READ_SONNAR_END

  READ_SONAR_INI:

    mov r3, #0xF                      @ mascara para pegar somente os 4 bits do sonar
    and r4, r4, r3

    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em PSR
    ldr r2, [r2, #GPIO_DR]

    bic r2, r2, r3, LSL #2            @ zera os sonars_mux usando a mascara

    orr r2, r2, r4, LSL #2            @ seta o id do sonar em R2 (PSR)

    ldr r3, =GPIO_BASE                @ atualiza o pino DR do GPIO

    bic r2, r2, #0x2                  @ TRIGGER = 0
    str r2, [r3, #GPIO_DR]            @ SET DEFINITIVO

    mov r4, #4096
  LOOP_TRIGGER_1:
    sub r4, r4, #1
    cmp r4, #0
    bge LOOP_TRIGGER_1

    orr r2, r2, #0x2                  @ TRIGGER = 1
    str r2, [r3, #GPIO_DR]            @ SET DEFINITIVO

    mov r4, #4096
  LOOP_TRIGGER_2:
    sub r4, r4, #1
    cmp r4, #0
    bge LOOP_TRIGGER_2

    bic r2, r2, #0x2                  @ TRIGGER = 0
    ldr r1, =GPIO_BASE                @ atualiza o pino DR do GPIO
    str r2, [r1, #GPIO_DR]            @ SET DEFINITIVO

  LOOP_FLAG:
    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em PSR
    ldr r2, [r2, #GPIO_DR]

    and r2, r2, #1
    cmp r2, #1
    beq FLAG_ONE

    mov r4, #8192

  FLAG_DELAY:
    sub r4, r4, #1
    cmp r4, #0
    bgt FLAG_DELAY
    b LOOP_FLAG

  FLAG_ONE:

    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em PSR
    ldr r2, [r2, #GPIO_DR]

    ldr r3, =SONAR_DISTANCE_MASK      @ mascara para pegar somente os 4 bits do sonar
    and r2, r2, r3, LSL #6

    mov r2, r2, LSR #6
    mov r0, r2

  READ_SONNAR_END:

    msr  CPSR_c, #0xD3       @ SUPERVISOR mode
    ldmfd sp!, {r4, pc}

    @b SYSCALL_HANDLER_END

REGISTER_PROXIMITY_CALLBACK:
    stmfd sp!, {r4-r5, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode

    ldr r5, =CALLBACKS_QTY
    ldr r5, [r5]

    cmp r5, #8                        @ verificar numero de callbacks ativos
    movhi r0, #-1
    bhi REGISTER_PROXIMITY_CALLBACK_END

    ldr r1, [sp]

    cmp r1, #15                       @ verificar id sonar
    movhi r1, #-2
    bhi REGISTER_PROXIMITY_CALLBACK_END

    mov r2, #16                       @ tamanho ocupado pelos tres parametros

    mul r4, r5, r2                    @ tamanho total utilizado
                                      @ pelo num de callbacks

    ldr r3, =CALLBACKS_VET            @ salvar id no vetor de callbacks
    str r1, [r3, r4]

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #4]                  @ carregar limiar de distancia
    str r1, [r3, r4]                  @ salvar limiar

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #8]                  @ carregar posicao da funcao
    str r1, [r3, r4]                  @ salvar posicao

    add r4, r4, #4                    @ posicao do proximo parametro
    mov r1, #0                        @ nao esta executando
    str r1, [r3, r4]                  @ salvar posicao

    ldr r1, =CALLBACKS_QTY            @ atualizar qtd de callbacks
    ldr r5, [r1]
    add r5, r5, #1
    str r5, [r1]

    mov r0, #0                        @ retorna 0

  REGISTER_PROXIMITY_CALLBACK_END:
    msr  CPSR_c, #0x13                @ SUPERVISOR mode
    ldmfd sp!, {r4-r5, pc}

SET_MOTOR_SPEED:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r4, [sp]                      @ Carrego o valor do id
    ldr r1, [sp, #4]                  @ valor da velocidade do motor

    cmp r4, #1
    bls SET_MOTOR_SPEED_VALID_ID      @ Verifica se o ID é 0 ou 1

    mov r0, #-1
    b SET_MOTOR_SPEED_END

  SET_MOTOR_SPEED_VALID_ID:           @ Verifica se a velocidade é <= 0xF e >= 0

    cmp r1, #0x3F
    bls SET_MOTOR_SPEED_INI

    mov r0, #-2
    b SET_MOTOR_SPEED_END

  SET_MOTOR_SPEED_INI:

    ldr r3, =MOTOR_SPEED_MASK        @ mascara para pegar somente os 6 primeiros bits
    and r1, r1, r3

    ldr r2, =GPIO_BASE              @ obtendo o estado atual dos pinos definido em PSR
    ldr r2, [r2, #GPIO_DR]

    ldr r3, =DR_MOTOR_SPEED_MASK    @ mascara para zerar os valores atuais em PSR

    cmp r4, #1
    beq MOTOR_1

    bic r2, r2, r3, LSL #25
    orr r2, r2, r1, LSL #26         @ seta a nova velocidade do motor1 em R2 (PSR)
    mov r4, #1
    bic r2, r2, r4, LSL #25        @ seta o pino do MOTOR0_WRITE para 0
    b SET_MOTOR_SPEED_SET

  MOTOR_1:
    bic r2, r2, r3, LSL #18
    orr r2, r2, r1, LSL #19         @ seta a nova velocidade do motor0 em R2 (PSR)
    mov r4, #1
    bic r2, r2, r4, LSL #18         @ seta o pino do MOTOR0_WRITE para 0

  SET_MOTOR_SPEED_SET:
    @ldr r2, =0Xfdf80000
    ldr r1, =GPIO_BASE              @ atualiza o pino DR do GPIO
    str r2, [r1, #GPIO_DR]          @ SET DEFINITIVO

    mov r0, #0                      @ Retorno correto da funcao

  SET_MOTOR_SPEED_END:

    msr  CPSR_c, #0x13              @ SUPERVISOR mode
    ldmfd sp!, {r4, pc}

    @b SYSCALL_HANDLER_END

SET_MOTOR_SPEEDS:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r4, [sp]                      @ Carrego o valor da velocidade do motor 0
    ldr r1, [sp, #4]                  @ valor da velocidade do motor 1
    msr  CPSR_c, #0x13       @ SUPERVISOR mode

    cmp r4, #0x3F                       @ Verifica a velocidade do motor 0
    bls SET_MOTOR_SPEEDS_VALID_1

    mov r0, #-1
    b SET_MOTOR_SPEEDS_END

  SET_MOTOR_SPEEDS_VALID_1:

    cmp r1, #0x3F                       @ Verifica a velocidade do motor 1
    bls SET_MOTOR_SPEEDS_VALID_2

    mov r0, #-2
    b SET_MOTOR_SPEEDS_END

  SET_MOTOR_SPEEDS_VALID_2:

    ldr r3, =MOTOR_SPEED_MASK        @ mascara para pegar somente os 6 primeiros bits
    and r4, r4, r3
    and r1, r1, r3

    ldr r2, =GPIO_BASE              @ obtendo o estado atual dos pinos definido em PSR
    ldr r2, [r2, #GPIO_DR]

    ldr r3, =DR_MOTOR_SPEED_MASK    @ mascara para zerar os valores atuais em PSR
    bic r2, r2, r3, LSL #18
    bic r2, r2, r3, LSL #25

    orr r2, r2, r4, LSL #19         @ seta a nova velocidade do motor0 em R3 (PSR)
    orr r2, r2, r1, LSL #26         @ seta a nova velocidade do motor1 em R3 (PSR)

    mov r4, #1
    bic r2, r2, r4, LSL #18         @ seta o pino do MOTOR0_WRITE para 0
    bic r2, r2, r4, LSL #25         @ seta o pino do MOTOR1_WRITE para 0

    @ldr r2, =0Xfdf80000
    ldr r1, =GPIO_BASE              @ atualiza o pino DR do GPIO
    str r2, [r1, #GPIO_DR]          @ SET DEFINITIVO

    @mov r4, #1
    @orr r2, r2, r4, LSL #18         @ seta o pino do MOTOR0_WRITE para 1
    @orr r2, r2, r4, LSL #25         @ seta o pino do MOTOR1_WRITE para 1
    @
    @str r2, [r1, #GPIO_DR]          @ SET DEFINITIVO

    mov r0, #0

  SET_MOTOR_SPEEDS_END:

    ldmfd sp!, {r4, pc}

    @b SYSCALL_HANDLER_END

GET_TIME:

    stmfd sp!, {lr}

    ldr r0, =SYSTEM_TIME
    ldr r0, [r0]

    ldmfd sp!, {pc}

    @b SYSCALL_HANDLER_END

SET_TIME:
    stmfd sp!, {lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r0, [sp]                      @ Carrego o valor do id

    ldr r1, =SYSTEM_TIME
    str r0, [r1]

    msr  CPSR_c, #0x13                @ SUPERVISOR mode

    ldmfd sp!, {pc}

    @b SYSCALL_HANDLER_END

SET_ALARM:

    stmfd sp!, {r4-r5, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode

    ldr r5, =ALARM_QTY
    ldr r5, [r5]

    cmp r5, #8                        @ verificar numero de callbacks ativos
    movhi r0, #-1
    bhi SET_ALARM_END

    ldr r1, [sp, #4]

    bl GET_TIME

    cmp r5, r1
    movgt r0, #-2
    bgt SET_ALARM_END

    mov r2, #8

    ldr r5, =ALARM_QTY
    ldr r5, [r5]                    @ tamanho ocupado pelos dois parametros

    mul r4, r5, r2                    @ tamanho total utilizado
                                      @ pelo num de alarms

    ldr r1, [sp]                      @ carrega o end da funcao

    ldr r3, =ALARM_VET                @ salva end no vetor de alarms
    str r1, [r3, r4]

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #4]                  @ carregar tempo do sistema
    str r1, [r3, r4]                  @ salvar tempo

    ldr r1, =ALARM_QTY                @ atualizar qtd de alarms
    ldr r5, [r1]
    add r5, r5, #1
    str r5, [r1]

    mov r0, #0                        @ retorna 0

  SET_ALARM_END:
    msr  CPSR_c, #0x13                @ SUPERVISOR mode
    ldmfd sp!, {r4-r5, pc}

@---------------------------------------------------------------------------
@ DATA E CONSTANTES
@---------------------------------------------------------------------------

.set MAX_ALARMS,            0x00000008
.set MAX_CALLBACKS,         0x00000008

.set MOTOR_SPEED_MASK,      0x0000003F
.set DR_MOTOR_SPEED_MASK,   0x0000007F
.set SONAR_MASK,            0x0000000F
.set SONAR_DISTANCE_MASK,   0x00000FFF

.set SYSTEM_TIME,           0x77801800
.set CALLBACKS_VET,         0x77801804

.set CALLBACKS_QTY,         0x77801884

.set ALARM_VET,             0x77801888

.set ALARM_QTY,             0x778018C8

.set SUPERVISOR_SP,         0x77801E00
.set SYSTEM_USER_SP,        0x77801F00
.set IRQ_SP,                0x77802000

.data
.org 0x0      @ 0x77801800

.space 4      @ SYSTEM_TIME
.space 128    @ CALLBACKS_VET   4*8*4
.space 4      @ CALLBACKS_QTY
.space 64     @ ALARM_VET       2*8*4
.space 4      @ ALARM_QTY

.org  0x500     @ 0x77801800 + 0x600 = 0x77801D00
.space 0x100    @ D00 - E00
.space 0x100    @ E00 - F00
.space 0x100    @ F00 - 000
