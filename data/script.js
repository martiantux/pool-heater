
function fetchData() {
    fetch('/api/data').then(response => response.json()).then(data => {
        document.getElementById('controller-uptime').innerText = data.controllerUptime || 'Error';
        document.getElementById('firmware-version').innerText = data.firmwareVersion || 'Error';
        document.getElementById('enclosure-temp').innerText = data.enclosureTemp || 'Error';
        document.getElementById('local-time').innerText = data.localTime || 'Error';
        document.getElementById('pump-status').innerText = data.pumpStatus || 'Error';
        document.getElementById('target-temp').innerText = data.targetTemp || 'Error';
        document.getElementById('pool-temp').innerText = data.poolTemp || 'Error';

        const poolTempTimeElement = document.getElementById('pool-temp-time');
        const poolTempTimeValue = data.poolTempTime || 'Error';
        
        if (poolTempTimeValue === '0 mins ago') {
            poolTempTimeElement.innerText = 'Real-time';
        } else if (poolTempTimeValue === '60 mins ago') {
            poolTempTimeElement.innerText = 'Initializing';
        } else {
            poolTempTimeElement.innerText = poolTempTimeValue;
        }

        document.getElementById('input-temp').innerText = data.inputTemp || 'Error';
        document.getElementById('output-temp').innerText = data.outputTemp || 'Error';
        document.getElementById('flow-rate').innerText = data.flowRate || 'Error';
        document.getElementById('energy-capture').innerText = data.energyCapture || 'Error';
    });
}

// Call fetchData every 3 seconds and on load
setInterval(fetchData, 3000);
document.addEventListener('DOMContentLoaded', fetchData);
