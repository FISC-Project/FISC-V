	.text
	.file	"a.ll"
	.globl	main
	.type	main,@function
main:                                   # @main
# BB#0:                                 # %entry
	subi sp, sp, 0x18
	str lr, [sp, 0x10]
	strw x19, [sp, 0x8]
	str x20, [sp, 0x0]
	bl %call26(start)
	#APP
	bl 0
	#NO_APP
	movz x0, 0x0, lsl 0x0
	nop
	nop
	ldr lr, [sp, 0x10]
	addi sp, sp, 0x18
	br lr
.Lfunc_end0:
	.size	main, .Lfunc_end0-main

	.globl	foo
	.type	foo,@function
foo:                                    # @foo
# BB#0:                                 # %entry
	movz x0, 0x7, lsl 0x0
	nop
	nop
	br lr
.Lfunc_end1:
	.size	foo, .Lfunc_end1-foo

	.globl	start
	.type	start,@function
start:                                  # @start
# BB#0:                                 # %entry
	subi sp, sp, 0x18
	str lr, [sp, 0x10]
	movrz x0, %movrel(foo), lsl 0x0
	nop
	nop
	str x0, [sp, 0x8]
	bl %call26(foo)
	strw x0, [sp, 0x0]
	ldr x0, [sp, 0x8]
	bl x0
	movz x1, 0x0, lsl 0x0
	movk x1, 0x1, lsl 0x1
	nop
	nop
	strw x0, [x1, 0x0]
	ldr x0, [sp, 0x8]
	bl x0
	movz x1, 0x4, lsl 0x0
	movk x1, 0x1, lsl 0x1
	nop
	nop
	strw x0, [x1, 0x0]
	ldrsw x0, [sp, 0x0]
	movz x1, 0x8, lsl 0x0
	movk x1, 0x1, lsl 0x1
	nop
	nop
	strw x0, [x1, 0x0]
	movz x0, 0xc, lsl 0x0
	movk x0, 0x1, lsl 0x1
	movz x1, 0xa, lsl 0x0
	nop
	nop
	strw x1, [x0, 0x0]
	ldr lr, [sp, 0x10]
	addi sp, sp, 0x18
	br lr
.Lfunc_end2:
	.size	start, .Lfunc_end2-start


	.ident	"clang version 3.8.1 "
	.ident	"clang version 3.8.1 "
	.section	".note.GNU-stack","",@progbits
