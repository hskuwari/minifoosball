# Remote-controlled Foosball 
This is a project made for 15-348 Embedded Systems. We used a mini foosball; with only 2 rods per each player. Each rod is controlled by a rotational servo-motor (can only move rotationally, not linearly). Scores are added manually by pressing a button, and the scores show by the number of LED that are turned on. The player can control each rod by moving the joystick horizontally and they can also choose which rod to move by choosing button A (for the  offense) and button B (for the defense). 

### Hardware used
For the microcontroller, we used Tiva™ TM4C123GH6PM Microcontroller. We used 4 microcontrollers in total: 2 per each player (1 that is connected with the servomotors and 1 that connected with the joystick). For the joystick, we used Educational BoosterPack MKII. We also used 2 IR recievers and 2 IR LED transmitters for IR communication between 2 microcontrollers. We used 24 LEDs in totol, 12 per each side. Each side uses 10 LEDs to display results, and 2 LEDs to know which rod is being controlled at the moment.  

### Implementation
The joystick is run by joy_x_y.ino (using energia). For each joystick, I change the definition of PLAYER to either 0 or 1. 

board_side.c also has PLAYER and it is set accordingly. (PLAYER with yellow uniform is 1 and the other one is 0).

We used IR communication (IR reciever, IR LED transmitter), shift register for LEDs that were on the board, and a button that increments the score. The board had 12 LEDs (10 for score and 2 for the player to know which row is being controlled). The score is reset after reaching 10 points, and then if the button is pressed again, it resets. 

IR communication was done by NEC protocol. We used the same header as NEC protocol has and then we send 16 bits which goes like: 
PLAYER (1 bit) + NOT PLAYER (1 bit) +  BUTTON (2 bits) + NOT BUTTON (2 bits) + JOYSTICK X VALUE (5 bits) + NOT (JOYSTICK X VALUE) (5 bits)
 
BUTTON:
0 => Joystick
1 => Button 1
2 => Button 2 

JOYSTICK X VALUE: 
Ranges from 7 to 21 

board_side.c checks whether the given value is correct and the player that was sent is the same player. 
