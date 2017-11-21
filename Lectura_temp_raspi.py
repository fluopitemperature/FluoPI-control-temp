import sys, time, datetime
import serial
ser = serial.Serial('/dev/ttyACM0', 19200)#Debes cambiar el nombre de /dev/cu....
while True:
    f = open("temperatura.txt",'a')
    ts = time.time()
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d;%H:%M:%S')
    valor0=ser.readline();
##    valor1=ser.read();
##    valor2=ser.read();
##    valor3=ser.read();
##    valor4=ser.read();
##    valor5=ser.read();
##    valor6=ser.read();
##
##    valor0=("".join(map(chr, valor0)))
##    valor1=("".join(map(chr, valor1)))
##    valor2=("".join(map(chr, valor2)))
##    valor3=("".join(map(chr, valor3)))
##    valor4=("".join(map(chr, valor4)))
	
#    f.write(str(valor0)+str(valor1)+str(valor2)+str(valor3)+str(valor4))
    f.write(st + ';'+ valor0)
#   f.write('\n')
    f.close()
