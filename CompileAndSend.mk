COMP = /home/hans/Desktop/Projekt_Rasp/Labor_Rasp/buildroot/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++.br_real
MFRC522 = /home/hans/Desktop/RFID PRO/Labor_Rasp/MFRC522
DataName = /home/hans/Desktop/RFID PRO/Labor_Rasp/Read
DataPI = /root/Read
PI_IP = root@192.168.0.22


all: $(DataName)

$(DataName): $(MFRC522).cpp $(DataName).cpp
	$(COMP) $(MFRC522).cpp $(DataName).cpp -o $(DataName) -std=c++11 -lbcm2835

.PHONY: clean update run

update:
	scp $(DataName) $(PI_IP):

clean:
	rm $(DataName)

run:
	ssh $(PI_IP) $(DataPI)

