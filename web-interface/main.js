let connectedDevices = [];

let debug_data = null;

let selectedDevice = null;

async function ToggleElectroMagnet(event){
    console.log("Electromagnet toggled");
    console.log("Received message:", event.target.value);
    //await sendToDevice(selectedDevice, 2);

    document.dispatchEvent(
        new CustomEvent("newMessage", {
            detail: {deviceName: selectedDevice, message: 2}
        })
    );
}

async function powerButton(){
    const powerButton = document.querySelector("#actionButton");
    let classes = powerButton.classList;

    if(selectedDevice == null){
        console.error("No device connected!");

        powerButton.innerHTML = "No device!" + power_icon;
        await new Promise(resolve => setTimeout(resolve, button_alert_time));
        if(classes.contains("power_on")){
            powerButton.innerHTML = "Turn On EM" + power_icon;
        } else {
            powerButton.innerHTML = "Turn Off EM" + power_icon;
        }
    } else {
        let msg;
        if(classes.contains("power_on")){
            msg = 1;
            classes.remove("power_on");
            classes.add("power_off");
            powerButton.innerHTML = "Turn Off EM" + power_icon;
        } else {
            msg = 0;
            classes.remove("power_off");
            classes.add("power_on");
            powerButton.innerHTML = "Turn On EM" + power_icon;
        }

        // Add the message to the msg queue
        document.dispatchEvent(
            new CustomEvent("newMessage", {
                detail: {deviceName: selectedDevice, message: msg}
            })
        );

        //await sendToDevice(selectedDevice, msg);
    }
}

function changeDevEMState(event){
    const decoder = new TextDecoder('utf-8'); // Assuming the data is UTF-8 encoded
    const stringValue = decoder.decode(event.target.value);
    const jsonData = JSON.parse(stringValue);

    console.log("Received message:", jsonData);

    const deviceButton = document.querySelector("#" + fix_dev_name(jsonData.device_name));
    // const deviceButton = document.querySelector("#" + jsonData.device_name);
    let EM_state = JSON.parse(jsonData.EM_state);
    deviceButton.updateEMState(EM_state);

    // Update the power button state 
    const powerButton = document.querySelector("#actionButton");
    let classes = powerButton.classList;
    const warningEl = document.querySelector("#info");

    if(EM_state){
        classes.remove("power_on");
        classes.add("power_off");
        powerButton.innerHTML = "Turn Off EM" + power_icon;
        warningEl.style.display = "block"; // Show the warning message
    } else {
        classes.remove("power_off");
        classes.add("power_on");
        powerButton.innerHTML = "Turn On EM" + power_icon;
        warningEl.style.display = "none"; // Hide the warning message
    }
        

}

async function selectDevice(deviceName) {
    try {
        // Find the device with the given name
        const device = connectedDevices.find(device => device.name === unfix_dev_name(deviceName));
        //const device = connectedDevices.find(device => device.name === deviceName);
        if (!device) {
            if(device == null){
                console.error("No device connected!");
            } else {
                console.error("Device not found:", deviceName);
            }
            return;
        }

        // Enable all devices' buttons
        const deviceListDiv = document.getElementById('deviceList');
        deviceListDiv.childNodes.forEach(button => {
            button.shadowRoot.querySelector("#"+button.id).disabled = false;
        });

        // Turn Off EM all devices' electromagnets
        for (let device of connectedDevices) {
            if(device.name != deviceName && device.name != null){
                // await sendToDevice(device.name, 0);
                document.dispatchEvent(
                    new CustomEvent("newMessage", {
                        detail: {deviceName: device.name, message: 0}
                    })
                );
            }
        }

        // Disable the selected device's button
        const selectedButton = document.getElementById(deviceName);
        selectedButton.shadowRoot.querySelector("#"+selectedButton.id).disabled = true;

        selectedDevice = deviceName;
        console.log(`Selected device: ${deviceName}`);
    } catch (error) {
        console.error("An error occurred:", error);
    }
}

async function deselectDevice() {
    // Enable all devices' buttons
    const deviceListDiv = document.getElementById('deviceList');
    
    if (deviceListDiv.childNodes.length > 0 && selectedDevice != null) {
        for(let button of deviceListDiv.childNodes){
            button.shadowRoot.querySelector("#"+button.id).disabled = false;

            let deviceName = button.id;
            // Turn Off EM selected device's electromagnet
            //await sendToDevice(deviceName, 0);
            document.dispatchEvent(
                new CustomEvent("newMessage", {
                    detail: {deviceName: selectedDevice, message: 0}
                })
            );

            console.log(`Em off for dev: ${deviceName}`);
        };

        console.log("Device deselected");
        selectedDevice = null;
    }
}

async function promptDeviceConnection() {
    try {
        // while (true) {
            console.log("Opening pairing menu...");

            let connectButton = document.querySelector("#connectButton");
            connectButton.disabled = true;
            connectButton.innerHTML = connect_icon + "Connecting...";
            
            // Request a device through the pairing interface
            await connectDevice([{services: [service_uuid]}]);

            connectButton.disabled = false;
            connectButton.innerHTML = connect_icon + "Connect";
            
            // localStorage.setItem('connectedDevices', connectedDevices);
        // }
    } catch (error) {
        if (error.name === "NotFoundError") {
            // Pairing menu was canceled by the user
            console.log("Pairing menu canceled. Stopping device connection.");
            return;
        } else if (error.name === "SecurityError") {
            return;
        } else {
            console.error("An error occurred:", error);
        }
    }

    console.log("List of connected devices:", connectedDevices);
}

