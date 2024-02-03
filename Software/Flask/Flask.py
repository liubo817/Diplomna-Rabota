from flask import Flask, render_template, request, jsonify
import plotly.express as px
from flask_sqlalchemy import SQLAlchemy
import pandas as pd
import pytz
import requests

ESP32_IP = ""

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///sensor_data.db'
db = SQLAlchemy(app)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.DateTime, server_default=db.func.now())
    sensor_type = db.Column(db.String(50))
    sensor_data = db.Column(db.Float)

with app.app_context():
    db.create_all()

def convert_to_bulgarian_time(timestamp):
    # Convert timestamp to Bulgarian time
    utc_timezone = pytz.timezone('UTC')
    bulgarian_timezone = pytz.timezone('Europe/Sofia')
    utc_time = utc_timezone.localize(timestamp)
    bulgarian_time = utc_time.astimezone(bulgarian_timezone)
    return bulgarian_time

@app.route("/", methods = ["GET"])
def main_page():
    return render_template("index.html")

@app.route("/control", methods = ["GET"])
def control_page():
    return render_template("control.html")

@app.route('/sensors')
def sensors():
    # Retrieve data from the database
    data = SensorData.query.all()

    # Create a pandas DataFrame
    df = pd.DataFrame([(convert_to_bulgarian_time(entry.timestamp), entry.sensor_type, entry.sensor_data) for entry in data], columns=['timestamp', 'sensor_type', 'sensor_data'])

    # Create a Plotly chart for each sensor type
    graphs = {}
    for sensor_type in df['sensor_type'].unique():
        sensor_df = df[df['sensor_type'] == sensor_type]
        fig = px.line(sensor_df, x='timestamp', y='sensor_data', labels={'x': 'Timestamp', 'y': 'Sensor Data'})
        graphs[sensor_type] = fig.to_html(full_html=False)

    return render_template('sensors.html', graphs=graphs)

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

@app.route('/receive_data', methods=['POST'])
def receive_data():
    try:
        # Assuming the data is sent as JSON
        data = request.get_json()
        sensor_type = data.get('sensorType')
        sensor_data = data.get('sensorData')

        # Process the sensor data as needed
        print(f"Received {sensor_type}: {sensor_data}")

        # Save data to the database
        new_data = SensorData(sensor_type=sensor_type, sensor_data=sensor_data)
        db.session.add(new_data)
        db.session.commit()

        return jsonify({"success": True}), 200
    except Exception as e:
        print(f"Error processing data: {str(e)}")
        return jsonify({"error": "Invalid data format"}), 400



if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
