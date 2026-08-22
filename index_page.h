// --- TEKS HTML (Menggunakan PROGMEM agar hemat RAM) ---
const char INDEX_PAGE[] PROGMEM = R"=====(
<!doctype html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Wemos Spray Controller</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        background: #f4f4f4;
        margin: 0;
        padding: 20px;
      }
      .card {
        background: white;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        max-width: 400px;
        margin: 20px auto;
      }
      h1 {
        color: #333;
        margin-block-end: 0em;
      }
      .version {
        font-size: 20px;
        font-weight: bold;
        color: #007bff;
        margin-block-end: 0.67em;
      }
      .value {
        font-size: 24px;
        font-weight: bold;
        color: #007bff;
        margin: 10px 0;
      }
      form {
        margin: 20px 0;
      }
      input[type="number"] {
        width: 80%;
        padding: 10px;
        font-size: 16px;
        margin: 10px 0;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      input[type="text"] {
        width: 80%;
        padding: 10px;
        font-size: 16px;
        margin: 10px 0;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      select {
        width: 85%;
        padding: 10px;
        font-size: 16px;
        margin: 10px 0;
        border: 1px solid #ccc;
        border-radius: 5px;
      }
      button {
        background: #28a745;
        color: white;
        border: none;
        padding: 10px 20px;
        font-size: 16px;
        border-radius: 5px;
        cursor: pointer;
      }
      button:hover {
        background: #218838;
      }
      .btn-reset {
        background: #dc3545;
        margin-top: 15px;
      }
      .btn-reset:hover {
        background: #c82333;
      }
    </style>
  </head>
  <body>
    <div class="card">
      <h1>Smart Humidifier</h1>
      <div class="version" id="version">v{version}</div>
      <hr />
      <div>
        Device Location:
        <div class="value" id="device_location"></div>
      </div>
      <div>
        Temperature:
        <div class="value" id="temperature">0</div>
      </div>
      <div>
        Humidity:
        <div class="value" id="humidity">0</div>
      </div>
      <div>
        Set Offset Temp:
        <div class="value" id="offset_temp">0</div>
      </div>
      <div>
        Set Offset Humidity:
        <div class="value" id="offset_hum">0</div>
      </div>
      <div>
        Auto Spray:
        <div class="value" id="is_auto">0</div>
      </div>
      <div>
        Min Humidity Start:
        <div class="value" id="min_hum_start">0</div>
      </div>
      <div>
        Max Humidity Stop:
        <div class="value" id="max_hum_stop">0</div>
      </div>
      <div>
        Spraying Status:
        <div class="value" id="spraying">0</div>
      </div>
      <br />
      <button class="btn-spray" onclick="location.href = '/spray_on'">
        Spray [ON]
      </button>
      <button class="btn-spray" onclick="location.href = '/spray_off'">
        Spray [OFF]
      </button>
    </div>

    <div class="card">
      <h3>Pengaturan Variabel</h3>
      <form action="/set_device_location" method="GET">
        <label>Ubah Lokasi Perangkat:</label><br />
        <input
          type="text"
          name="device_location"
          id="input_device_location"
          required
        /><br />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_offset_temp" method="GET">
        <label>Ubah Offset Temperatur:</label><br />
        <input
          type="number"
          name="offset_temp"
          id="input_offset_temp"
          required
        /><br />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_offset_hum" method="GET">
        <label>Ubah Offset Kelembapan:</label><br />
        <input
          type="number"
          name="offset_hum"
          id="input_offset_hum"
          required
        /><br />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_auto_spray" method="GET">
        <label>Ubah Mode Spraying:</label><br />
        <select name="is_auto" id="input_is_auto">
          <option value="1">Auto</option>
          <option value="0">Manual</option></select
        ><br />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_min_hum_start" method="GET">
        <label>Ubah Min Humidity Start:</label><br />
        <input
          type="number"
          name="min_hum_start"
          id="input_min_hum_start"
          required
        /><br />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_max_hum_stop" method="GET">
        <label>Ubah Max Humidity Stop:</label><br />
        <input
          type="number"
          name="max_hum_stop"
          id="input_max_hum_stop"
          required
        /><br />
        <button type="submit">Simpan</button>
      </form>
      <p />
      <button class="btn-reset" onclick="location.href = '/reset_wifi'">
        Reset Koneksi WiFi
      </button>
    </div>

    <script>
      setInterval(function () {
        fetch("/data")
          .then((response) => response.json())
          .then((data) => {
            document.getElementById("device_location").innerText = data.device_location;
            document.getElementById("version").innerText = data.version;
            document.getElementById("temperature").innerText = data.temperature;
            document.getElementById("humidity").innerText = data.humidity;
            document.getElementById("offset_temp").innerText = data.offset_temp;
            document.getElementById("offset_hum").innerText = data.offset_hum;
            document.getElementById("is_auto").innerText = data.is_auto == "1" ? "Auto" : "Manual";
            document.getElementById("min_hum_start").innerText = data.min_hum_start;
            document.getElementById("max_hum_stop").innerText = data.max_hum_stop;
            document.getElementById("spraying").innerText = data.spraying == "1" ? "ON" : "OFF";
          });
      }, 2000);
    </script>
  </body>
</html>
)=====";