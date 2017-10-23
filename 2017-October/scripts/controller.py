#! /usr/bin/env python
# -*- coding: utf-8 -*-
#
# This file presents an interface for interacting with the Playstation 4 Controller
# in Python. Simply plug your PS4 controller into your computer using USB and run this
# script!
#
# NOTE: I assume in this script that the only joystick plugged in is the PS4 controller.
#       if this is not the case, you will need to change the class accordingly.
#
# Copyright © 2015 Clay L. McLeod <clay.l.mcleod@gmail.com>
#
# Distributed under terms of the MIT license.

import os
import pprint
import pygame
import serial
import math

from time import sleep, time

def clear():
    os.system('cls' if os.name=='nt' else 'clear')

def response(a,y):
    return 1.0 / a * math.log((1 - y)/(1 + y))

class PS4Controller(object):
    """Class representing the PS4 controller. Pretty straightforward functionality."""

    controller = None
    key_data = None
    axis_data = None
    button_data = None
    hat_data = None
    ser = None
    def init(self):
        """Initialize the joystick components"""

        pygame.init()
        pygame.display.set_mode((640, 480))
        pygame.display.set_caption("Gears 4 Geeks")
        pygame.joystick.init()
        self.controller = pygame.joystick.Joystick(0)
        self.controller.init()
        self.ser = serial.Serial('COM4', 9600)

    def listen(self):
        """Listen for events to happen"""

        if not self.key_data:
            self.key_data = {}
            for i in range(1024):
                self.key_data[i] = False

        if not self.axis_data:
            self.axis_data = {}
            for i in range(self.controller.get_numaxes()):
                self.axis_data[i] = 0.0

        if not self.button_data:
            self.button_data = {}
            for i in range(self.controller.get_numbuttons()):
                self.button_data[i] = False

        if not self.hat_data:
            self.hat_data = {}
            for i in range(self.controller.get_numhats()):
                self.hat_data[i] = (0, 0)

        debug_toggle = True
        print_state_toggle = True

        # These parameters define how frequesnt speed setting sent over serial to arduino
        speed_threshold = 10.0 # sets update threshold
        speed_step = 1 # sets acceleration
        speed_delay = 0.01 # delay per 1 step in sec

        mode_switch = "k" # control mode: k - keyboard, j - joystick

        # Parameters for keyboard control mode
        speed = 0.0
        speed_current = 0
        direction = "r" # r - release, f - forward, b - backward
        direction_current = "r"

        # Parameters for joystick control mode
        speed_l = 0
        speed_r = 0
        prev_speed_l = 0
        prev_speed_r = 0
        prev_btn = False

        while True:
            prev = self.axis_data
            for event in pygame.event.get():
                if event.type == pygame.KEYDOWN:
                    self.key_data[event.key] = True
                elif event.type == pygame.KEYUP:
                    self.key_data[event.key] = False
                if event.type == pygame.JOYAXISMOTION:
                    self.axis_data[event.axis] = round(event.value,2)
                elif event.type == pygame.JOYBUTTONDOWN:
                    self.button_data[event.button] = True
                elif event.type == pygame.JOYBUTTONUP:
                    self.button_data[event.button] = False
                elif event.type == pygame.JOYHATMOTION:
                    self.hat_data[event.hat] = event.value

            # check for exit command
            if self.button_data[9] or self.key_data[pygame.QUIT] or self.key_data[pygame.K_ESCAPE]:
                pygame.quit()
                break

            # toggle debug
            if self.key_data[pygame.K_d]:
                if debug_toggle:
                    print("Toggle debug")
                    self.ser.write(b'd')
                    debug_toggle = False
            else:
                debug_toggle = True

            # print out motors status
            if self.key_data[pygame.K_p]:
                if print_state_toggle:
                    self.ser.write(b'p')
                    if self.ser.in_waiting:
                        print (self.ser.readline())
                    print_state_toggle = False
            else:
                print_state_toggle = True

            if self.key_data[pygame.K_1] and mode_switch != "k":
                mode_switch = "k"

            if self.key_data[pygame.K_2] and mode_switch != "j":
                print("Joystick mode: ON")
                mode_switch = "j"

            if mode_switch == "k": # keyboard control mode
                # accelearte forward
                if self.key_data[pygame.K_a] and direction != "r":
                    if speed < 255.0:
                        speed = speed + speed_step
                        sleep(speed_delay)
                # accelerate backward
                if self.key_data[pygame.K_z] and direction != "r":
                    if speed > 0.0:
                        speed = speed - speed_step
                        sleep(speed_delay)

                if self.key_data[pygame.K_UP] and direction != "f":
                    direction = "f"
                if self.key_data[pygame.K_DOWN] and direction != "b":
                    direction = "b"
                if self.key_data[pygame.K_UP] == False and direction == "f":
                    direction = "r"
                if self.key_data[pygame.K_DOWN] == False and direction == "b":
                    direction = "r"

                if math.fabs(speed - speed_current) > speed_threshold or direction != direction_current:
                    # print("{0}, {1}, {2}, {3}".format(speed, speed_current, direction, direction_current))
                    direction_current = direction
                    if direction == "r":
                        speed = 0.0
                    speed_current = int(speed)
                    str_r = "sr" + direction_current + str(speed_current) + "e"
                    str_l = "sl" + direction_current + str(speed_current) + "e"
                    print(str_l)
                    print(str_r)
                    self.ser.write(str_r.encode())
                    self.ser.write(str_l.encode())

                if(self.key_data[pygame.K_LEFT]):
                    str_rf = "srf" + str(speed_current) + "e"
                    self.ser.write(str_rf.encode())
                    str_lf = "slf" + str(int(speed_current*0.9)) + "e"
                    self.ser.write(str_lf.encode())
                elif(self.key_data[pygame.K_RIGHT]):
                    str_rb = "srf" + str(int(speed_current*0.9)) + "e"
                    self.ser.write(str_rb.encode())
                    str_lb = "slf" + str(speed_current) + "e"
                    self.ser.write(str_lb.encode())

                if (self.key_data[pygame.K_UP] == False and self.key_data[pygame.K_DOWN] == False) and (self.key_data[pygame.K_a] == False and self.key_data[pygame.K_z] == False):
                    speed = 0
                    speed_current = speed
                    direction = "r"
                    direction_current = direction
                    self.ser.write(b'srze')
                    self.ser.write(b'slze')

            if mode_switch == "j": # joystick control mode
                if self.ser.in_waiting:
                    print (self.ser.readline())

                prev_speed_l = speed_l
                prev_speed_r = speed_r
                speed_threshold = 1

                #simplified linear mapping for controller
                speed_l = int(math.fabs(self.axis_data[1]*255))
                speed_r = int(math.fabs(self.axis_data[3]*255))

                #print("curr_l: {0}, perv_l: {1}, curr_r:{2}, perv_r:{3}".format(speed_l, prev_speed_l, speed_r,prev_speed_r))

                if self.axis_data[1] < -0.03 and math.fabs(speed_l - prev_speed_l) > speed_threshold:
                    str_lf = "slf" + str(speed_l) + "e"
                    self.ser.write(str_lf.encode())
                elif self.axis_data[1] > 0.03 and math.fabs(speed_l - prev_speed_l) > speed_threshold:
                    str_lb = "slb" + str(speed_l) + "e"
                    self.ser.write(str_lb.encode())


                if self.axis_data[3] < -0.03 and math.fabs(speed_r - prev_speed_r) > speed_threshold:
                    str_rf = "srf" + str(speed_r) + "e"
                    self.ser.write(str_rf.encode())
                elif self.axis_data[3] > 0.03 and math.fabs(speed_r - prev_speed_r) > speed_threshold:
                    str_rb = "srb" + str(speed_r) + "e"
                    self.ser.write(str_rb.encode())

                if ( self.axis_data[1] >= -0.03 and self.axis_data[1] <= 0.03 ) and ( self.axis_data[3] >= -0.03 and self.axis_data[3] <= 0.03 ):
                    speed_l = 0
                    speed_r = 0
                    self.ser.write(b'srze')
                    self.ser.write(b'slze')

                #Logic to call RFID scan only once per click of R1 button
                if(prev_btn != self.button_data[5]):
                    print("Scanning for RFID Card")
                    prev_btn = self.button_data[5]
                    if self.button_data[5] :
                        self.ser.write("i".encode())

                # clear()
                # pprint.pprint(self.button_data)
                # pprint.pprint(self.axis_data)
                # pprint.pprint(self.hat_data)

if __name__ == "__main__":
    ps4 = PS4Controller()
    ps4.init()
    ps4.listen()
