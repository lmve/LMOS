# 2023    lucky_ma
# 定义一些变量

BUILD_DIR = ./build
# 内核起始地址
ENTRY_POINT = 0xc0001500                  
#HD60M_PATH = /home/luckyma/桌面/os/bochs-gdb/hd60M.img    #支持  c-gdb
HD60M_PATH = /home/luckyma/桌面/os/bochs/hd60M.img    

# 12.27 Add thread
# 12.28 Add userprog
# 12.29 Add fs
AS = nasm
CC = gcc-4.4
LD = ld
LIB = -I lib/ -I lib/user/ -I lib/kernel/ -I kernel/ -I device/ -I thread/ -I userprog/ -I fs/ -I shell/
ASFLAGS = -f elf -g

#-Wall warning wall的意思，产生尽可能多警告信息，-fno-builtin不要采用内部函数，
#-W 会显示警告，但是只显示编译器认为会出现错误的警告
#-Wstrict-prototypes 要求函数声明必须有参数类型，否则发出警告。-Wmissing-prototypes 必须要有函数声明，否则发出警告
# -fno-stack-protector 禁用栈保护措施
# -m32 表示在64位系统下编译32位的二进制文件，需要额外的库支持
CFLAGS = -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -m32 -fno-stack-protector -g

LDFLAGS = -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_DIR)/kernel.map -m elf_i386
#-Map,生成map文件，就是通过编译器编译之后，生成的程序、数据及IO空间信息的一种映射文件
#里面包含函数大小，入口地址等一些重要信息

#规则
# 12.26 add debug.o string.o bitmap.o memory.o
# 12.27 add thread.o list.o switch.o
# 12.28 add sync.o console.o keyboard.o ioqueue.o 
#           tss.o process.o syscall.o syscall-init.o stdio.o
# 12.29 add stdio-kernel.o ide.o
# after fs  
#       fs.o inode.o file.o dir.o
# 12.30 add fork.o shell.o buildin_cmd.o
#
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o \
	   $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/timer.o \
	   $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
	   $(BUILD_DIR)/debug.o $(BUILD_DIR)/string.o \
	   $(BUILD_DIR)/bitmap.o $(BUILD_DIR)/memory.o \
	   $(BUILD_DIR)/thread.o $(BUILD_DIR)/list.o \
	   $(BUILD_DIR)/switch.o $(BUILD_DIR)/sync.o \
	   $(BUILD_DIR)/console.o $(BUILD_DIR)/keyboard.o \
	   $(BUILD_DIR)/ioqueue.o $(BUILD_DIR)/tss.o \
	   $(BUILD_DIR)/process.o $(BUILD_DIR)/syscall.o \
	   $(BUILD_DIR)/syscall-init.o $(BUILD_DIR)/stdio.o \
	   $(BUILD_DIR)/stdio-kernel.o $(BUILD_DIR)/ide.o \
	   $(BUILD_DIR)/fs.o $(BUILD_DIR)/inode.o \
	   $(BUILD_DIR)/file.o $(BUILD_DIR)/dir.o \
	   $(BUILD_DIR)/fork.o $(BUILD_DIR)/shell.o \
	   $(BUILD_DIR)/buildin_cmd.o
#顺序最好是调用在前，实现在后

# - - - - - - - boot section - - - - - - - - - - - 
boot:$(BUILD_DIR)/mbr.o $(BUILD_DIR)/loader.o
$(BUILD_DIR)/mbr.o:boot/mbr.s
	$(AS) -I include/ -o build/mbr.o boot/mbr.s
$(BUILD_DIR)/loader.o:boot/loader.s
	$(AS) -I include/ -o build/loader.o boot/loader.s




# - - - - - - - C kernel section - - - - - - - - - - - - -

$(BUILD_DIR)/main.o:kernel/main.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/init.o:kernel/init.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/interrupt.o:kernel/interrupt.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/timer.o:device/timer.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/debug.o:kernel/debug.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/string.o:lib/user/string.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/bitmap.o:lib/kernel/bitmap.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/memory.o:kernel/memory.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/thread.o:thread/thread.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/list.o:lib/kernel/list.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/sync.o:thread/sync.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/console.o:device/console.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/keyboard.o:device/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/ioqueue.o:device/ioqueue.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/tss.o:userprog/tss.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/process.o:userprog/process.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/syscall.o:lib/user/syscall.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/syscall-init.o:userprog/syscall-init.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/stdio.o:lib/user/stdio.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/stdio-kernel.o:lib/kernel/stdio-kernel.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/ide.o:device/ide.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/fs.o:fs/fs.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/inode.o:fs/inode.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/file.o:fs/file.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/dir.o:fs/dir.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/fork.o:userprog/fork.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/shell.o:shell/shell.c
	$(CC) $(CFLAGS) -o $@ $<
$(BUILD_DIR)/buildin_cmd.o:shell/buildin_cmd.c
	$(CC) $(CFLAGS) -o $@ $<
# $@表示规则中目标文件名的集合这里就是$(BUILD_DIR)/main.o  $<表示规则中依赖文件的第一个，这里就是kernle/main.c



# - - - - - - - Assembly Language section - - - - - - - - -
$(BUILD_DIR)/kernel.o:kernel/kernel.s 
	$(AS) $(ASFLAGS) -o $@ $<
$(BUILD_DIR)/print.o:lib/kernel/print.s
	$(AS) $(ASFLAGS) -o $@ $<
$(BUILD_DIR)/switch.o:thread/switch.s
	$(AS) $(ASFLAGS) -o $@ $<
	
	
# - - - - - -  LD all - - - - - - - - - - - - - - - - - - - 
$(BUILD_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
# $^表示规则中所有依赖文件的集合，如果有重复，会自动去重
#
# 定义6个伪目标 7个
.PHONY:mk_dir hd clean build all boot gdb_symbol
mk_dir:
	if [ ! -d $(BUILD_DIR) ];then mkdir $(BUILD_DIR);fi
# 判断是否存在build目录
#
hd:
	dd if=build/mbr.o of=$(HD60M_PATH) count=1 bs=512 conv=notrunc && \
	dd if=build/loader.o of=$(HD60M_PATH) count=4 bs=512 conv=notrunc seek=2 && \
	dd if=$(BUILD_DIR)/kernel.bin of=$(HD60M_PATH) count=200 bs=512 conv=notrunc seek=9

clean:
	@cd $(BUILD_DIR) && rm -f ./* && echo "remove ./build all files down."

build:$(BUILD_DIR)/kernel.bin

gdb_symble:
	objcopy --only-keep-debug $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/kernel.sym

all:mk_dir boot build hd gdb_symble
