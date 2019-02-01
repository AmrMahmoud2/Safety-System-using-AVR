# Safety-System-using-AVR
Safety System using AVR micro-controller.

The project is divided into four phases.

In the first phase, we are supposed to set a password 'A' which enters the whole system, the password is entered using keypad, if the password is right, a LED will blink 4 times, if the password is wrong, the LED will blink 1 time, and if we enter a wrong password  for 5 times, the keypad will not accept any password entered for 30 seconds.

In the second phase, we are supposed set password 'B' which enters the first application in the system, in this application a temperature sensor will measure the temperature and show it on two 7 segments. If the temperature is higher than a certain level, a LED will turn on. When you exit this application, the last temperature that the sensor measures is saved and
the LED and the two 7 segments are turned off. If you press the '*' in the keypad, you will exit this application and the system will wait for a new password.

In the third phase, instead of using a LED to detect whether the password is wrong or right, we use LCD, the password is shown on the LCD if it is right the LCD displays "Right Password" and if it is wrong the LCD displays "Wrong Password". If the entered password is wrong 5 times, the counter which counts down the 30 seconds will appear on the LCD.

In the fourth phase, we keep the third phase working and we set password 'C' that enters another application. We also set password 'D' which enters the last application in which we change all the passwords in the system -password A, B, C, and D- and test them. 

For the demo video of the project, check: https://drive.google.com/open?id=1SSZ_uYegoZStebx3IqDYuFtHpOdgU470
