import React, { useState, useEffect } from 'react';
import { LineChart, Line, CartesianGrid, XAxis, YAxis, Tooltip } from 'recharts';
import './App.css';

function App() {
  const [temperature, setTemperature] = useState(null);
  const [chartData, setChartData] = useState([]); // Store chart data as an array

  useEffect(() => {
    // Connect to the ESP8266 WebSocket server
    const ws = new WebSocket('ws://192.168.1.43/ws');

    ws.onmessage = (event) => {
      const temp = parseFloat(event.data); // Convert to number
      setTemperature(temp); // Update current temperature
      setChartData((prevData) => [
        ...prevData,
        { name: new Date().toLocaleTimeString(), temp: temp }, // Add new data point
      ]);
    };

    ws.onopen = () => {
      console.log('Connected to ESP8266');
    };

    ws.onerror = (error) => {
      console.error('WebSocket error:', error);
    };

    // Cleanup on component unmount
    return () => ws.close();
  }, []);

  const renderLineChart = (
    <LineChart
      width={600}
      height={300}
      data={chartData}
      margin={{ top: 20, right: 20, bottom: 20, left: 20 }}
    >
      <Line type="monotone" dataKey="temp" stroke="#8884d8" />
      <CartesianGrid stroke="#ccc" strokeDasharray="5 5" />
      <XAxis dataKey="name" label={{ value: 'Time', position: 'insideBottom', offset: -5 }} />
      <YAxis label={{ value: 'Temperature (°C)', angle: -90, position: 'insideLeft' }} />
      <Tooltip />
    </LineChart>
  );

  return (
    <div className="box">
      <h1>ESP8266 Thermometer</h1>
      <p>Temperature: {temperature ? `${temperature} °C` : 'Loading...'}</p>
      {renderLineChart}
    </div>
  );
}

export default App;