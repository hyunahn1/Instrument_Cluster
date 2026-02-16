from piracer.gamepads import ShanWanGamepad

def main(): 
    shanwan_gamepad = ShanWanGamepad()

    while True:
        gamepad_input = shanwan_gamepad.read_data()

        l1 = gamepad_input.button_l1
        r1 = gamepad_input.button_r1
        r2 = gamepad_input.button_r2
        l2 = gamepad_input.button_l2
        a = gamepad_input.button_a
        b = gamepad_input.button_b
        y = gamepad_input.button_y
        x = gamepad_input.button_x
        start = gamepad_input.button_start
        select = gamepad_input.button_select
        home = gamepad_input.button_home
        analog_stick_left = gamepad_input.analog_stick_left.z
        analog_stick_right = gamepad_input.analog_stick_right.z

        print("L2: {0}, R2: {1}".format(l2,r2))
        print("L1: {0}, R1: {1}".format(l1,r1))
        print("A: {0}, B: {1}, X: {2}, Y: {3}".format(a,b,x,y))
        print("Start: {0}, Select: {1}, Home: {2}".format(start,select,home))
        print("Left Analog Stick: {0}, Right Analog Stick: {1}".format(analog_stick_left, analog_stick_right))

if __name__ == '__main__':
    main()