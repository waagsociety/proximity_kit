# Proximity Sensor Case Prototyping

**Proximity sensor, PCB + Components Dimensions**

Screw holes: 51.5mm x 38.75mm

H: 70mm

W: 59mm x 59mm

**Proximity Sensor Case, Key points for design**

-Allow NFC to function

-Design for manufacture with Fablab machines

-Ease of assembly

-Access to all buttons and USB port

-Access to battery

-B,C and Tag LEDs all visible

## Research and Preparation

I chose 4mm (3.85mm nominal thickness) poplar triplex as a material for the proximity sensor as I think it well suited to the intended application of the proximity sensor; they will be indoors and there will be many of them. The 4mm poplar triplex is a good value for its level of durability.

I did tests to find the right sizing and settings for making press fit components on our lasercutter. After some time I descided that Speed 100 and Power 100 gave a good balance between keeping corners sharp and minimizing burning. It also has the added benefit of cutting quickly.

Next I had to find out the kerf of my lasercutter with this material at these settings. Cutting the slots of my press fit joints to 3.7mm gave a good fit with the 4mm triplex I have decided to use for the proximity sensor case.

Here's a guide on how to figure out the kerf of your lasercutter:
http://fab.academany.org/2018/labs/fablabuae/laser.html

All of the designs here were made with the .15 (each side) kerf of my lasercutter, you will have to adjust the sizing of the joints for the characteristics of your lasercutter.

## Designing & Making

### Designing

<p align="center"><img src="images/Sketch.jpg" width="600"></p>

If I make the box with press-fit finger joints on all sides, then the enclosures can be stacked in the x and y axis, providing opportunity for expansions to the proximity sensor in the future.

For this project, I think its the most time effective to use an online lasercut box generator.

A quick google search gave me some online options for designing lasercut joints. I liked "makercase.com" because of its  simplicity and visual preview.

<p align="center"><img src="images/Makercase.png" width="600"></p>

I used makercase to quickly design the joints of the box. The website allows you to enter the kerf of your lasecutter and adjusts the files it outputs to press fit together nicely.

I then edited the parts in illustrator to make the width of the joints the same along the whole length of all sides of the case.(makercase.com makes the first and last joints larger, I don't want this)  This will allow the boxes to be stacked together as per my design. 

I cut out small sections of each part to check the fit without wasting much material: 

<p align="center"><img src="images/Tests.jpg" width="600"></p>

After I had the basic box shape, I measured and mapped out the required holes for the buttons and LEDs to be usable/ visable, again testing as I went.

Here is the plans for the final design of my case for the MUV proximity sensor:

<p align="center"><img src="images/Plans.png" width="600"></p>

Each part of the design can be cut halfway and the joint pattern can be extended to make any height or width box.

### Materials Needed

4mm triplex poplar (300mm x 320mm)

Small pieces of frosted acrylic to act as diffusers for the LEDs ( can be cut from scrap)

Small piece of double sided tape

2x m2.6 x 20 screws and nuts

4x m2.6 x 7.5 screws and nuts

Sandpaper, CA glue and rubber hammer (if needed to help with assembly)

### Making/ Instructions

Step 0:

Collect your parts:
<p align="center"><img src="images/Step 0.jpg" width="600"></p>

Step 1:

Add a piece of double sided tape to the back of the NFC sensor:
<p align="center"><img src="images/Step 1.jpg" width="600"></p>

Step 2: 

Stick battery to the back of NFC sensor as low down the sensor ass possible:
<p align="center"><img src="images/Step 2.jpg" width="600"></p>

Step 3: 

Screw the PCB to the the floor plate with the NFC sensor facing the arrow: 
<p align="center"><img src="images/Step 3.jpg" width="600"></p>

Step 4:

Insert Acrylic piece into the front plate:
<p align="center"><img src="images/Step 4.jpg" width="600"></p>

Step 5:

Add a piece of double sided tape to the top of the front of the NFC sensor
<p align="center"><img src="images/Step 5.jpg" width="600"></p>

Step 6:

Attach the front plate with the NFC symbol and stick the NFC sensor to the front plate to keep it close against the front plate:
<p align="center"><img src="images/Step 6.jpg" width="600"></p>

Step 7:

Attach the side plate with the holes for the USB and ON/OFF switches:
<p align="center"><img src="images/Step 7.jpg" width="600"></p>

Step 8:

Press fit the ON/OFF switch into its hole:
<p align="center"><img src="images/Step 8.jpg" width="600"></p>

Step 9: 

Attach the side plate without any holes halfway:
<p align="center"><img src="images/Step 9.jpg" width="600"></p>

Step 10:

Insert the top plate with the hole for the lid and fully attach the side plate from the previous step:
<p align="center"><img src="images/Step 10.jpg" width="600"></p>

Step 11:

Insert the 2 frosted acrylic pieces for the reset and config LEDs:
<p align="center"><img src="images/Step 11.jpg" width="600"></p>

Step 12:

Attach the back plate halfway:
<p align="center"><img src="images/Step 12.jpg" width="600"></p>

Step 13:

Press fit the reset and config switches into their holes:
<p align="center"><img src="images/Step 13.jpg" width="600"></p>

Step 14: 

They should fit like this:
<p align="center"><img src="images/Step 14.jpg" width="600"></p>

Step 15: 

Attach the speaker with the 2 m3 x 20 screws and the 2 nuts:
<p align="center"><img src="images/Step 15.jpg" width="600"></p>

Step 16:

Gather the 3 lid parts:
<p align="center"><img src="images/Step 16.jpg" width="600"></p>

Step 17:

Join the middle and top pieces of the lid:
<p align="center"><img src="images/Step 17.jpg" width="600"></p>

Step 18:

Attach the bottom of the lid:
<p align="center"><img src="images/Step 18.jpg" width="600"></p>

Step 19:

Insert the whole lid into the top plate of the box and turn to close:
<p align="center"><img src="images/Step 19.jpg" width="600"></p>

Step 20:

Attach the bottom cover like you did the other panels, with one side first as in the photo and then the other 3:
<p align="center"><img src="images/Step 20.jpg" width="600"></p>

Step 21:

Sit back and admire your finished MUV Proximity Sensor case!
<p align="center"><img src="images/Step 21.jpg" width="600"></p>



















