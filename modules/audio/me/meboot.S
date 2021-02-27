#include <regdef.h>

#define STATUS	$12
#define CAUSE 	$13
#define CONFIG 	$16
#define TAGLO 	$28
#define TAGHI 	$29
#define IXIST	0x01
#define DXIST	0x11

	.global me_run
	.global me_end
	.set noreorder
	.set noat

me_run:
	li $2, 0xbfc00608
	li $3, 12
	sw $3, 0($2)
	
	li		k0, 0xbc100000
	li		t0, 7
	sw		t0, 80(k0)
	mtc0		zero, TAGLO
	mtc0		zero, TAGHI
	li		k1, 8192

	li $2, 0xbfc00608
	li $3, 0xf0
	sw $3, 0($2)

a:
	addi		k1, k1, -64
	bne		k1, zero, a
	cache		IXIST, 0(k1)
	li		k1, 8192

	li $2, 0xbfc00608
	li $3, 0xf1
	sw $3, 0($2)

b:
	addi		k1, k1, -64
	bne		k1, zero, b

	li $2, 0xbfc00608
	li $3, 0xf2
	li		t0, 0xbfc00000
	lw		k0, 0x600(t0) 
	sw k0, 0($2)

	cache		DXIST, 0(k1)
	mtc0		zero, CAUSE
	li		k0, 0x20000000
	mtc0		k0, STATUS



	sync
	


	li		t0, 0xbfc00000
	lw		a0, 0x604(t0)
	lw		k0, 0x600(t0) 
	li		sp, 0x80200000

	
	jr		k0	
	nop
me_end:
