;       boot_display.s       boot/bootdisplay.s
;       (C) 2023 Lucky_ma

;  first time using graphics card to display directly
;  ok let's see the code.

; LOADER_BASE_ADDR equ 0xA000
; LOADER_START_SECTION equ 0x2

; set the basic registers
SECTION bootdisplay vstart=0x7c00    
    mov ax,cs                   
    mov ds,ax
    mov es,ax
	mov ss,ax
	mov fs,ax     
	mov sp,0x7c00
	mov ax,0xb800     ;Address of video memory
	mov gs,ax

; clear screen 
    mov ax,0x0600
	mov bx,0x0700
	mov cx,0
	mov dx,0x184f

	int 0x10

; output: background is green, font color is read, and string willflash.
; string: 'lm-0.0'
; just do it.

    mov byte [gs:0x00],'l'
	mov byte [gs:0x01],0xA4

	mov byte [gs:0x02],'m'
	mov byte [gs:0x03],0xA4

	mov byte [gs:0x04],'-'
	mov byte [gs:0x05],0xA4

	mov byte [gs:0x06],'0'
	mov byte [gs:0x07],0xA4

	mov byte [gs:0x08],'.'
	mov byte [gs:0x09],0xA4

	mov byte [gs:0x10],'0'
	mov byte [gs:0x11],0xA4

	jmp $                   ; while(true);
	times 510-($-$$) db 0   ; write empty area
	db 0x55,0xaa            ; boot disk logo

