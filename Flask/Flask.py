from flask import Flask, render_template, request, jsonify
import plotly.express as px
from flask_sqlalchemy import SQLAlchemy
import pandas as pd

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///temperature_data.db'
db = SQLAlchemy(app)

class TemperatureData(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    timestamp = db.Column(db.DateTime, server_default=db.func.now())
    temperature = db.Column(db.Float)

with app.app_context():
    # This is necessary to create the tables before running the app
    db.create_all()

@app.route('/')
def index():
    # Retrieve data from the database
    data = TemperatureData.query.all()

    # Create a pandas DataFrame
    df = pd.DataFrame([(entry.timestamp, entry.temperature) for entry in data], columns=['timestamp', 'temperature'])

    # Create a Plotly chart
    fig = px.line(df, x='timestamp', y='temperature', labels={'x': 'Timestamp', 'y': 'Temperature'})
    
    # Save the chart to HTML
    chart_html = fig.to_html(full_html=False)

    return render_template('index.html', chart_html=chart_html)

@app.route('/receive_data', methods=['POST'])
def receive_data():
    try:
        # Assuming the data is sent as JSON
        data = request.get_json()
        temperature = data.get('sensorData')

        # Process the temperature data as needed
        print(f"Received temperature: {temperature}")

        # Save data to the database
        new_data = TemperatureData(temperature=temperature)
        db.session.add(new_data)
        db.session.commit()

        return jsonify({"success": True}), 200
    except Exception as e:
        print(f"Error processing data: {str(e)}")
        return jsonify({"error": "Invalid data format"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
