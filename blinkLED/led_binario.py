import RPi.GPIO as GPIO
import time

def AddZero(s):
    if (len(s) == 1):
        s = "000" + s
    elif (len(s) == 2):
        s = "00" + s
    elif len(s) == 3:
        s = "0" + s
    return s

GPIO.setmode(GPIO.BCM)
GPIO.setup(18, GPIO.OUT)
GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.OUT)
GPIO.setup(25, GPIO.OUT)

dic = {0: 18, 1: 23, 2: 24, 3: 25}
aux = 0
apaga = []

# Testando o funcionamento dos leds
GPIO.output(dic[0], GPIO.HIGH)
GPIO.output(dic[1], GPIO.HIGH)
GPIO.output(dic[2], GPIO.HIGH)
GPIO.output(dic[3], GPIO.HIGH)
time.sleep(2)
GPIO.output(dic[0], GPIO.LOW)
GPIO.output(dic[1], GPIO.LOW)
GPIO.output(dic[2], GPIO.LOW)
GPIO.output(dic[3], GPIO.LOW)

# Valores de 1 a 15 em binário
for i in range(16):
    num = bin(i).split('b')
    b = AddZero(num[1])
    for j in b:
        if j == '1':
            GPIO.output(dic[aux], GPIO.HIGH)
            apaga.append(dic[aux])
        else:
            GPIO.output(dic[aux], GPIO.LOW)
        aux += 1
    
    time.sleep(2)
    for k in apaga:
        GPIO.output(k, GPIO.LOW)
    apaga = []
    aux = 0
