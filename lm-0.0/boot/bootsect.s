;    bootsect.s              boot/bootsect.s
;    (c) 2023   lucky_ma

;  This file is the master bootloader.

%include "boot.inc"
SECTION bootsect vstart=0x7c00
    mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov fs,ax
	mov sp,0x7c00
	mov ax,0xb800
	mov gs,ax

; Clear the screen
    mov ax,0x600
	mov bx,0x700
	mov cx,0
	mov dx,0x184f

	int 0x10

; Print 
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

    mov eax,LOADER_START_SECTOR     ;The start sector address
	mov bx,LOADER_BASE_ADDR         ;The start write address
	mov cx,1                        ;read sector's number
	call rd_disk_m_16               ;function to read first sector
	jmp LOADER_BASE_ADDR

rd_disk_m_16:	   
                                    ;-------------------------------------------------------------------------------
				                    ; eax=LBA扇区号
				                    ; ebx=将数据写入的内存地址
				                    ; ecx=读入的扇区数
    mov esi,eax	                    ;备份eax
    mov di,cx		                ;备份cx
                                    ;读写硬盘:
                                    ;第1步：选择特定通道的寄存器，设置要读取的扇区数
    mov dx,0x1f2
    mov al,cl
    out dx,al                       ;读取的扇区数

    mov eax,esi	                    ;恢复ax

                                    ;第2步：在特定通道寄存器中放入要读取扇区的地址，将LBA地址存入0x1f3 ~ 0x1f6
                                    ;LBA地址7~0位写入端口0x1f3
    mov dx,0x1f3                       
    out dx,al                          

                                    ;LBA地址15~8位写入端口0x1f4
    mov cl,8
    shr eax,cl
    mov dx,0x1f4
    out dx,al

                                    ;LBA地址23~16位写入端口0x1f5
    shr eax,cl
    mov dx,0x1f5
    out dx,al

    shr eax,cl
    and al,0x0f	                    ;lba第24~27位
    or al,0xe0	                    ; 设置7～4位为1110,表示lba模式
    mov dx,0x1f6
    out dx,al

                                    ;第3步：向0x1f7端口写入读命令，0x20 
    mov dx,0x1f7
    mov al,0x20                        
    out dx,al

                                    ;第4步：检测硬盘状态
.not_ready:
                                    ;同一端口，写时表示写入命令字，读时表示读入硬盘状态
    nop
    in al,dx
    and al,0x88	                    ;第4位为1表示硬盘控制器已准备好数据传输，第7位为1表示硬盘忙
    cmp al,0x08
    jnz .not_ready	                ;若未准备好，继续等。

                                    ;第5步：从0x1f0端口读数据
    mov ax, di                      ;di当中存储的是要读取的扇区数
    mov dx, 256                     ;每个扇区512字节，一次读取两个字节，所以一个扇区就要读取256次，与扇区数相乘，就等得到总读取次数
    mul dx                          ;8位乘法与16位乘法知识查看书p133,注意：16位乘法会改变dx的值！！！！
    mov cx, ax	                    ; 得到了要读取的总次数，然后将这个数字放入cx中
    mov dx, 0x1f0
.go_on_read:
    in ax,dx
    mov [bx],ax
    add bx,2		  
    loop .go_on_read
    ret

    times 510-($-$$) db 0
    db 0x55,0xaa
