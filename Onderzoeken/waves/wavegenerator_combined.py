import numpy as np
import random

fArray = open("output_array.txt", 'w')
fCSV = open("output_csv.txt", 'w')
fSSV = open("output_ssv.txt", 'w')

r = 0.5
tensionMatrix = np.matrix([[0,1,0,0],[-r,0,r,0],[2*r,r-3,3-2*r,-r],[-r,2-r,r-2,r]])
waves = [
    [12,"Breaking wave antidunes",[[0,4,0],[3,4.5,0],[8,3,0]]],
    [13,"Standig wave antidunes",[[0,4,0],[4,5.5,0],[7,4,0],[10,3,0]]],
    [10,"Washed-out dunes", [[0,4,0],[2,4.5,0],[5,3,0]]]
    ]

    #[9,"Antidunes",[[0,4,0],[3,2.5,0],[6,5.5,0]]],

waveArray = []
curX = 0
random.seed(123456789)

for x in range(0, 119):
    randomWave      = random.randint(0,len(waves) - 1)
    waveSize        = waves[randomWave][0]
    waveName        = waves[randomWave][1]
    waveFootprint   = waves[randomWave][2]

    print("Generated wave: " + waveName + " from " + str(curX) + " to: " + str(curX + waveSize - 1))
    
    for y in range(0, len(waveFootprint) - 1):
        waveArray.append(waveFootprint[y].copy())
        waveArray[len(waveArray) - 1][0] = waveArray[y][0] + curX
    curX += waveSize -1
    
waveArray.insert(0,waveArray[0])
waveArray.append(waveArray[len(waveArray) - 1].copy())

for x in range(0, 119):
    for y in range(0, 10):
        pr = (y / 10)
        progressMatrix = np.matrix([[1,pr,pr * pr,pr*pr*pr]])
        pointsMatrix = np.matrix([waveArray[x],waveArray[x+1],waveArray[x+2],waveArray[x+3]])
        output = progressMatrix*tensionMatrix*pointsMatrix
        fSSV.write(str(output.item((0,0))) + " " + str(output.item((0,1))) + " 0\n")
        fArray.write("{" + str(output.item((0,0))) + "," + str(output.item((0,1))) + ",0},\n")
        fCSV.write(str(output.item((0,0))) + "," + str(output.item((0,1))) + "," + str(output.item((0,2))) + "\n")
fArray.close()
fCSV.close()

































#waveArray = [[0,4,0],[3,4.5,0],[8,3,0]] #Breaking wave antidunes       12
#waveArray = [[0,4,0],[3,2.5,0],[6,5.5,0]] #Anitdunes                     9
#waveArray = [[0,4,0],[4,5.5,0],[7,4,0],[10,3,0]] #Standig wave antidunes13
#waveArray = [[0,5,0],[5,5,0]]           #plane bed                     10
#waveArray = [[0,4,0],[2,4.5,0],[5,3,0]] #Washed-out dunes              10
#waveArray = [[0,4,0],[3,5,0],[6,1.5,0]] #Dunes                         9
#waveArray = [[0,4,0],[1,5,0],[2,4.5,0],[4,6.5,0]] #Dunes, with superimposed ripples 6
#waveArray = [[0,4,0],[2,3,0]]           #Ripples                       4
#waveArraySize = len(waveArray)
#waveSize = 13
