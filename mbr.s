
 ; boot
 ; (c) 2023  luckyma


SECTION MBR vstart=0x7c00 ;Have the compiler compile the starting address to 0x7c00
	mov ax,cs         ;BIOS jump to MBR (jmp 0:0x7c00) 
        mov ds,ax
        mov es,ax
	mov ss,ax
	mov fs,ax
	mov sp,0x7c00

; clear screen            
	mov ax,0x600      ;AH : function number 0x06; AL : cow numbers (0 is all)  
	mov bx,0x700      ;BH : attribute
	mov cx,0          ;(CL , CH) window's posstion of top left corner
	mov dx,0x184f     ;(DL , DH) lower right corner    VGA text mode just hold 80 chars in a line, totally 25 lines
	int 0x10

; get the posstion of the cursor
	mov ah,3          ;function number
	mov bh,0          ;bh save the information of wait cursor's page number
	int 0x10

; print 
	mov ax,message    
	mov bp,ax        ; es : bp is message's address
	mov cx,10       ; message's lenght
	mov ax,0x1301    ; 
	mov bx,0x3
	int 0x10
	
	jmp $
	message db " (c) lm..."
	times 510-($-$$) db 0
	db 0x55,0xaa
