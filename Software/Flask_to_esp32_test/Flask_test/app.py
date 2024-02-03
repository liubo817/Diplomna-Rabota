# app.py
from flask import Flask, render_template, jsonify
import requests

app = Flask(__name__)

ESP32_IP = "192.168.1.3"  # Replace with your ESP32's IP address

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/control_led_on')
def control_led_on():
    try:
        # Send the 'control=ON' signal to ESP32 using HTTP GET request
        esp32_url = f"http://{ESP32_IP}/control?state=ON"
        response = requests.get(esp32_url)

        if response.status_code == 200:
            return "LED control signal 'ON' sent to ESP32!"
        else:
            return f"Failed to send control signal to ESP32. Response: {response.text}", 500
    except Exception as e:
        return jsonify({"error": str(e)})

@app.route('/control_led_off')
def control_led_off():
    try:
        # Send the 'control=OFF' signal to ESP32 using HTTP GET request
        esp32_url = f"http://{ESP32_IP}/control?state=OFF"
        response = requests.get(esp32_url)

        if response.status_code == 200:
            return "LED control signal 'OFF' sent to ESP32!"
        else:
            return f"Failed to send control signal to ESP32. Response: {response.text}", 500
    except Exception as e:
        return jsonify({"error": str(e)})

if __name__ == '__main__':
    app.run(debug=True)
