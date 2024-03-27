### Operating system
command:
- 编译汇编程序:
  nasm -o ./mbr ./mbr.s -I ../include/
  nasm -o ./loader ./loader.s -I ../include/
- 写入磁盘
  dd if=./mbr of=../bochs/hd60M.img bs=512 count=1 conv=notrunc
  dd if=./loader of=../bochs/hd60M.img bs=512 count=4 conv=notrunc seek=2
  seek 表明要跳过几个扇区
