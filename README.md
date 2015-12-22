# bingo-matrix

Play a game of Bingo on the Arduino and a RGB 16x32 led matrix panel.

Press the button and a random number between 1 and 75 will be drawn and displayed 
(and blinking for a few times) on the matrix. Numbers that were already drawn are colored red.
Non-drawn numbers are colored green. For an impression, see the bingo-matrix.png.
Duplicates are taken care of. To get a 'real' random number the Entropy library is used.
Press the 2nd button 5 times and the game will reset itself.

Tested on Arduino Uno.

16x32 matrix used: https://www.adafruit.com/products/420

Happy playing

Rino

