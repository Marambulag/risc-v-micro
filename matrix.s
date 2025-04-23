.data
A:
    .word 1, 2, 3
    .word 4, 5, 6
    .word 7, 8, 9
B:
    .word 9, 8, 7
    .word 6, 5, 4
    .word 3, 2, 1
C:
    .zero 36  # 9 words (3x3) inicializadas a 0

.text
.globl _start
_start:
    la a0, A           # Direccion base de A
    la a1, B           # Direcci0n base de B
    la a2, C           # Direccion base de C

    li t0, 0           # i = 0

loop_i:
    li t1, 0           # j = 0

loop_j:
    li t2, 0           # sum = 0
    li t3, 0           # k = 0

loop_k:
   
    # Calculo de direccion de A[i][k] 
   
    add t4, t0, t0     # t4 = i*2
    add t4, t4, t0     # t4 = i*3 (i*2 + i)
    add t4, t4, t3     # t4 = i*3 + k
    add t4, t4, t4     # t4 = (i*3 + k)*2
    add t4, t4, t4     # t4 = (i*3 + k)*4 
    add t4, a0, t4     # Direccion de A[i][k]
    lw t5, 0(t4)       # t5 = A[i][k]

    
    # Calculo de direccion de B[k][j] 
    
    add t6, t3, t3     # t6 = k*2
    add t6, t6, t3     # t6 = k*3 (k*2 + k)
    add t6, t6, t1     # t6 = k*3 + j
    add t6, t6, t6     # t6 = (k*3 + j)*2
    add t6, t6, t6     # t6 = (k*3 + j)*4 (byte offset)
    add t6, a1, t6     # Dirección de B[k][j]
    lw t6, 0(t6)       # t6 = B[k][j]

    
    # Multiplicacion por sumas igual que antes
   
    li a3, 0           # producto = 0
    beqz t5, end_mult  # Si A[i][k] es 0 saltar
    beqz t6, end_mult  # Si B[k][j] es 0 saltar

    mv a4, t5          # a4 = valor a sumar A[i][k]
    mv a5, t6          # a5 = contador B[k][j]

mult_loop:
    add a3, a3, a4     # producto += A[i][k]
    addi a5, a5, -1    # decrementar contador
    bnez a5, mult_loop # Repetir si contador != 0

end_mult:
    add t2, t2, a3     # sum += producto

    addi t3, t3, 1     # k++
    li t4, 3
    blt t3, t4, loop_k # Si k < 3 continuar

   
    # Almacenar en C[i][j] 
    
    add t4, t0, t0     # t4 = i*2
    add t4, t4, t0     # t4 = i*3
    add t4, t4, t1     # t4 = i*3 + j
    add t4, t4, t4     # t4 = (i*3 + j)*2
    add t4, t4, t4     # t4 = (i*3 + j)*4 
    add t4, a2, t4     # Direccion de C[i][j]
    sw t2, 0(t4)       # Guardar sum en C[i][j]

    addi t1, t1, 1     # j++
    li t4, 3
    blt t1, t4, loop_j # Si j < 3, continuar

    addi t0, t0, 1     # i++
    li t4, 3
    blt t0, t4, loop_i # Si i < 3, continuar

    
    # Salir
   
    li a7, 10          # Código de exit (10)
    ecall