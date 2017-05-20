#include<mips/regdef.h>
#include<sys/syscall.h>

.rdata		
version:  .asciiz "Organizacion de Computadoras - TP1 \nEncoder/Decoder Base64 - v2.2 \nGroup Members:\nGonzalez Perez, Ailen Padron: 97043\nMariotti, Maria Eugenia Padron: 96260\nRaña, Cristian Ezequiel Padron: 95457\n"		
help: .asciiz "Usage: \ntp0 -h \ntp1 -V \ntp0 [options] \nOptions: \n-V, --version Print version and quit.\n-h, --help Print this information.\n-i, --input Location of the input file.\n-o, --output Location of the output file.\n-a, --action Program action: encode (default) or decode.\nExamples:\ntp1 -a encode -i ~/input -o ~/output\ntp1 -a decode \n"

.text		
.abicalls		

.ent show_version		
.globl show_version		
show_version: 
	.frame $fp,8,ra		
	.set noreorder		
	.cpload t9		
	.set reorder		
	
	subu sp,sp,8		
	sw $fp,0(sp)			
	.cprestore 4		

	li v0,SYS_write		
	li a0,1 # show_version es stdout  	
	la a1,version		
	li a2,195 #frase de 195 caracteres (1 character = 8 bits = 1 byte)	
	syscall		
					
	lw $fp,0(sp)		
	lw gp,4(sp)			
	addu sp,sp,8		
	jr ra
.end show_version
		

.ent show_help		
.globl show_help	
show_help: 
	.frame $fp,8,ra		
	.set noreorder		
	.cpload t9		
	.set reorder		
	
	subu sp,sp,8		
	sw $fp,0(sp)			
	.cprestore 4		

	li v0,SYS_write		
	li a0,1 # show_version es stdout  	
	la a1,help		
	li a2,326 #frase de 326 caracteres (1 character = 8 bits = 1 byte)	
	syscall		
					
	lw $fp,0(sp)		
	lw gp,4(sp)			
	addu sp,sp,8		
	jr ra
.end show_help
