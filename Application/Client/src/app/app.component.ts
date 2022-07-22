import { AfterViewInit, Component, OnInit, NgModule } from '@angular/core';
import { Router, NavigationEnd } from '@angular/router';
import { IconSetService } from '@coreui/icons-angular';
import { freeSet } from '@coreui/icons';
import { ElectronService } from 'ngx-electron';
import { DataserviceService } from '../services/dataservice.service';
import { Observable, interval, Subscription } from 'rxjs';
import { setInterval } from 'timers';
//import { THIS_EXPR } from '@angular/compiler/src/output/output_ast';

/*
@NgModule({
  imports: [
    MqttModule.forRoot(MQTT_SERVICE_OPTIONS)
  ]
})
*/

@Component({
  // tslint:disable-next-line
  selector: 'body',
  template: '<router-outlet></router-outlet>',
  providers: [IconSetService],
})

export class AppComponent implements OnInit, AfterViewInit {

  message:string;
  config:string;

  topics_message: string;
  MAC_List: string[];

  constructor(
    private router: Router,
    public iconSet: IconSetService,
    private _electronService: ElectronService,
    private data: DataserviceService,
    ) {
    // iconSet singleton
    iconSet.icons = { ...freeSet };
  }

  ngOnInit() {
    this.data.currentMessage.subscribe(message => this.message = message);
    this.data.newConfig.subscribe(message => this.config = message);
    this.data.UpdatePorts.subscribe(message => this.request_refresh(message));
    this.router.events.subscribe((evt) => {
      if (!(evt instanceof NavigationEnd)) {
        return;
      }
      window.scrollTo(0, 0);
    });

    if(this._electronService.isElectronApp){
      this._electronService.ipcRenderer.on('SerialData', (event, args) => {
        console.log(args);
        this.Send_Data(args);
      });

      this._electronService.ipcRenderer.on('ListPort', (event, args) => {
        console.log("Received List Port:" + args);
        this.ListPort(args);
      });
    }

  }

  public Disconnect_Port(){
    if(this._electronService.isElectronApp) {
      this._electronService.ipcRenderer.send('DisconnectPort');
    }
  }

  public Config_Port(port:string, baudrate:number){
    if(this._electronService.isElectronApp) {
      console.log("Config Port");
      console.log("Port:" + port);
      console.log("Baud:" + baudrate);
      this._electronService.ipcRenderer.send('ConfigPort', {port, baudrate});
    }
  }

  public List_Ports(){
    if(this._electronService.isElectronApp) {
      console.log("List Ports");
      this._electronService.ipcRenderer.send('ListPort');
    }
  }

  public setConfig(message): void {
    console.log("Message Here: " + message);
    if(message != ""){
      var config = JSON.parse(message);
      this.Config_Port(config.port, config.baud);
    }
    else{
      this.Disconnect_Port();
    }

  }

  public Send_Data(msg){
    this.data.changeData(msg);
  }

  public ListPort(msg){
    this.data.updatePort(msg);
  }

  request_refresh(message){
    if(message == "Refresh"){
      this.List_Ports();
    }
  }

  ngAfterViewInit(){
      this.data.newConfig.subscribe(message => this.setConfig(message))
  }

}
