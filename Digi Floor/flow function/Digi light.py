#Variables
light #(Traffic Light Status, if it is still performing its sequence function, 1=performing, 0=not)send
light_color #(What side did it allow to cross, 1=traffic going up, -1=traffic going down, 2=No command)send
order #(Data sent by the Main Computer)


#(1=Go, 0=Stop, 2=Yellow)
#1- Light POV for the traffic from up stairs
l1 

#2- Light POV for the traffic from down stairs
l2 

#3- Light POV for the traffic from 1st Floor
l3_1 #for traffic wanting to go down stairs
l3_2 #for traffic wanting to go Up or to Tube

#4- Light POV for the treaffic from tube 
l4_1 #for traffic wanting to go 1st floor or down stairs
l4_2 #for traffic wanting to go up stairs

#Functions
display() #to command the RGB lights the colors to show

retrieve() #retrieve values from other components

send() #function to send data from this computer

check_manual() #function to check if manual mode is on
    if remote_status == 1:
        manual_pilot()
        break

auto_pilot() #function for autop pilot mode sequence
    if order == -1
        light = 1
        light_color = -1
        send(light, light_color)
        l1 = 1
        l2 = 0
        l3_1 = 1
        l3_2 = 0
        l4_1 = 1
        l4_2 = 0
        display()
        wait 10s
        check_manual()
        light_color = 1
        send(light_color)
        l1 = 0
        l2 = 1
        l3_1 = 0
        l3_2 = 1
        l4_1 = 0
        l4_2 = 1
        display()
        wait 5s
        check_manual()
        light = 0
        send(light)

    if order == -2
        light = 1
        light_color = -1
        send(light, light_color)
        l1 = 1
        l2 = 0
        l3_1 = 1
        l3_2 = 0
        l4_1 = 1
        l4_2 = 0
        display()
        wait 15s
        check_manual()
        light_color = 1
        send(light_color)
        l1 = 0
        l2 = 1
        l3_1 = 0
        l3_2 = 1
        l4_1 = 0
        l4_2 = 1
        display()
        wait 5s
        check_manual()
        light = 0
        send(light)        

    if order == 1
        light = 1
        light_color = -1
        send(light, light_color)
        l1 = 1
        l2 = 0
        l3_1 = 1
        l3_2 = 0
        l4_1 = 1
        l4_2 = 0
        display()
        wait 5s
        check_manual()
        light_color = 1
        send(light_color)
        l1 = 0
        l2 = 1
        l3_1 = 0
        l3_2 = 1
        l4_1 = 0
        l4_2 = 1
        display()
        wait 10s
        check_manual()
        light = 0
        send(light)        

    if order == 2
        light = 1
        light_color = -1
        send(light, light_color)
        l1 = 1
        l2 = 0
        l3_1 = 1
        l3_2 = 0
        l4_1 = 1
        l4_2 = 0
        display()
        wait 5s
        check_manual()
        light_color = 1
        send(light_color)
        l1 = 0
        l2 = 1
        l3_1 = 0
        l3_2 = 1
        l4_1 = 0
        l4_2 = 1
        display()
        wait 15s
        check_manual()
        light = 0
        send(light)

manual_pilot() #function for manual operationsd
    if remote_order == 1
        light_color = 1
        send(light_color)
        l1 = 0
        l2 = 1
        l3_1 = 0
        l3_2 = 1
        l4_1 = 0
        l4_2 = 1
        display()
    
    if remote_order == -1
        light_color = -1
        send(light_color)
        l1 = 1
        l2 = 0
        l3_1 = 1
        l3_2 = 0
        l4_1 = 1
        l4_2 = 0
        display()
    
    else
        light_color = 2
        send(light_color)
        l1 = 2
        l2 = 2
        l3_1 = 2
        l3_2 = 2
        l4_1 = 2
        l4_2 = 2
        display()


#Main
retrieve(remote_status, order, remote_order)

if remote_status == 1
    manual_pilot()
if remote_status == 0
    auto_pilot()