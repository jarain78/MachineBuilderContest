from math import sin, cos, pi
from pylx16a.lx16a import *
import time
import ikpy.chain
import ikpy.utils.plot as plot_utils
import yaml
import numpy as np
import time
import math
import cv2

import ipywidgets as widgets

# IKPy imports
from ikpy.chain import Chain

# Iniciar captura de video
#cap = cv2.VideoCapture(0)


def readAngle():

    angleServoLeft1 = servo_base.get_physical_angle()
    angleServoLeft2 = servo_shoulder.get_physical_angle()
    angleServoLeft3 = servo_elbow.get_physical_angle()
    angleServoLeft4 = servo_wrist.get_physical_angle()
    angleServoLeft5 = servo_wrist_r.get_physical_angle()
    #angleServoLeft6 = servo_grepper.get_physical_angle()                


    print([angleServoLeft1, angleServoLeft2, angleServoLeft3, angleServoLeft4, angleServoLeft5]            )


#-------------------------------------------------------------------
speed = 800
servo_left_1_zero_angle = 90
servo_left_2_zero_angle = 170
servo_left_3_zero_angle = 90
servo_left_4_zero_angle = 90
servo_left_5_zero_angle = 90
servo_left_6_zero_angle = 90


LX16A.initialize("/dev/ttyUSB0")



try:
    # Slave Arm
    servo_base = LX16A(7)
    servo_shoulder = LX16A(8)
    servo_elbow = LX16A(9)
    servo_wrist = LX16A(10)
    servo_wrist_r = LX16A(11)
    #servo_grepper = LX16A(12)

    servo_base.disable_torque()
    servo_shoulder.disable_torque()
    servo_elbow.disable_torque()
    servo_wrist.disable_torque()
    servo_wrist_r.disable_torque()
    #servo_grepper.disable_torque()
    time.sleep(2)

except ServoTimeoutError as e:
    print(f"Servo {e.id_} is not responding. Exiting...")
    quit()

#slaveServo1.enable_torque()
'''servo_base.move(90, time=speed)
servo_shoulder.move(45, time=speed)
servo_elbow.move(0, time=speed)
servo_wrist.move(20, time=speed)
servo_wrist_r.move(90, time=speed)
servo_grepper.move(11, time=speed)'''

time.sleep(2)

readAngle()

time.sleep(2)
servo_base.enable_torque()
servo_shoulder.enable_torque()
servo_elbow.enable_torque()
servo_wrist.enable_torque()
servo_wrist_r.enable_torque()
#servo_grepper.enable_torque()
time.sleep(2)


position = [[10.0, 180.64, 180.64, 89.76, 88.08],
           [81.6, 106.32, 113.52, 90.24, 104.88],
           [81.6, 62.16, 54.96, 90.0, 80.64],
           [90.96, 76.32, 81.12, 90.24, 95.28],
           [69.84, 61.92, 69.84, 90.48, 94.56],
           [78.0, 46.08, 52.32, 90.48, 94.56],
           [10.0, 180.64, 180.64, 89.76, 88.08]]

for i in position:
    servo_base.move(i[0], time=speed)
    servo_shoulder.move(i[1], time=speed)
    servo_elbow.move(i[2], time=speed)
    servo_wrist.move(i[3], time=speed)
    servo_wrist_r.move(i[4], time=speed)
    #servo_grepper.move(i[5], time=speed)
    time.sleep(3)
