

loadercopy:
	sudo mount -o loop fd.img /mnt/
	sudo rm -f /mnt/LOADER.BIN
	sudo cp ./LOADER.BIN /mnt/
	sudo umount /mnt/
    
kernelcopy:
	rm -f kernel.bin
	rm -f kernel.bin.stripped
	cp -v ../kernel/kernel.bin ./
	strip kernel.bin -o kernel.bin.stripped
	sudo mount -o loop fd.img /mnt/
	sudo rm -f /mnt/KERNEL.BIN
	sudo cp -fv ./kernel.bin.stripped /mnt/KERNEL.BIN
	sudo umount /mnt/
