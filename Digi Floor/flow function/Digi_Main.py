#Variables
time #(Data sent by the clock component, 1=up, -1=down)retrieve
sensor1 #(Data sent by the sonic sensors from 2nd floor, -1=Full, 0=Not Full)retrieve
sensor2 #(Data sent by the sonic sensors from math block, 1=Full, 0=Not Full)retrieve
light #(Traffic Light Status, if it is still performing its sequence function, 1=performing, 0=not)retrieve
remote_status #(if the operator placed it on Manual Mode, 1=On, 0=Off)retrieve
remote_order #(Orders Given by the operator, 1=Up, -1=Down)retrieve
command #(Orders to be sent to the light components)send
status #(Floor Status)send


#Functions
retrieve() #retrieve values from other components

send() #function to send data from this computer

priority_count() #calculator on giving results on to who it 
    return sensor1 + sensor2 + time

auto_pilot()
    if light == 0
        priority_count(sensor1, sensor2, time)
        
        if priority_count() == 0
            order = time
            send(order)

        else 
            order = priority_count()
            send(order)

manual_pilot()
    send(remote_status, remote_order) 

#Main
retrieve(time, sensor1, sensor2, remote_status)

if remote_status == 1
    manual_pilot()

if remote_status == 0
    auto_pilot()