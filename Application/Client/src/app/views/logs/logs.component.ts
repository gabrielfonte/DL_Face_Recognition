import { AfterViewInit, Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Terminal } from 'xterm';
import { FitAddon } from 'xterm-addon-fit';
import { DataserviceService } from '../../../services/dataservice.service';
import { FormGroup, UntypedFormBuilder, Validators } from '@angular/forms';
import { DomSanitizer } from '@angular/platform-browser';
import { Port } from '../../../classes/port';
import { Baudrate } from '../../../classes/baudrate';
import { LogStoreService } from '../../../services/log-store.service';
import { ViewEncapsulation } from '@angular/core';
import { WebsocketService } from "../../../services/websocket.service"
import { ChatService } from "../../../services/chat.service"
import { HttpService } from '../../../services/http.service';

@Component({
  selector: 'app-logs',
  templateUrl: './logs.component.html',
  styleUrls: ['./logs.component.scss'],
  providers: [WebsocketService, ChatService, HttpService],
  encapsulation: ViewEncapsulation.None,
})
export class LogsComponent implements OnInit, AfterViewInit {

  @ViewChild('terminalContainer', { static: false }) terminalContainer: ElementRef;

  terminal: Terminal;
  public image;
  frame: any;
  isImageLoading: boolean;

  connected = false;

  download;
  message:string;  
  config:string;

  dataConfig = this.formBuilder.group({
    port: [null, null ],
    baud: [null, null ]
  });

  registerNewFace = this.formBuilder.group({
    name: "",
  })

  AllPorts = [];

  AllBauds = [
    new Baudrate(2400),
    new Baudrate(4800),
    new Baudrate(9600),
    new Baudrate(19200),
    new Baudrate(57600),
    new Baudrate(115200),
    new Baudrate(230400),
    new Baudrate(460800), 
  ]

  /*
  ConfigValues = this.formBuilder.group({
    com: undefined,
    baud: undefined
  });
  */

  constructor(private data: DataserviceService, private http: HttpService, private formBuilder: UntypedFormBuilder, private LogService: LogStoreService, private sanitizer: DomSanitizer, private chatService: ChatService) {
    this.data.currentMessage.subscribe(message => this.message = message);
    this.data.newConfig.subscribe(message => this.config = message);
    this.data.UpdatePorts.subscribe(msg => this.refresh(msg));
    chatService.messages.subscribe(msg => {
      console.log("Response from websocket: " + msg[1]);
      if(msg.length > 100){
        console.log(msg);
        let objectURL = 'data:image/jpg;base64,' + msg[1];
        this.image = this.sanitizer.bypassSecurityTrustUrl(objectURL);
      }
    });
  }

  ngOnInit(): void {
    this.data.currentMessage.subscribe(message => this.PrintTerminalSerial(message))
    //this.fitAddon.fit();
    //this.image = this.sanitizer.bypassSecurityTrustUrl("http://192.168.0.15/stream");
  }

  ngAfterViewInit(){
    this.terminal = this.terminalInitialize(this.terminalContainer);
    this.request_refresh();
  }

  private terminalInitialize(container: any){
    let terminal = new Terminal({
      theme: {
        background: 'white',
        foreground: 'black',
        selection: 'black'
      }
      });
      const fitAddon = new FitAddon();
      terminal.loadAddon(fitAddon);
      fitAddon.activate(terminal);
      terminal.open(container.nativeElement);
      fitAddon.fit();
      return terminal;
  }

  public PrintTerminalSerial(data:string)
  {
      this.terminal.writeln(" $ " + data);
      this.LogService.save_log(data);    
  }

  onFormSubmit() {
    let formvalue = this.dataConfig.value;
    let port = formvalue.port;
    let baud = formvalue.baud;
    let temp = {port: port.port, baud: baud.baudrate};
    let configstr = JSON.stringify(temp);
    console.log(configstr);
    this.data.changeConfig(configstr);
    this.connected=true;
    this.dataConfig.reset();
  }

  onRegisterSubmit(){
    let formvalue = this.registerNewFace.value;
    let name = formvalue.name;
    let json_msg = {"command": 1, "name": name};
    console.log(json_msg);
    this.sendMsg(json_msg);
  }

  createImageFromBlob(img: Blob){
    let reader = new FileReader();
      reader.addEventListener("load", () => {
        console.log("loaded");
        this.frame = reader.result;
    }, false);

    if (img) {
        reader.readAsDataURL(img);
    }
  }

  disconnect(){
    console.log("Disconnected");
    this.data.changeConfig('');
    this.dataConfig.reset();
    this.connected=false;
  }

  get port() {
		return this.dataConfig.get('baud');
	}
	get baud() {
		return this.dataConfig.get('port');
	}

  downloadLog(){
    var txt_string = this.LogService.Log_Array.join('\r\n');
    var data = new Blob([txt_string], {type: 'text/plain'});
    this.download = this.sanitizer.bypassSecurityTrustResourceUrl(window.URL.createObjectURL(data));
  }

  request_refresh(){
    this.data.updatePort("Refresh");
  }

  refresh(message){
    console.log("Message arrived: " + message);
    this.AllPorts = [];
    var ports = JSON.parse(message);
    ports.forEach(element => {
      //console.log("Element.path" + element.path);
      this.AllPorts.push(new Port(element.path, element.manufacturer));
    });
    //console.log(this.AllPorts);

  }

  sendMsg(msg) {
    console.log("new message from client to websocket: ", msg);
    this.chatService.messages.next(msg);
  }

  getImageFromService() {
    this.isImageLoading = true;
    this.http.getFrame().subscribe(data => {
      this.createImageFromBlob(data);
      this.isImageLoading = false;
    }, error => {
      this.isImageLoading = false;
      console.log(error);
    });
}

  public requestCamera(){
    this.sendMsg({"command": 3});
  }

}
