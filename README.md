Requirements:

For building:
```bash
sudo apt-get update
sudo aapt-get install bc
sudo apt-get install fdisk
```
For runnning i386 apps like old grub with qemu:
```bash
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install libc6:i386 libncurses5:i386 libstdc++6:i386
```
There is a Makefile for interaction with project:

For building:
```bash
sudo make all 
```
For reset:
```bash
sudo make clan
```
For rebuild:
```bash
sudo make rebuild
```
For run with qemu:
```bash
sudo make run
```
