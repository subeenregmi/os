
all: build/my.iso

build/my.iso: build/isodir/boot/myos.bin build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/myos.iso build/isodir

build/isodir/boot/myos.bin: build/myos.bin
	cp build/myos.bin build/isodir/boot/myos.bin

build/isodir/boot/grub/grub.cfg: grub.cfg
	cp grub.cfg build/isodir/boot/grub/grub.cfg

build/myos.bin: build/boot.o build/kernel.o src/linker.ld
	i686-elf-gcc -T src/linker.ld -o build/myos.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernel.o -lgcc
	grub-file --is-x86-multiboot build/myos.bin

build/boot.o: src/boot.asm
	nasm -felf32 src/boot.asm -o build/boot.o

build/kernel.o: src/kernel.c
	i686-elf-gcc -c src/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

qemu: all
	qemu-system-i386 -cdrom build/myos.iso

clean: 
	rm build/*.o build/*.bin build/isodir/boot/myos.bin build/isodir/boot/grub/* build/*.iso

