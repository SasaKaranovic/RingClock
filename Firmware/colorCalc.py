#Rainbow Challenge - www.101computing.net/rainbow-challenge
import turtle
import numpy as np

myPen = turtle.Turtle()
myPen.shape("turtle")
myPen.speed(900)

window = turtle.Screen()
window.bgcolor("#69C5FF")

# rainbowColors = ["#FF0000","#FFA600","#FFFF00", "#62FF00", "#1E56FC","#4800FF","#CC00FF","#69C5FF"]
rainbowColors = [ ]

for count in range(0,64):
    red   = np.around(np.sin(0.1*count + 0) * 127 + 128, 0)
    green = np.around(np.sin(0.1*count + 2) * 127 + 128, 0)
    blue  = np.around(np.sin(0.1*count + 4) * 127 + 128, 0)
    rgb = "#{:02X}{:02X}{:02X}".format(int(red), int(green), int(blue))
    # print(rgb)
    rainbowColors.append(rgb)

# exit()
print(rainbowColors)
size=600

myPen.penup()
myPen.goto(0,-600)

for color in rainbowColors:
  myPen.color(color)
  myPen.fillcolor(color)
  myPen.begin_fill()
  myPen.circle(size)
  myPen.end_fill()
  size -= 5
  

turtle.mainloop()