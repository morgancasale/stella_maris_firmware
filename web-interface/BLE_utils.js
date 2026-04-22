let error_flag = false;

async function connectDevice(filters){
    let device;
    try{
        device = await navigator.bluetooth.requestDevice({
            filters: filters
        });

        if (!connectedDevices.includes(device)) {
            let server;
            // Connect to the selected device
            let not_done = true;
            do{
                try {
                    server = await device.gatt.connect();
                    // Wait to ensure the connection is established
                    await new Promise(resolve => setTimeout(resolve, ble_connection_cooldown));

                    debug_data = server.connected;
                    if(server != null && server.connected){
                        not_done = false;
                    }
                } catch (error) {
                    not_done = true;
                }
            } while(not_done);
                
            console.log(`Connected to device: ${device.name}`);

            // Set up event listener for when device gets connected and disconnected.
            console.log("Server state: " + (server.connected ? "Connected" : "Disconnected"));
            onDeviceConnected(server, device);
            device.addEventListener('gattserverdisconnected', onDeviceDisconnected);
            // Store the connected device
            connectedDevices.push(device);
        } else {
            console.log(`Device already connected: ${device.name}`);

            const connectButton = document.querySelector("#connectButton");
            connectButton.innerHTML = connect_icon + "Already connected!";
            await new Promise(resolve => setTimeout(resolve, button_alert_time));
            connectButton.innerHTML = connect_icon + "Connect";
        }

    } catch (error) {
        const connectButton = document.querySelector("#connectButton");
        if(error.name === "NotFoundError"){
            connectButton.innerHTML = connect_icon + "Prompt canceled";
        } else {
            connectButton.innerHTML = connect_icon + "Error!";
        }

        await new Promise(resolve => setTimeout(resolve, button_alert_time));
        
        connectButton.innerHTML = connect_icon + "Connect";
        connectButton.disabled = false;

        if (error.name === "SecurityError") {
            console.log("Security error: User gesture required.");
            throw error;
        } else {
            console.log("An error occurred:");
            console.log(error);
        }
    }
}

async function disconnectDevice(deviceName) {
    const device = connectedDevices.find(device => device.name === unfix_dev_name(deviceName));
    //const device = connectedDevices.find(device => device.name === deviceName);

    if (!device) {
        console.error("Device not found:", deviceName);
        return;
    }

    // Disconnect from the device
    await device.gatt.disconnect();
    console.log('Disconnected from:', device.name);

    await new Promise(resolve => setTimeout(resolve, button_alert_time));

    // Remove the device from the list of connected devices
    connectedDevices = connectedDevices.filter(connectedDevice => connectedDevice !== device);

    // Remove the device's button
    const deviceButton = document.getElementById(fix_dev_name(device.name));
    //const deviceButton = document.getElementById(device.name);
    if(deviceButton != null){
        deviceButton.remove();
    }

    // If the disconnected device was the selected device, clear the selected device
    if (selectedDevice === device.name) {
        selectedDevice = null;
    }
}

async function disconnectAllDevices() {
    const disconnectButton = document.querySelector("#disconnectButton");
    disconnectButton.disabled = true;
    disconnectButton.innerHTML = disconnect_icon + "Disconnecting...";
    // Disconnect from all connected devices
    const startTime = performance.now();
    for(let device of connectedDevices){
        await device.gatt.disconnect();
        console.log('Disconnected from:', device.name);
    }

    await new Promise(resolve => setTimeout(resolve, button_alert_time-(performance.now()-startTime)));

    disconnectButton.disabled = false;
    disconnectButton.innerHTML = disconnect_icon + "Disconnect All";

    // Clear the list after disconnecting
    connectedDevices.length = 0;
}

async function onDeviceDisconnected(event) {
    const device = event.target;

    // The event listener is kept even after the device is disconnected so
    // it interferes with the reconnection process. Remove it.
    device.removeEventListener('gattserverdisconnected', onDeviceDisconnected);

    console.log(`Device disconnected: ${device.name}`);

    // Remove the device from the list of connected devices
    connectedDevices = connectedDevices.filter(connectedDevice => connectedDevice !== device);

    // Remove the device's button
    let connectButton = document.querySelector("#connectButton");
    const deviceButton = document.getElementById(fix_dev_name(device.name));
    //const deviceButton = document.getElementById(device.name);

    connectButton.disabled = true;
    connectButton.innerHTML = connect_icon + "Disconnected!";

    if(deviceButton != null){
        const int_btn_el = deviceButton.shadowRoot.querySelector("#" + fix_dev_name(device.name));
        const opt_btn_el = deviceButton.shadowRoot.querySelector("#optsButton_" + fix_dev_name(device.name));
        // const int_btn_el = deviceButton.shadowRoot.querySelector("#" + device.name);
        // const opt_btn_el = deviceButton.shadowRoot.querySelector("#optsButton_" + device.name);
        int_btn_el.disabled = true;
        opt_btn_el.disabled = true;

        const msg = deviceButton.dev_icon + (error_flag ? "Error!" : "Disconnected!");
        error_flag = false;

        int_btn_el.innerHTML = msg;
    }

    await new Promise(resolve => setTimeout(resolve, button_alert_time));

    connectButton.disabled = false;
    connectButton.innerHTML = connect_icon + "Connect";
    if(deviceButton != null){
        deviceButton.remove();
    }

    // If the disconnected device was the selected device, clear the selected device
    if (selectedDevice === device.name) {
        selectedDevice = null;
    }
}

async function onDeviceConnected(server, device) {
    if(server.connected){
        const deviceListDiv = document.getElementById('deviceList');
        if (! connectedDevices.length) {
            // Create a button for each connected device
            deviceListDiv.innerHTML = "";
        }

        await addCmdListener(device, server);
        await addStateListener(device, server);

        const deviceButton = document.createElement('exp-btn');

        deviceButton.setAttribute("deviceName", fix_dev_name(device.name));
        deviceButton.id = fix_dev_name(device.name);
        
        // deviceButton.setAttribute("deviceName", device.name);
        // deviceButton.id = device.name;

        deviceListDiv.appendChild(deviceButton);
    } else {
        console.error("Device not connected!");
    }
}

async function addCmdListener(device, server) {
    try{
        if(server.connected){
            const service = await withTimeout(server.getPrimaryService(service_uuid), ble_operation_timeout); 
            // TODO: Sometimes getPrimaryService hangs up indefinitely and gets the device stuck, aka, the device cannot be connected to again until page refresh.
            
            const deviceCmdChar = await service.getCharacteristic(deviceCmdChar_uuid);
            //fromDeviceMsgChar.writeValue(Uint8Array.of(0));

            // Start notifications
            deviceCmdChar.startNotifications();
            console.log(`Started notifications listener`);

            // Add an event listener for received messages
            deviceCmdChar.addEventListener('characteristicvaluechanged', (event) => ToggleElectroMagnet(event));
        } else {
            console.error("Device not connected!");
        }
    } catch (error) {
        console.error("An error occurred:", error);
        if(error.message === "Operation timed out"){
            error_flag = true;
            await device.gatt.disconnect();
        }
    }
}

async function addStateListener(device, server) {
    try{
        if(server.connected){
            const service = await withTimeout(server.getPrimaryService(service_uuid), ble_operation_timeout); 
            const deviceStateChar = await service.getCharacteristic(deviceStateChar_uuid);

            // Start notifications
            deviceStateChar.startNotifications();
            console.log(`Started state notifications listener`);

            // Add an event listener for received messages
            deviceStateChar.addEventListener('characteristicvaluechanged', (event) => changeDevEMState(event));
        } else {
            console.error("Device not connected!");
        }
    } catch (error) {
        console.error("An error occurred:", error);
        if(error.message === "Operation timed out"){
            error_flag = true;
            await device.gatt.disconnect();
        }
    }
}

async function sendToDevice(deviceName, message){
    try {
        // Find the device with the given name
        //const device = connectedDevices.find(device => device.name === unfix_dev_name(deviceName));
        const device = connectedDevices.find(device => device.name === deviceName);

        if (!device) {
            console.error("Device not found:", deviceName);
            return;
        }

        // Connect to the device
        const server = await device.gatt.connect();
        // console.log(`Connected to device: ${device.name}`);

        // Get the service
        const service = await server.getPrimaryService(service_uuid);

        // Get the characteristic
        const characteristic = await service.getCharacteristic(toDeviceMsgChar_uuid);

        // Send the message
        await characteristic.writeValue(Uint8Array.of(message));
        console.log(`Sent message to device: ${device.name}`);
    } catch (error) {
        console.error("An error occurred:", error);
    }
}