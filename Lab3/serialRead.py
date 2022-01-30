import serial
import csv

PuertoSerial = serial.Serial(port = '/dev/ttyS0')

f= open("valor.csv",'w') 
writer = csv.writer(f)

while(1):
    	valor = PuertoSerial.readline().decode().split(' ')
    	writer.writerow(valor)
	print(valor)

f.close()