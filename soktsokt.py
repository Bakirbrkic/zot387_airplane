import websocket

# Connect to WebSocket server
ws = websocket.WebSocket()
ws.connect("ws://192.168.0.107")
print("Connected to WebSocket server")

while 1:
	# Ask the user for some input and transmit it
	str = input("1 for light 0 for dark: ")
	ws.send(str)

	# Wait for server to respond and print it
	result = ws.recv()
	print("Received: " + result)	
	pass



# Gracefully close WebSocket connection
ws.close()