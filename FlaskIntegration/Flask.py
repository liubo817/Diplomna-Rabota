from flask import Flask, request

app = Flask(__name__)

@app.route('/receive_data', methods=['POST'])
def receive_data():
    if request.method == 'POST':
        sensor_data = request.json  # Assuming data is sent in JSON format
        # Process the sensor data as needed
        print(sensor_data)
        return 'Data received successfully'

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
