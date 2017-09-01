var noble = require('./index');

// BT MAC address
//var controllerId = 'd0ae2e1d0bd2';
var controllerId = 'ee6759835b56';

var controllerServiceUuid = '6e400001b5a3f393e0a9e50e24dcca9e';
var controllerRxUuid = '6e400002b5a3f393e0a9e50e24dcca9e';
var controllerTxUuid = '6e400003b5a3f393e0a9e50e24dcca9e';
var controllerTxDescriptorUuid = '2902';

var controllerRxCharacteristic = null;
var controllerTxCharacteristic = null;
var controllerTxDescriptor = null;

var eventTypeStart = 1;
var eventtypeLength = 1;
var timeStampStart = eventTypeStart + eventtypeLength;
var timeStampLength = 4;
var wStart = timeStampStart + timeStampLength;
var wLength = 2;
var xStart = wStart + wLength;
var xLength = 2;
var yStart = xStart + xLength;
var yLength = 2;
var zStart = yStart + yLength;
var zLength = 2;

// Event Type
var IMU = "03"

// Convert to decimal point
var magicNumber = 14;

// Round off to the 4th decimal place
var roundOfNumber = 4;  

// require core module `file system`
var fs = require( 'fs' );
var util = require('util');
var allDataFile = fs.createWriteStream('../allData.log', {flags : 'w'});
var lastQuaternionFileFd = fs.openSync('../lastQuaternion.log', 'w');

noble.on('stateChange', function(state) {
    if (state === 'poweredOn') {
        console.log('BT scanning...');
        noble.startScanning();
    } else {
        noble.stopScanning();
    }
});

noble.on('discover', function(peripheral) {
    if (peripheral.id === controllerId) {  
        // we found the controller, stop scanning
        noble.stopScanning();

        //
        // The advertisment data contains a name, power level (if available),
        // certain advertised service uuids, as well as manufacturer data,
        // which could be formatted as an iBeacon.
        //
        console.log('found peripheral:', peripheral.advertisement);

        //
        // Once the peripheral has been discovered, then connect to it.
        // It can also be constructed if the uuid is already known.
        //
        peripheral.connect(function(err) {
            //
            // Once the peripheral has been connected, then discover the
            // services and characteristics of interest.
            //
            peripheral.discoverServices([controllerServiceUuid], function(err, services) {
                var service = services[0];
                console.log('found service:', service.uuid);

                //
                // So, discover its characteristics.
                //
                service.discoverCharacteristics([], function(err, characteristics) {
                    characteristics.forEach(function(characteristic) {
                        //
                        // Loop through each characteristic and match them to the
                        // UUIDs that we know about.
                        //
                        console.log('found characteristic:', characteristic.uuid);

                        if (controllerRxUuid == characteristic.uuid) {
                            controllerRxCharacteristic = characteristic;
                            console.log('got Rx characteristics');
                        } else if (controllerTxUuid == characteristic.uuid) {
                            controllerTxCharacteristic = characteristic;
                            console.log('got Tx characteristics');

                            // discover its descriptors.
                            controllerTxCharacteristic.discoverDescriptors(function(error, descriptors) {
                                var descriptor = descriptors[0];
                                console.log('found descriptor:', descriptor.uuid);
                                if (controllerTxDescriptorUuid == descriptor.uuid) {
                                    controllerTxDescriptor = descriptor;
                                    console.log('got Tx descriptor');
                                    enableTxNotification();
                                }
                            })
                        }
                    })
                    readDataFromController();
                })
            })
        })
    }	
})

function enableTxNotification() {
    if (controllerTxDescriptor) {
        controllerTxDescriptor.writeValue(new Buffer([0x01, 0x00]), function(error) {
            console.log('enableTxNotification Done');
        });
    }
}

function readDataFromController() {
    if (controllerTxCharacteristic) {
        controllerTxCharacteristic.on('read', function(data, isNotification) {
            // data is a buffer
            console.log('data is: ' + data.toString('hex'));
            handleData(data);
        });
    }
}

function handleData(data) {
    const eventTypeBuffer = data.slice(eventTypeStart, eventTypeStart + eventtypeLength);
    var eventType = eventTypeBuffer.toString('hex');
    //console.log('event type is: ' + eventType);

    if (eventType === IMU) {
        // timeStamp data
        //const timeStampBuffer = data.slice(timeStampStart, timeStampStart + timeStampLength);
        //console.log('timeStamp is: ' + timeStampBuffer.toString('hex'));
    
        // w data
        wBuffer = Buffer.from(data.slice(wStart, wStart + wLength));
        wBuffer.swap16();
        var w = (wBuffer.readInt16BE()) / (1 << magicNumber);
        w = round_dp(w, roundOfNumber);    
        //console.log('w is: ' + w);

        // x data
        xBuffer = Buffer.from(data.slice(xStart, xStart + xLength));
        xBuffer.swap16();
        var x = (xBuffer.readInt16BE()) / (1 << magicNumber);
        x = round_dp(x, roundOfNumber);
        //console.log('x is: ' + x);

        // y data
        yBuffer = Buffer.from(data.slice(yStart, yStart + yLength));
        yBuffer.swap16();
        var y = (yBuffer.readInt16BE()) / (1 << magicNumber);
        y = round_dp(y, roundOfNumber);
        //console.log('y is: ' + y);

        // z data
        zBuffer = Buffer.from(data.slice(zStart, zStart + zLength));
        zBuffer.swap16();
        var z = (zBuffer.readInt16BE()) / (1 << magicNumber);
        z = round_dp(z, roundOfNumber);    
        //console.log('z is: ' + z);

        // allData.log
        allDataFile.write('0.0' + '\t' + '0.0' + '\t' + '0.0' + '\t');
        allDataFile.write(util.format(x) + '\t' + util.format(y) + '\t' + util.format(z) + '\t' + util.format(w) + '\t\n');

        // lastQuaternion.log
        var lastQuaternionFile = fs.createWriteStream('/home/skull/lastQuaternion.log', {flags : 'w', fd : lastQuaternionFileFd, start : 0});
        quaternionBuffer = Buffer.from(data.slice(wStart, zStart + zLength));    
        lastQuaternionFile.write(quaternionBuffer.toString('hex') + '\n');
    }
}

function round_dp(num , dp) {
    var sh = Math.pow(10 , dp)
    return Math.round(num * sh) / sh
}
