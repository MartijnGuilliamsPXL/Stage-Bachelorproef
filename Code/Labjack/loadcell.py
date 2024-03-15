import u3  # Assuming you have the LabJack Python library installed

# Establish connection to the LabJack U3 device
try:
    lj = u3.U3()
except Exception as e:
    print("Error connecting to LabJack U3:", e)
    exit()

# Define scaling factor (determined by your HX711 setup)
scaling_factor = 2400  # Replace with the appropriate value for your system

# Continuous reading loop
while True:
    try:
        # Read raw ADC value using getFeedback method
        feedback = lj.getFeedback(LJ_ioGET_AIN, 0, 0)
        adc_value = feedback[0]  # Access the first element (analog input value)

        # Apply calibration and scaling (replace with your specific formula)
        offset = 5
        weight = (adc_value - offset) / scaling_factor  # Adjust for offset if necessary

        # Print the measured weight
        print("Weight:", weight, "grams")

    except KeyboardInterrupt:
        print("Exiting...")
        break

# Close connection to the LabJack U3 device
lj.close()
