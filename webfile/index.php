<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ARYZZ IOT WEB1</title>
    <script src="https://unpkg.com/mqtt/dist/mqtt.min.js"></script>
    <link rel="stylesheet" href="style.css">
</head>
<body>

    <?php include "layouthtml/header.html";?>

    <div class="container">
        <section class="status">
            <img src="img/D1.png" alt="Device Status Icon" class="section-icon">
            <h2>Device Status</h2>
            <div class="kotakdivzz">
                <p>Broker Status:</p>
                <p class="sensorzz"><b id="brokerstatus" class="status-text offline" style="color: red;">Lost Connection to Broker</b></p>
            </div>
            <div class="kotakdivzz">
                <p>D1 (ESP32):</p>
                <p class="sensorzz"><b id="devicestatus" class="status-text online" style="color: red;">offline</b></p>
            </div>
        </section>

        <section class="sensor-data">
            <img src="img/D2.png" alt="Sensor Data Icon" class="section-icon">
            <h2>Sensor Data</h2>
            <div class="kotakdivzz">
                <p>Ultras:</p>
                <p class="sensorzz"><span id="ultras">???</span> CM</p>
            </div>
            <div class="kotakdivzz">
                <p>Pot1:</p>
                <p class="sensorzz"><span id="poten1">???</span></p>
            </div>
            <div class="kotakdivzz">
                <p>Pot2:</p>
                <p class="sensorzz"><span id="poten2">???</span></p>
            </div>
        </section>

        <section class="control">
            <img src="img/D3.png" alt="LED and Servo Icon" class="section-icon">
            <h2>LED and Servo</h2>

            <div class="led-container">
                <button id="led1" class="led-button" style="background-color: red; color: white;">LED1 = OFF</button>
                <button id="led2" class="led-button" style="background-color: red; color: white;">LED2 = OFF</button>
                <button id="led3" class="led-button" style="background-color: red; color: white;">LED3 = OFF</button>
                <button id="led4" class="led-button" style="background-color: red; color: white;">LED4 = OFF</button>
            </div>

            <div class="servo-control">
                <label for="servo1">Servo 1:</label>
                <div class="srvvsbmt11">
                    <input type="text" id="servo1" placeholder="Servo 1">
                    <button id="srvvsbmt1">submit</button>
                </div>
            </div>
            <div class="servo-control">
                <label for="servo2">Servo 2:</label>
                <div class="srvvsbmt22">
                    <input type="text" id="servo2" placeholder="Servo 2">
                    <button id="srvvsbmt2">submit</button>
                </div>
            </div>
        </section>
    </div>

    <?php include "layouthtml/footer.html";?>

    <script>
        const userbrokermqtt = "prickleshift857";
        const passbrokermqtt = "kelasiotcoba12";
        const webclientID = 'mqttjs_' + Math.random().toString(16).substr(2, 8);
        const hostweb = "wss://prickleshift857.cloud.shiftr.io:443";

        const option_konfigurasi = {
            keepalive: 60,
            clientId: webclientID,
            username: userbrokermqtt,
            password: passbrokermqtt,
            protocolId: "MQTT",
            protocolVersion: 4,
            clean: false,
            reconnectPeriod: 1000,
            connectTimeout: 60000
        };

        console.log("Menghubungkan ke Broker");

        const wsclient_connecttobroker = mqtt.connect(hostweb, option_konfigurasi);

        wsclient_connecttobroker.on('connect', () => {
            console.log('Terhubung ke Broker. ClientID: ' + webclientID);
            let brokerStatus = document.getElementById("brokerstatus");
            brokerStatus.innerHTML = "Connected to Broker";
            brokerStatus.classList.remove("offline");
            brokerStatus.classList.add("online");
            brokerStatus.style.color = "green";
            console.log('Koneksi MQTT berhasil');
            wsclient_connecttobroker.subscribe("kelasiot/#", { qos: 1 }, function(err) {
                if (!err) {
                    console.log('Subscribe ke topic kelasiot/# berhasil');
                } else {
                    console.error('Gagal subscribe ke topic kelasiot/#', err);
                }
            });
        });

        wsclient_connecttobroker.on('message', function(topiczz, data_payloadz) {
            let topicku = topiczz.toString();
            let payloadku = data_payloadz.toString();
            console.log('Data diterima dari topic:', topicku, 'Payload:', payloadku);
            if (topicku == "kelasiot/POPO123/Status") {
                let payloadtrim = payloadku.trim();
                document.getElementById("devicestatus").innerHTML = payloadku;
                if(payloadtrim == "online"){
                    document.getElementById("devicestatus").style.color = "green";
                } else if(payloadtrim == "offline"){
                    document.getElementById("devicestatus").style.color = "red";
                }
            }
            if (topicku == "kelasiot/pot1") {
                document.getElementById("poten1").innerHTML = payloadku;
            }
            if (topicku == "kelasiot/pot2") {
                document.getElementById("poten2").innerHTML = payloadku;
            }
            if (topicku == "kelasiot/ultras") {
                document.getElementById("ultras").innerHTML = payloadku;
            }
        });

        function publishhdata(topicz,payloadz){
            wsclient_connecttobroker.publish(topicz, payloadz, {qos:1, retain:true});
        };

        function setupLedButton(buttonId, label, ledpub) {
            let button = document.getElementById(buttonId);
            let isOn = false;
            button.addEventListener("click", function() {
                isOn = !isOn;
                if (isOn) {
                    button.textContent = label + " = ON";
                    button.style.backgroundColor = "green"
                    button.style.color = "black";
                    publishhdata("LED/" + ledpub, "on");
                } else {
                    button.textContent = label + " = OFF";
                    button.style.backgroundColor = "red";
                    button.style.color = "white";
                    publishhdata("LED/" + ledpub, "off");
                }
            });
        }
    </script>
</body>
</html>
