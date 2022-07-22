const {app, BrowserWindow} = require('electron')
const electron = require('electron'),
ipc = electron.ipcMain;

    const url = require("url");
    const path = require("path");

    let mainWindow;

    function createWindow () {
      mainWindow = new BrowserWindow({
        show: false,
        webPreferences: {
          nodeIntegration: true,
          contextIsolation: false
        }
      })

      mainWindow.maximize();
      mainWindow.show();

      mainWindow.loadURL(
        url.format({
          pathname: path.join(__dirname, `dist/index.html`),
          protocol: "file:",
          slashes: true
        })
      );
      // Open the DevTools.
      //mainWindow.webContents.openDevTools()

      mainWindow.on('closed', function () {
        mainWindow = null
      })

      mainWindow.on('open', function () {
        console.log(__dirname);
        //listPorts();
      })

      //listPorts();
      //connectSerial('COM4', 115200);
      //readData();
      //setTimeout(function sendDataTimedOut() {
        //sendData("Teste");
      //}, 2000);
    }

    app.allowRendererProcessReuse = false;

    connect_state = false;

    app.on('ready', createWindow)

    app.on('window-all-closed', function () {
      if (process.platform !== 'darwin') app.quit()
    })

    app.on('activate', function () {
      if (mainWindow === null) createWindow()
    })

    const serialport = require('serialport');
const { delimiter } = require('path/posix');
    const Readline = serialport.parsers.Readline;

    async function listSerialPorts(event) {
      await serialport.list().then((ports, err) => {
        if(err) {
          console.log(err.message);
          return
        } else {
        }
        console.log('ports', ports);

        event.reply('ListPort', JSON.stringify(ports));
    
        if (ports.length === 0) {
          console.log('No ports discovered');
        }
    
        //console.log(ports);
      })
    }

    function listPorts(event){
      listSerialPorts(event);
    }

    var sp; //Variable to Store the serial Port

    function connectSerial(port_path, baud) {
      sp = new serialport(port_path, {
        baudRate: baud,
      });
      console.log("Connected to " + port_path + " with the baudrate " + baud);
    }

    function disconnectSerial(){
      connect_state = false;
      sp.close();
    }

    function sendData(data){
      sp.write( data, function(err) {
        if (err) {
            return console.log('Error on write: ', err.message);
        }
        console.log('message written');
      });
    }

    function readData(event, args){
      
      sp.on('data', function (data) {
        //console.log('Data:', data)
        //event.reply('SerialData', data);
      })
      const lineStream = sp.pipe(new Readline());
      lineStream.on('data', data => event.reply('SerialData', data));
    }

    ipc.on('ConfigPort', (event, args) => {
      console.log("Receive Configuration Event");
      console.log(args);
      if(connect_state == false){
        connectSerial(args.port, args.baudrate);
        event.reply('ConfigPort', "Connected to " + args.port + " with the baudrate " + args.baudrate)
        setTimeout(function StartRead() {
          readData(event, args);
        },100);
        connect_state = true;
      }
     });

     ipc.on('DisconnectPort', (event,args) => {
      console.log("Disconnected from Serial");
      event.reply('DisconnectPort', "Disconnected from Serial");
      disconnectSerial();
     })

     ipc.on('ListPort', (event, args) => {
      console.log("List Port Event");
      listPorts(event);
     });





  