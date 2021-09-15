import cv2
import mediapipe as mp

class handDetector():
    def __init__(self):
        self.mpHands = mp.solutions.hands
        self.hands = self.mpHands.Hands(max_num_hands = 1)
        self.mpDraw = mp.solutions.drawing_utils

    # Draw connecting lines indicating detection of hands
    def findHands(self, img, draw=True):
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        self.results = self.hands.process(imgRGB)

        if self.results.multi_hand_landmarks:
            for handLms in self.results.multi_hand_landmarks:
                if draw:
                   self.mpDraw.draw_landmarks(img, handLms, self.mpHands.HAND_CONNECTIONS)

        return img

    # Return index, x and y position of all landmark points (0 - 20) on hand
    def findPos(self, img, handNum=0, draw=True):
        lmList = []

        if self.results.multi_hand_landmarks:
            myHand = self.results.multi_hand_landmarks[handNum]

            for id, landmark in enumerate(myHand.landmark):
                height, width, channel = img.shape
                xpos, ypos  = int(landmark.x * width), int(landmark.y * height)
                lmList.append([id, xpos, ypos])

                if draw:
                    cv2.circle(img, (xpos, ypos), 5, (0,255,0), cv2.FILLED)

        return lmList

