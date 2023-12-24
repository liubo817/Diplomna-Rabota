from flask import Flask, render_template, request, jsonify
import plotly.express as px
from flask_sqlalchemy import SQLAlchemy
import pandas as pd
import pytz

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///temperature_data.db'
db = SQLAlchemy(app)

class SensorData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.DateTime, server_default=db.func.now())
    sensor_type = db.Column(db.String(50))
    sensor_data = db.Column(db.Float)

with app.app_context():
    # This is necessary to create the tables before running the app
    db.create_all()

def convert_to_bulgarian_time(timestamp):
    # Convert timestamp to Bulgarian time
    utc_timezone = pytz.timezone('UTC')
    bulgarian_timezone = pytz.timezone('Europe/Sofia')
    utc_time = utc_timezone.localize(timestamp)
    bulgarian_time = utc_time.astimezone(bulgarian_timezone)
    return bulgarian_time

@app.route('/')
def index():
    # Retrieve data from the database
    data = SensorData.query.all()

    # Create a pandas DataFrame
    df = pd.DataFrame([(convert_to_bulgarian_time(entry.timestamp), entry.sensor_type, entry.sensor_data) for entry in data], columns=['timestamp', 'sensor_type', 'sensor_data'])

    # Create a Plotly chart
    fig = px.line(df, x='timestamp', y='sensor_data', color='sensor_type', labels={'x': 'Timestamp', 'y': 'Sensor Data'})

    # Save the chart to HTML
    chart_html = fig.to_html(full_html=False)

    return render_template('index.html', chart_html=chart_html)

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
