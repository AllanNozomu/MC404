.globl _start

.data

input_buffer:   .skip 32
output_buffer:  .skip 32
    
.text
.align 4

@ Funcao inicial
_start:
    @ Chama a funcao "read" para ler 4 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #5             @ 4 caracteres + '\n'
    bl  read
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "atoi" para converter a string para um numero
    ldr r0, =input_buffer
    mov r1, r4
    bl  atoi

    @ Chama a funcao "encode" para codificar o valor de r0 usando
    @ o codigo de hamming.
    bl  encode
    mov r4, r0             @ copia o retorno para r4.
	
    @ Chama a funcao "itoa" para converter o valor codificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #7
    mov r2, r4
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #7]

    @ Chama a funcao write para escrever os 7 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #8         @ 7 caracteres + '\n'
    bl  write

    @ Chama a funcao "read" para ler 7 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #8             @ 7 caracteres + '\n'
    bl  read
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "atoi" para converter a string para um numero
    ldr r0, =input_buffer
    mov r1, r4
    bl  atoi

    @ Chama a funcao "decode" para decodificar o valor de r0 usando
    @ o codigo de hamming.
    bl  decode
    mov r4, r0             @ copia o retorno para r4.
    mov r5, r1             @ copia o segundo retorno para r5.
	
    @ Chama a funcao "itoa" para converter o valor decodificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #4
    mov r2, r4
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 4)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #4]

    @ Chama a funcao write para escrever os 4 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #5         @ 4 caracteres + '\n'
    bl  write

    @ Chama a funcao "itoa" para converter o valor decodificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #1
    mov r2, r5
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 1)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #1]

    @ Chama a funcao write para escrever o caracter e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #2         @ 1 caracter + '\n'
    bl  write

    @ Chama a funcao "atoi" para converter a string para um numero
    @ Chama a funcao "atoi" para converter a string para um numero
    @ Chama a funcao exit para finalizar processo.
    mov r0, #0
    bl  exit

@ Codifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (4 bits menos significativos)
@ retorno:
@  r0: valor codificado (7 bits como especificado no enunciado).
encode:    
       push {r4-r11, lr}
       
       @ <<<<<< MEU CODIGO AQUI >>>>>>
       mov r1, r0, lsr #3       @ d1
       and r1, r1, #1

       mov r2, r0, lsr #2       @ d2
       and r2, r2, #1

       mov r3, r0, lsr #1       @ d3
       and r3, r3, #1

       mov r4, r0               @ d4
       and r4, r4, #1

       mov r0, #0               @ zera o retorno pois sera feitouma serie de ORS
       orr r0, r0, r1, lsl #4   @ coloca em r0 os respectivos d1, d2, d3 e d4
       orr r0, r0, r2, lsl #2
       orr r0, r0, r3, lsl #1
       orr r0, r0, r4
    
       eor r5, r1, r2           @ p1
       eor r5, r5, r4
       orr r0, r0, r5, lsl #6   @ Coloca o p1 em r0 

       eor r5, r1, r3           @ p2
       eor r5, r5, r4
       orr r0, r0, r5, lsl #5   @ Coloca o p2 em r0

       eor r5, r2, r3           @ p3
       eor r5, r5, r4           
       orr r0, r0, r5, lsl #3   @ Coloca o p3 em r0
       @ <<<<<< FIM DO MEU CODIGO AQUI >>>>>>

       pop  {r4-r11, lr}
       mov  pc, lr

@ Decodifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (7 bits menos significativos)
@ retorno:
@  r0: valor decodificado (4 bits como especificado no enunciado).
@  r1: 1 se houve erro e 0 se nao houve.
decode:    
       push {r4-r11, lr}
       
       @ <<<<<< MEU CODIGO AQUI >>>>>>
       mov r1, r0, lsr #4       @ d1
       and r1, r1, #1

       mov r2, r0, lsr #2       @ d2
       and r2, r2, #1

       mov r3, r0, lsr #1       @ d3
       and r3, r3, #1

       mov r4, r0               @ d4
       and r4, r4, #1

       mov r7, #0               @ Armazena se havera erro

       mov r5, r0, lsr #6       @ verificacao do p1
       and r5, r5, #1
       mov r6, #0
       eor r6, r5, r1
       eor r6, r6, r2
       eor r6, r6, r4

       orr r7, r6               @ Muda para 1 se houver erro

       mov r5, r0, lsr #5       @ verificacao do p2
       and r5, r5, #1
       mov r6, #0
       eor r6, r5, r1
       eor r6, r6, r3
       eor r6, r6, r4

       orr r7, r6               @ Muda para 1 se houver outro erro em p2

       mov r5, r0, lsr #3       @ verificacao do p3
       and r5, r5, #1
       mov r6, #0
       eor r6, r5, r2
       eor r6, r6, r3
       eor r6, r6, r4           @ Muda para 1 se houver outro erro em p3

       orr r7, r6

       mov r0, #0               @ Monta a saida em r0, colocando
       orr r0, r0, r1, lsl #3   @ d1
       orr r0, r0, r2, lsl #2   @ d2
       orr r0, r0, r3, lsl #1   @ d3
       orr r0, r0, r4           @ d4 respectivamente

       mov r1, r7               @ Retorna o erro tambem
       @ <<<<<< FIM DO MEU CODIGO AQUI >>>>>>

       pop  {r4-r11, lr}
       mov  pc, lr

@ Le uma sequencia de bytes da entrada padrao.
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de bytes.
@  r1: numero maximo de bytes que pode ser lido (tamanho do buffer).
@ retorno:
@  r0: numero de bytes lidos.
read:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #0         @ stdin file descriptor = 0
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho maximo.
    mov r7, #3         @ read
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Escreve uma sequencia de bytes na saida padrao.
@ parametros:
@  r0: endereco do buffer de memoria que contem a sequencia de bytes.
@  r1: numero de bytes a serem escritos
write:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #1         @ stdout file descriptor = 1
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho do buffer.
    mov r7, #4         @ write
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Finaliza a execucao de um processo.
@  r0: codigo de finalizacao (Zero para finalizacao correta)
exit:    
    mov r7, #1         @ syscall number for exit
    svc 0x0

@ Converte uma sequencia de caracteres '0' e '1' em um numero binario
@ parametros:
@  r0: endereco do buffer de memoria que armazena a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@ retorno:
@  r0: numero binario
atoi:
    push {r4, r5, lr}
    mov r4, r0         @ r4 == endereco do buffer de caracteres
    mov r5, r1         @ r5 == numero de caracteres a ser considerado 
    mov r0, #0         @ number = 0
    mov r1, #0         @ loop indice
atoi_loop:
    cmp r1, r5         @ se indice == tamanho maximo
    beq atoi_end       @ finaliza conversao
    mov r0, r0, lsl #1 
    ldrb r2, [r4, r1]  
    cmp r2, #'0'       @ identifica bit
    orrne r0, r0, #1   
    add r1, r1, #1     @ indice++
    b atoi_loop
atoi_end:
    pop {r4, r5, lr}
    mov pc, lr

@ Converte um numero binario em uma sequencia de caracteres '0' e '1'
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@  r2: numero binario
itoa:
    push {r4, r5, lr}
    mov r4, r0
itoa_loop:
    sub r1, r1, #1         @ decremento do indice
    cmp r1, #0          @ verifica se ainda ha bits a serem lidos
    blt itoa_end
    and r3, r2, #1
    cmp r3, #0
    moveq r3, #'0'      @ identifica o bit
    movne r3, #'1'
    mov r2, r2, lsr #1  @ prepara o proximo bit
    strb r3, [r4, r1]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r4, r5, lr}
    mov pc, lr    
