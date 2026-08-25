// --- TEKS HTML (Menggunakan PROGMEM agar hemat RAM) ---
const char INDEX_PAGE[] PROGMEM = R"=====(
<!doctype html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Smart Humidifier</title>
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
      select, input[type="number"], input[type="text"], input[type="time"], input[type="range"], input[type="file"] {
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
      <div class="version" id="version">{version}</div>
      <hr />
      <div>
        Device Location:
        <div class="value" id="device_location">{device_location}</div>
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
        <div class="value" id="offset_temp">{offset_temp}</div>
      </div>
      <div>
        Set Offset Humidity:
        <div class="value" id="offset_hum">{offset_hum}</div>
      </div>
      <div>
        Auto Spray:
        <div class="value" id="is_auto">{is_auto}</div>
      </div>
      <div>
        Min Humidity Start:
        <div class="value" id="min_hum_start">{min_hum_start}</div>
      </div>
      <div>
        Max Humidity Stop:
        <div class="value" id="max_hum_stop">{max_hum_stop}</div>
      </div>
      <div>
        Spraying Status:
        <div class="value" id="spraying">{spraying}</div>
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
        <label>Ubah Lokasi Perangkat:</label>
        <input
          type="text"
          name="device_location"
          id="input_device_location"
          required
        />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_offset_temp" method="GET">
        <label>Ubah Offset Temperatur:</label>
        <input
          type="number"
          name="offset_temp"
          id="input_offset_temp"
          required
        />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_offset_hum" method="GET">
        <label>Ubah Offset Kelembapan:</label>
        <input
          type="number"
          name="offset_hum"
          id="input_offset_hum"
          required
        />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_auto_spray" method="GET">
        <label>Ubah Mode Spraying:</label>
        <select name="is_auto" id="input_is_auto">
          <option value="1">Auto</option>
          <option value="0">Manual</option></select
        >
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_min_hum_start" method="GET">
        <label>Ubah Min Humidity Start:</label>
        <input
          type="number"
          name="min_hum_start"
          id="input_min_hum_start"
          required
        />
        <button type="submit">Simpan</button>
      </form>
      <form action="/set_max_hum_stop" method="GET">
        <label>Ubah Max Humidity Stop:</label>
        <input
          type="number"
          name="max_hum_stop"
          id="input_max_hum_stop"
          required
        />
        <button type="submit">Simpan</button>
      </form>
    </div>

    <div class="card">
      <h3>Systems</h3>
      <div>
        IP Address:
        <div class="value" id="ip_address">{ip_address}</div>
      </div>
      <div>
        DNS Name:
        <div class="value" id="dns_name">{dns_name}</div>
      </div>
      <button class="btn-reset" onclick="resetWifi()">
        Reset Koneksi WiFi
      </button>
      <br />
      <button class="btn-reset" onclick="location.href = '/server-ota'">
        Update Firmware
      </button>
    </div>

    <script>
      setInterval(function () {
        fetch("/data")
          .then((response) => response.json())
          .then((data) => {
            document.getElementById("temperature").innerText = data.temperature;
            document.getElementById("humidity").innerText = data.humidity;
          });
      }, 2000);

      // function to reset wifi connection
      function resetWifi() {
        if (confirm("Apakah Anda yakin ingin mereset koneksi WiFi?")) {
        fetch("/reset_wifi")
            .then((response) => {
              if (response.ok) {
                alert("Koneksi WiFi telah direset.");
              } else {
                alert("Gagal mereset koneksi WiFi.");
              }
            })
          .catch((error) => {
            console.error("Error:", error);
            alert("Terjadi kesalahan saat mereset koneksi WiFi.");
          });
        }
      }
    </script>
  </body>
</html>
)=====";