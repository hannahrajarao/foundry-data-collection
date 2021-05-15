#hello
import serial #communication
import argparse #read input arguments
import time
import datetime
import sys
import os
import threading
import atexit

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--comport', dest='COMport', required = True)
	parser.add_argument('--runtime', dest='runtime', required = True)
	arguments = parser.parse_args()
	
	ser = serial.Serial(port = arguments.COMport, baudrate = 9600);
	time.sleep(2) #sleeping 2 seconds to make sure serial comm happens
	ser.flush()
	if ser.is_open:
		print('Serial opened successfully!')
	
	atexit.register(ser.close)
	
	directory = os.getcwd() #returns current directory
	current_date = datetime.datetime.now().strftime('%Y-%m-%d_%H-%M-%S')
	fileName = directory + '\\FoundryData\\' + current_date + '.csv'
	data_file = open(fileName, 'w')
	data_file.write('Time (s),Temperature 1,Temperature 2,Temperature 3,Temperature 4,Ambient Temperature\n') #column headers
	data_file.close()
	
	data_buffer = []
	#atexit.register(savedata, fileName, data_buffer)
	
	runtime = float(arguments.runtime)
	start_time = time.time()
	current_time = time.time()
	prev_save_time = current_time
	
	while current_time - start_time < runtime:
		line = ser.readline().decode() #reading line from serial
		line = line[:-1]
		data_buffer.append(line)
		if(current_time - prev_save_time > 60):
			thread = threading.Thread(target = savedata, args=(fileName, data_buffer))
			thread.start()
			data_buffer = [] #clear data buffer
			prev_save_time = current_time
			
		#data = line.split(', ')
		print(line)
		current_time = time.time()
	savedata(fileName, data_buffer)
	#ser.close()
	
		
def savedata(fileName, data_buffer):
	data_file = open(fileName, 'a') #appending to current file
	for data in data_buffer:
		data_file.write(data)
	data_file.close()
	print('Data saved!')
		
if __name__ == '__main__':
	main()