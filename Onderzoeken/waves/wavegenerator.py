import numpy as np

f_instance = open("wave_anitdunes.txt", 'w')

r = 0.5
tensionMatrix = np.matrix([[0,1,0,0],[-r,0,r,0],[2*r,r-3,3-2*r,-r],[-r,2-r,r-2,r]])
waveArray = [[0,4,0],[3,4.5,0],[8,3,0]] #Breaking wave antidunes       12
#waveArray = [[0,3.5,0],[3,2,0],[6,5,0]] #Anitdunes                     9
#waveArray = [[0,4,0],[4,5.5,0],[7,4,0],[10,3,0]] #Standig wave antidunes13
#waveArray = [[0,5,0],[5,5,0]]           #plane bed                     10
#waveArray = [[0,4,0],[2,4.5,0],[5,3,0]] #Washed-out dunes              10
#waveArray = [[0,5,0],[3,6,0],[6,2.5,0]] #Dunes                         9
#waveArray = [[0,3,0],[1,4,0],[2,3.5,0],[4,5.5,0]] #Dunes, with superimposed ripples 6
#waveArray = [[0,4,0],[2,3,0]]           #Ripples                       4
waveArraySize = len(waveArray)
waveSize = 12

for x in range(0, 119):
    for y in range(0, waveArraySize):
        waveArray.append(waveArray[y].copy())
        waveArray[(x + 1) * waveArraySize + y][0] = waveArray[y][0] + ((x + 1) * waveSize)
    
waveArray.insert(0,waveArray[0])
waveArray.append(waveArray[len(waveArray) - 1].copy())

for x in range(0, 100):
    for y in range(0, 10):
        pr = (y / 10)
        progressMatrix = np.matrix([[1,pr,pr * pr,pr*pr*pr]])
        pointsMatrix = np.matrix([waveArray[x],waveArray[x+1],waveArray[x+2],waveArray[x+3]])
        output = progressMatrix*tensionMatrix*pointsMatrix
        f_instance.write("{" + str(output.item((0,0))) + "," + str(output.item((0,1))) + ",0},\n")
        #f_instance.write("{" + str(output.item((0,0))) + "," + str(output.item((0,1))) + "},\n")
        #f_instance.write(str(output.item((0,0))) + "," + str(output.item((0,1))) + "," + str(output.item((0,2))) + "\n")
f_instance.close()
