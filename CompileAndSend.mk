COMP = /home/hans/Desktop/Projekt_Rasp/Labor_Rasp/buildroot/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-g++.br_real
MFRC522 = /home/hans/Desktop/RFID_PRO/Labor_Rasp/MFRC522
DataName = /home/hans/Desktop/RFID_PRO/Labor_Rasp/Read
WriteMasterKey = /home/hans/Desktop/RFID_PRO/Labor_Rasp/writeMasterKey
DataPI = /root/Read
PI_IP = root@192.168.0.22


all: $(DataName) $(WriteMasterKey)

$(DataName): $(MFRC522).cpp $(DataName).cpp
	$(COMP) $(MFRC522).cpp $(DataName).cpp -o $(DataName) -std=c++11 -lbcm2835

$(WriteMasterKey): $(MFRC522).cpp $(WriteMasterKey).cpp
	$(COMP) $(MFRC522).cpp $(WriteMasterKey).cpp -o $(WriteMasterKey) -std=c++11 -lbcm2835

.PHONY: clean update run updateWriteMaster

update:
	scp $(DataName) $(PI_IP):

updateWriteMaster:
	scp $(WriteMasterKey) $(PI_IP):

clean:
	rm $(DataName)

run:
	ssh $(PI_IP) $(DataPI)

