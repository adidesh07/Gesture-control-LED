from hand_tracker import handDetector
import cv2
import time
import serial


"""
Detect tip of index finger, tip of thumb, calculate real-time distance between them.
Send toggle signal every time distance crosses a threshold value.

Landmark indices:
Tip of thumb = 4
Tip of index finger = 8
"""


curTime = 0
prevTime = 0
capture = cv2.VideoCapture(0)
detector = handDetector()

boardComm = serial.Serial(
    'COM9', # Active port connected to USB for serial communication with controller
    115200, # Baud rate for contrller
)

if not boardComm.is_open:
    boardComm.open()

# Check flag to ensure toggle operations occurs only once for each condition
check = 0

while True:
    success, image = capture.read()
    image = detector.findHands(image)

    # List of landmark indices, their x, y positions
    # [ind, xpos, ypos]
    landmarkList = detector.findPos(image)
    
    if len(landmarkList) > 0:
        xThumb, yThumb = landmarkList[4][1::]
        xIndex, yIndex = landmarkList[8][1::]

        # Calculate distance from points
        xLen = abs(xThumb - xIndex)
        yLen = abs(yThumb - yIndex)
        distance = (xLen ** 2 + yLen ** 2) ** 0.5
        
        if distance < 50 and check:
            check = 0
            boardComm.write(b"Toggle")
        elif distance >= 50 and not check:
            check = 1
            boardComm.write(b"Toggle")

    curTime = time.time()
    fps = 1 / (curTime - prevTime)
    prevTime = curTime

    cv2.putText(image, str(int(fps)), (10, 30), cv2.FONT_HERSHEY_PLAIN,
                2, (255, 0, 0), 2)
    cv2.imshow('Motion Detection', image)
    cv2.waitKey(1)

