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
    ldr r2, =SYSTEM_TIME
    mov r0,#0
    str r0,[r2]

    @ Zera o contador de callbacks
    ldr r2, =CALLBACKS_QTY
    mov r0,#0
    str r0,[r2]

    @ Zera o contador de alarmes
    ldr r2, =ALARM_QTY
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
    .set GPT_CR,              0x0
    .set GPT_PR,              0x4
    .set GPT_SR,              0x8
    .set GPT_OCR1,            0x10
    .set GPT_IR,              0xC
    .set TIME_SZ,             512

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
    .set GPIO_GDIR,             0x4
    .set GPIO_PSR,              0x8

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

    ldr  r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov  r0, #(1 << 7)
    str  r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov  r0, #(1 << 7)
    str  r0, [r1, #TZIC_ENSET1]

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
    mov  r0, #1
    str  r0, [r1, #TZIC_INTCTRL]

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled

@---------------------------------------------------------------------------
@ MODOS
@---------------------------------------------------------------------------

    msr  CPSR_c, #0x12       @ IRQ mode
    ldr sp, =IRQ_SP          @ Inicializacao da pilha IRQ

    msr  CPSR_c, #0x1F       @ SYSTEM mode
    ldr sp, =SYSTEM_USER_SP  @ Inicializacao da pilha SYSTEM

    msr  CPSR_c, #0x13       @ SUPERVISOR mode
    ldr sp, =SUPERVISOR_SP   @ Inicializacao da pilha SUPERVISOR

    msr  CPSR_c, #0x10       @ USER mode, IRQ/FIQ enabled
    ldr r0, =0x77802000      @ Mover fluxo de execucao para main
    mov pc, r0

@---------------------------------------------------------------------------
@ HANDLERS
@---------------------------------------------------------------------------

SYSCALL_HANDLER:
    stmfd sp!, {lr}

    mrs r1, SPSR                        @ Salva modo antigo na pilha
    stmfd sp!, {r1}

    @ Verificacao da syscall salva em r7
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

    @ Syscall especial - Muda modo para supervisor
    cmp r7, #30
    bne SYSCALL_HANDLER_END
    add sp, sp, #4                @ Desempilha modo antigo na pilha
    ldmfd sp!, {lr}
    mov pc, lr

  SYSCALL_HANDLER_END:
    ldmfd sp!, {r1}
    msr SPSR, r1                  @ Muda para modo antigo salvo na pilha

    ldmfd sp!, {lr}
    movs pc, lr

@---------------------------------------------------------------------------

IRQ_HANDLER:
    stmfd sp!, {r0-r7, lr}

    mrs r0, SPSR                       @ Salva modo antigo
    stmfd sp!, {r0}

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

    mov r4, #0                         @ contador de callbacks
    msr CPSR_c, #0x10                  @ USER mode

  LOOP_CALLBACKS_CHECK:             @ Loop para verificar cada callback
    ldr r3, =CALLBACKS_QTY          @ Carrega quantidade de callbacks no vetor
    ldr r3, [r3]
    cmp r4, r3
    bge END_CALLBACKS_CHECK

    ldr r3, =CALLBACKS_VET          @ Carrega end do vetor de callbacks

    @ VETOR DE CALLBACKS - Desenho explicativo
    @ _____________________________________________________________________
    @ |             |                     |          |                     |
    @ | ID do sonar | Limiar de distancia | Ponteiro | Flag aitva/ inativa |
    @ |_____________|_____________________|__________|_____________________|

    mov r1, #16                     @ 4 parametros de 4 bytes = 16
    mul r2, r4, r1                  @ multiplica 16*contador
    add r3, r3, r2                  @ atualiza o endereco do vetor de callbacks
    add r4, r4, #1                  @ incrementa contador de callbacks

    ldr r5, [r3, #12]               @ Verifica a flag de callback ativa ou inativa
    cmp r5, #1
    beq LOOP_CALLBACKS_CHECK

    ldr r1, [r3]                    @ Id do sonar da callback
    stmfd sp!, {r2-r3}              @ Salva registradores caller save
    stmfd sp!, {r1}                 @ empilha o id do sonar para ser chamada a read_sonar
    mov r7, #16
    svc 0x0                         @ svc para read_sonar
    add sp, sp, #4                  @ desempilha
    ldmfd sp!, {r2-r3}              @ desempilha registradores caller

    ldr r1, [r3, #4]                @ carrega limiar de distancia do callbacks
    cmp r0, r1                      @ compara a distancia com a limiar
    bge LOOP_CALLBACKS_CHECK

    mov r5, #1                      @ Seta flag da callback para ativa (flag = 1)
    str r5, [r3, #12]               @ Salva flag

    ldr r2, [r3, #8]                @ Carrega ponteiro da funcao a ser chamada pela callback
    stmfd sp!, {r3, lr}             @ Empilha registradores caller save e lr
    blx r2                          @ Chamada da funcao
    ldmfd sp!, {r3, lr}             @ Desempilha

    mov r5, #0                      @ Seta flag da callback para inativa (flag = 0)
    str r5, [r3, #12]               @ Salva flag

    b LOOP_CALLBACKS_CHECK

  END_CALLBACKS_CHECK:              @ Fim do loop

    mov r4, #0                      @ Contador de alarmes

  LOOP_ALARMS_CHECK:
    ldr r3, =ALARM_QTY              @ Carrega quantidade de alarmes
    ldr r3, [r3]
    cmp r4, r3
    bge END_ALARMS_CHECK

    ldr r3, =ALARM_VET              @ Carrega endereco do vetor de alarmes

    mov r1, #8                      @ 2 parametros de 4 bytes = 8
    mul r2, r4, r1                  @ Multiplica 8*contador de alarmes
    add r4, r4, #1                  @ Incrementa contador de alarmes

    add r3, r3, r2                  @ Atualiza endereco do vetor de alarmes
    ldr r1, [r3, #4]                @ Carrega parametro de tempo do alarme

    ldr r0, =SYSTEM_TIME            @ Carrega tempo do sistema
    ldr r0, [r0]

    cmp r0, r1                      @ compara o tempo dos alarmes com o tempo de sistema
    ble LOOP_ALARMS_CHECK

    mov r6, r3                      @ salva o endereco do ponteiro da funcao


    @ Bloco de codigo que transfere ultimo alarme do vetor
    @ no lugar do alarme que vai ser executado, sobreescrevendo-o
    mov r1, #8
    ldr r0, =ALARM_QTY
    ldr r5, [r0]
    sub r5, r5, #1
    str r5, [r0]
    mul r2, r5, r1
    ldr r1, =ALARM_VET
    add r1, r1, r2

    @ Atualiza valores do alarme transferido
    ldr r0, [r1]
    str r0, [r3]
    ldr r0, [r1, #4]
    str r0, [r3, #4]
    sub r4, r4, #1                  @ Decrementa contador de alarmes


    ldr r2, [r6]                    @ Carrega ponteiro da funcao a ser chamada
    stmfd sp!, {lr}
    blx r2
    ldmfd sp!, {lr}

    b LOOP_ALARMS_CHECK

  END_ALARMS_CHECK:

    mov r7, #30                     @ Chamada da syscall para mudar de modo para supervisor
    svc 0x0

    msr CPSR_c, #0xD2               @ IRQ mode

    ldmfd sp!, {r0}
    msr SPSR, r0                    @ Carrega modo antigo

    ldmfd sp!, {r0-r7, lr}

    sub lr, lr, #4
    movs pc, lr

@---------------------------------------------------------------------------
@ FUNCOES
@---------------------------------------------------------------------------

    @---------------------------------------------------------------------------
    @ READ_SONAR
    @---------------------------------------------------------------------------

READ_SONAR:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0xDF                @ muda para SYSTEM mode para pegar o parametro
    ldr r4, [sp]                      @ Carrega o id do sonar

    cmp r4, #15                       @ Verificacao se o id do sonar eh valido
    bls READ_SONAR_INI
    mov r0, #-1                       @ retorno invalido

    b READ_SONNAR_END

  READ_SONAR_INI:

    mov r3, #0xF                      @ mascara para pegar somente os 4 bits do sonar
    and r4, r4, r3                    @ aplica a mascara no parametro

    ldr r2, =GPIO_BASE                @ pega o estado atual dos pinos em DR
    ldr r2, [r2, #GPIO_DR]

    bic r2, r2, r3, LSL #2            @ zera os pinos do sonar_mux usando a mascara

    orr r2, r2, r4, LSL #2            @ seta o id do sonar (r2) nos pinos do sonar_mux (DR)

    bic r2, r2, #0x2                  @ TRIGGER = 0

    ldr r3, =GPIO_BASE                @ carrega o endereco do GPI_BASE para salvar os pinos
    str r2, [r3, #GPIO_DR]            @ SET DEFINIIVO dos pinos em DR

    mov r4, #4096                     @ delay para trigger 0
  LOOP_TRIGGER_1:
    sub r4, r4, #1
    cmp r4, #0
    bge LOOP_TRIGGER_1

    orr r2, r2, #0x2                  @ TRIGGER = 1
    str r2, [r3, #GPIO_DR]            @ SET DEFINITIVO do trigger em DR

    mov r4, #4096                     @ delay para trigger 1
  LOOP_TRIGGER_2:
    sub r4, r4, #1
    cmp r4, #0
    bge LOOP_TRIGGER_2

    bic r2, r2, #0x2                  @ TRIGGER = 0
    str r2, [r3, #GPIO_DR]            @ SET DEFINITIVO do trigger em DR

  LOOP_FLAG:
    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em DR
    ldr r2, [r2, #GPIO_DR]

    and r2, r2, #1
    cmp r2, #1                        @ verifica se a flag foi setada para parar o loop
    beq FLAG_ONE

    mov r4, #8192                     @ delay da flag

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

    @---------------------------------------------------------------------------
    @ REGISTER_PROXIMITY_CALLBACK
    @---------------------------------------------------------------------------

REGISTER_PROXIMITY_CALLBACK:
    stmfd sp!, {r4-r5, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode

    ldr r5, =CALLBACKS_QTY
    ldr r5, [r5]

    cmp r5, #8                        @ Verifica se o numero de callbacks limite foi atingido
    movge r0, #-1                     @ retorno se foi atingido o numero maximo de callbacks
    bge REGISTER_PROXIMITY_CALLBACK_END

    ldr r1, [sp]                      @ Carrega o id do sonar

    cmp r1, #15                       @ verifica se o id do sonar eh id valido
    movhi r1, #-2                     @ retorno se o id do sonar eh valido
    bhi REGISTER_PROXIMITY_CALLBACK_END

    @ VETOR DE CALLBACKS - Desenho explicativo
    @ _____________________________________________________________________
    @ |             |                     |          |                     |
    @ | ID do sonar | Limiar de distancia | Ponteiro | Flag aitva/ inativa |
    @ |_____________|_____________________|__________|_____________________|

    mov r2, #16                       @ 4 parametros de 4 bytes = 16,

    mul r4, r5, r2                    @ para atuliazao o vetor de callbacks

    ldr r3, =CALLBACKS_VET            @ salvar id no vetor de callbacks
    str r1, [r3, r4]

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #4]                  @ Carrega a limiar de distancia
    str r1, [r3, r4]                  @ salvar no vetor a limiar

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #8]                  @ Carrega o ponteiro da funcao
    str r1, [r3, r4]                  @ salvar no vetor o ponteiro

    add r4, r4, #4                    @ posicao do proximo parametro
    mov r1, #0                        @ 0 = inativa (pode ser executada)
    str r1, [r3, r4]                  @ salva no vetor a inatividade do callback

    ldr r1, =CALLBACKS_QTY            @ atualiza a quantidade de callbacks
    ldr r5, [r1]
    add r5, r5, #1
    str r5, [r1]

    mov r0, #0                        @ retorna 0

  REGISTER_PROXIMITY_CALLBACK_END:
    msr  CPSR_c, #0x13                @ SUPERVISOR mode
    ldmfd sp!, {r4-r5, pc}

    @---------------------------------------------------------------------------
    @ SET_MOTOR_SPEED
    @---------------------------------------------------------------------------

SET_MOTOR_SPEED:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r4, [sp]                      @ Carrega o valor do id do motor
    ldr r1, [sp, #4]                  @ Carrega a velocidade do motor

    cmp r4, #1
    bls SET_MOTOR_SPEED_VALID_ID      @ Verifica se o ID é 0 ou 1 (valido)

    mov r0, #-1                       @ Retorna -1 se o ID for invalido
    b SET_MOTOR_SPEED_END

  SET_MOTOR_SPEED_VALID_ID:

    cmp r1, #0x3F                     @ Verifica se a velocidade eh valida
    bls SET_MOTOR_SPEED_INI

    mov r0, #-2                       @ Retorna -2 se a velocidade for invalida
    b SET_MOTOR_SPEED_END

  SET_MOTOR_SPEED_INI:

    ldr r3, =MOTOR_SPEED_MASK         @ mascara para pegar somente os 6 primeiros bits
    and r1, r1, r3                    @ aplica a mascara no na velocidade

    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em DR
    ldr r2, [r2, #GPIO_DR]

    ldr r3, =DR_MOTOR_SPEED_MASK      @ mascara para zerar os valores atuais em DR

    cmp r4, #1                        @ se for o motor 1 executa o motor 1
    beq MOTOR_1

    bic r2, r2, r3, LSL #25           @ zera os valores da velocidade do moto 0
    orr r2, r2, r1, LSL #26           @ seta a nova velocidade do motor1 em R2 (DR)
    mov r4, #1
    bic r2, r2, r4, LSL #25           @ seta o pino do MOTOR0_WRITE para 0
    b SET_MOTOR_SPEED_SET

  MOTOR_1:
    bic r2, r2, r3, LSL #18           @ zera os valores da velocidade do moto 1
    orr r2, r2, r1, LSL #19           @ seta a nova velocidade do motor1 em R2 (DR)
    mov r4, #1
    bic r2, r2, r4, LSL #18           @ seta o pino do MOTOR1_WRITE para 0

  SET_MOTOR_SPEED_SET:
    ldr r1, =GPIO_BASE                @ atualiza o pino DR do GPIO
    str r2, [r1, #GPIO_DR]            @ SET DEFINITIVO dos pinos alterados em DR

    mov r0, #0                        @ Retorno correto da funcao

  SET_MOTOR_SPEED_END:

    msr  CPSR_c, #0x13                @ SUPERVISOR mode
    ldmfd sp!, {r4, pc}

    @---------------------------------------------------------------------------
    @ SET_MOTOR_SPEEDS
    @---------------------------------------------------------------------------

SET_MOTOR_SPEEDS:
    stmfd sp!, {r4, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r4, [sp]                      @ Carrega o valor da velocidade do motor 0
    ldr r1, [sp, #4]                  @ Carrega o valor da velocidade do motor 1
    msr  CPSR_c, #0x13                @ SUPERVISOR mode

    cmp r4, #0x3F                     @ Verifica a velocidade do motor 0
    bls SET_MOTOR_SPEEDS_VALID_1

    mov r0, #-1                       @ Motor 0 invalido
    b SET_MOTOR_SPEEDS_END

  SET_MOTOR_SPEEDS_VALID_1:

    cmp r1, #0x3F                     @ Verifica a velocidade do motor 1
    bls SET_MOTOR_SPEEDS_VALID_2

    mov r0, #-2                       @ Motor 1 invalido
    b SET_MOTOR_SPEEDS_END

  SET_MOTOR_SPEEDS_VALID_2:

    ldr r3, =MOTOR_SPEED_MASK         @ mascara para pegar somente os 6 primeiros bits
    and r4, r4, r3                    @ Aplica a mascara para o motor 0
    and r1, r1, r3                    @ Aplica a mascara para o motor 1

    ldr r2, =GPIO_BASE                @ obtendo o estado atual dos pinos definido em DR
    ldr r2, [r2, #GPIO_DR]

    ldr r3, =DR_MOTOR_SPEED_MASK      @ mascara para zerar os valores atuais das velocidades em DR
    bic r2, r2, r3, LSL #18
    bic r2, r2, r3, LSL #25

    orr r2, r2, r4, LSL #19           @ seta a nova velocidade do motor0 em R3 (DR)
    orr r2, r2, r1, LSL #26           @ seta a nova velocidade do motor1 em R3 (DR)

    mov r4, #1
    bic r2, r2, r4, LSL #18           @ seta o pino do MOTOR0_WRITE para 0 (escrita)
    bic r2, r2, r4, LSL #25           @ seta o pino do MOTOR1_WRITE para 0 (escrita)

    ldr r1, =GPIO_BASE                @ atualiza o pino DR do GPIO
    str r2, [r1, #GPIO_DR]            @ SET DEFINITIVO

    mov r0, #0

  SET_MOTOR_SPEEDS_END:

    ldmfd sp!, {r4, pc}

    @---------------------------------------------------------------------------
    @ GET_TIME
    @---------------------------------------------------------------------------

GET_TIME:

    stmfd sp!, {lr}

    ldr r0, =SYSTEM_TIME              @ Carrega o tempo e retorno em r0
    ldr r0, [r0]

    ldmfd sp!, {pc}

    @---------------------------------------------------------------------------
    @ SET_TIME
    @---------------------------------------------------------------------------

SET_TIME:
    stmfd sp!, {lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode
    ldr r0, [sp]                      @ Carrego o novo tempo que sera setado

    ldr r1, =SYSTEM_TIME              @ carrega o endereco do SYSTEM_TIME
    str r0, [r1]                      @ seta o novo tempo

    msr  CPSR_c, #0x13                @ SUPERVISOR mode

    ldmfd sp!, {pc}

    @---------------------------------------------------------------------------
    @ SET_ALARM
    @---------------------------------------------------------------------------

SET_ALARM:

    stmfd sp!, {r4-r5, lr}

    msr  CPSR_c, #0x1F                @ SYSTEM mode

    ldr r5, =ALARM_QTY                @ carrega quantidade de alarmes
    ldr r5, [r5]

    cmp r5, #8                        @ verificar numero de alarmes ativos
    movge r0, #-1                     @ retorno se foi atingido o numero maximo de alarmes
    bge SET_ALARM_END

    ldr r1, [sp, #4]                  @ carrega parametro tempo

    bl GET_TIME                       @ pega tempo do sistema

    cmp r0, r1                        @ verifica se o parametro tempo eh
    movgt r0, #-2                     @ menor que o tempo do sistema
    bgt SET_ALARM_END

    mov r2, #8                        @ 2 parametros * 4 bytes = 8
    mul r4, r5, r2                    @ tamanho total utilizado
                                      @ pelo num de alarms

    ldr r1, [sp]                      @ carrega o end da funcao

    ldr r3, =ALARM_VET                @ salva end no vetor de alarms
    str r1, [r3, r4]

    add r4, r4, #4                    @ posicao do proximo parametro
    ldr r1, [sp, #4]                  @ carregar parametro tempo
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
.org 0x0                    @ 0x77801800

.space 4                    @ SYSTEM_TIME
.space 128                  @ CALLBACKS_VET   tamanho = 4*8*4
.space 4                    @ CALLBACKS_QTY
.space 64                   @ ALARM_VET       tamanho = 2*8*4
.space 4                    @ ALARM_QTY

.org  0x500                 @ 0x77801800 + 0x600 = 0x77801D00
.space 0x100                @ D00 - E00
.space 0x100                @ E00 - F00
.space 0x100                @ F00 - 000
