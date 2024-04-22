#Variables 
distance #distance sensed by the sensor
sensor2 #(Data at the tunnel, 1=Full, 0=Not Full)send
distanceA #(Data of distance)
distanceB #(Data of distance)
distanceC #(Data of distance)

#Functions
send() #function to send data from this computer

ave_distance() #calculate average distance from three datas
    return (distanceA + distanceB + distanceC)/3

sensor() #functions of the sonic sensor

Computer() #general workings and calculations
    #three data from 3s
    sensor()
    distanceA = distance
    wait 1s
    sensor()
    distanceB = distance
    wait 1s
    sensor()
    distanceC = distance

    ave_distance()

    if ave_distance() > 61 #cm
        sensor2 == 0
    
    else
        sensor2 == 1

#Main
Computer()
send(sensor2)