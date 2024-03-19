import u3

# Open the first found LabJack U3
device = u3.U3()

# Configure the FIO ports for SPI
device.configIO(FIOAnalog=0)

# Set the SPI parameters
# Clock high between communication, data valid on trailing edge, chip select active low
clockIdleState = 1
dataOnTrailing = 1
chipSelectActiveLow = 1

# Write and read data
dataToSend = [0x01, 0x02, 0x03]  # Example data to send
numBytesToRead = 3  # Number of bytes to read back

# Perform the SPI transaction
result = device.spi(dataToSend, AutoCS=True, DisableDirConfig=True, SPIMode=(clockIdleState, dataOnTrailing, chipSelectActiveLow), NumSPIBytesToTransfer=numBytesToRead)

print("Sent:", dataToSend)
print("Received:", result['SPIBytes'])
