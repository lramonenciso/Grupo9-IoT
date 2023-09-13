import time
import datetime
import json
import random

#GDD --> temperatura base 15

def generate_random_temperature(sensor_data):
    current_temp = sensor_data["value"]
    difference = random.uniform(0, 0.5)
    add_or_substract = random.randint(0, 1)

    if add_or_substract and current_temp < 35:
        sensor_data["value"] += difference
    elif current_temp > 10:
        sensor_data["value"] -= difference

def control_relay(relay_data1, relay_data2, temperature):
    #Relay1 control de temperatura > 30 --> activa relay1 ON para enfriar el invernadero  
    if temperature >= 30 and relay_data1["value"] == 'OFF':
        relay_data1["value"] = 'ON'
    elif temperature < 25 and relay_data1["value"] == 'ON':
        relay_data1["value"] = 'OFF'
    
    #Relay2 control de temperatura < 20 --> activa relay2 ON para calentar el invernadero
    if temperature < 20 and relay_data2["value"] == 'OFF':
        relay_data2["value"] = 'ON'
    elif temperature < 25 and relay_data2["value"] == 'ON':
        relay_data2["value"] = 'OFF'

def main():
    # Define las variables de inicio y objetivo en formato timestamp Unix
    tiempo_inicial_unix = int(datetime.datetime(2023, 9, 12, 10, 0).timestamp())
    tiempo_objetivo_unix = int(datetime.datetime(2023, 9, 13, 10, 0).timestamp()) 
    # Define la diferencia inicial en segundos (1 hora = 3600 segundos)
    diferencia_segundos = 3600

    temperature_data = {
        "device_id": "sensor_temperature",
        "client_id": "c03d5155",
        "sensor_type": "Temperature",
        "value": 25,
        "timestamp": tiempo_inicial_unix
    }

    relay_data1 = {
        "device_id": "relay1",
        "client_id": "c03d5155",
        "sensor_type": "Relay",
        "value": 'OFF',
        "timestamp": tiempo_inicial_unix
    }

    relay_data2 = {
        "device_id": "relay2",
        "client_id": "c03d5155",
        "sensor_type": "Relay",
        "value": 'OFF',
        "timestamp": tiempo_inicial_unix
    }

    while tiempo_inicial_unix < tiempo_objetivo_unix:
        tiempo_inicial_unix += diferencia_segundos
        generate_random_temperature(temperature_data)
        control_relay(relay_data1, relay_data2, temperature_data["value"])
        print(temperature_data)
        #print(relay_data)
        with open('/tmp/ouput_mock_sensor.json', 'a') as output_file:
            output_file.write(f'{json.dumps(temperature_data)}\n')
            #output_file.write(f'{json.dumps(relay_data1)}\n')
            #output_file.write(f'{json.dumps(relay_data2)}\n')
        
        relay_data1["timestamp"] = tiempo_inicial_unix
        relay_data2["timestamp"] = tiempo_inicial_unix
        temperature_data["timestamp"] = tiempo_inicial_unix
        time.sleep(10)

if __name__ == '__main__':
    main()