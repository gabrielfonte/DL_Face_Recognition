import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';
import { Port } from '../classes/port';

@Injectable({
  providedIn: 'root'
})
export class DataserviceService {

  private messageSource = new BehaviorSubject('Data');
  currentMessage = this.messageSource.asObservable();

  private configSource = new BehaviorSubject('Config');
  newConfig = this.configSource.asObservable();

  private portsSource = new BehaviorSubject('UpdatePorts');
  UpdatePorts = this.portsSource.asObservable();

  constructor() { }

  changeData(message: string) {
    console.log("ChangeData: " + message)
    this.messageSource.next(message)
  }

  changeConfig(message:string)
  {
    console.log("ChangeConfig: " + message)
    this.configSource.next(message);
  }

  updatePort(message:string){
    console.log("Data Service: " + message);
    this.portsSource.next(message);
  }

}
