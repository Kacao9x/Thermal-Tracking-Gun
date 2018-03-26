# Thermal-Tracking-Gun
This awesome project got done within 36 hours in the #Hackathon spring 2018 at ISU.

# Built With
Arduino, C, 8x8 infrared camera, 2 x servos, a power-supply

# Inspiration

# What it does

# How we built it
MECHANICAL: We used 2 pairs of wooden poles to make the gun rack. These poles were glued together and mounted on an 8-inch spinning disk which is controlled by a servo. When spinning, the disk will make the gun barrel swing horizontally. There was another motor with a supporting arm that connected to the barrel. The gun could swing vertically easily when the arm pushed/pulled.

ELECTRICAL: There is a few notice here because we use a rotating nerf gun. When we pull the trigger, the gun barrel will rotate for 0.5s before it actually fire. What appears to be the mechanism of the shooting mode is there are 2 separate motors inside the gun, one for rotating barrel and one for Shot. To control these motors, we decided to use 2 relays. Every connection from the trigger to motor had been disconnected before hand.

SOFTWARE: 
Harlin, an aerospace engineer came up with the idea of searching mode when no object is detected. We programmed a preset pattern for the gun to swing. Once the target is found, the gun fires.

The sensor is an 8x8 array of IR thermal sensors. When connected to your microcontroller (Arduino) it will return an array of 64 individual infrared temperature readings over I2C. To make it easier to use, Interrupt Service is used to detect human body in front of sensor. Any reading on any pixel above TEMP_INT_HIGH (30 degree) will trigger the interrupt. We can tell which pixels triggered the interrupt by reading the bits in this array of bytes. Any bit that is a 1 means that pixel triggered

     		bit 0  bit 1  bit 2  bit 3  bit 4  bit 5  bit 6  bit 7
byte 0 |  0      1      0      0      0      0      0      1
byte 1 |  0      0      0      0      0      0      0      0
byte 2 |  0      0      0      0      0      0      0      0
byte 3 |  0      0      0      1      0      0      0      0
byte 4 |  0      0      0      0      0      0      0      0
byte 5 |  0      0      0      0      0      0      0      0
byte 6 |  0      0      0      0      0      1      0      0
byte 7 |  0      0      0      0      0      0      0      0

Next, we have to determine the direction for the gun to swing. We develop a bit-counting algorithm to compare the sum of each quarter. The servo will swing the gun the the area which has higher sum. The gun will FIRE if any pair of central pixels are 1 (e.g [3][3], [4][3], [3][4], [4][4]).

# Challenges we ran into
An issue we ran into was a crack in the supporting arm because the motor turned fast. Also, the chasing target mode acted very slowly. The reason is probably our algorithm to handle Interrupt service was not implemented efficiently

# Accomplishments that we're proud of
We are most proud of the simplicity in the mechanics of our design. Simple, robust and very effective. 
Personally, this marked the first time I joined a Hackathon and built things. 

# What we learned
Cross learning. Each team member has different engineering expertise. 

# What's next for the Heat-tracking Nerf Gun
- Wiring --> PCB
- Firmware: Upgrade the algorithm to track an object out of target. Optimize the preset pattern (in IDLE) for the gun to scan objects.
- Design 3D-customized motor arms that better support the gun to swing at higher speed
