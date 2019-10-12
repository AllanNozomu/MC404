disk.img: ra163527.out
	mksd.sh --so /home/specg12-1/mc404/simulador/dummyos.elf --user ra163527.out
ra163527.out: ra163527.o
	arm-eabi-ld ra163527.o -o ra163527.out -Ttext=0x77802000 -Tdata=0x77802500
ra163527.o:
	arm-eabi-as ra163527.s -o ra163527.o
clean:
	rm -f ra163527.out ra163527.o disk.img

