import { Injectable } from '@angular/core';
import { Log } from '../classes/log';

@Injectable({
  providedIn: 'root'
})
export class LogStoreService {

  Log_Array = [];
  MAX_LOG = 1000;
  index_log = 0;

  Logs_MQTT = [];

  public save_log(log:String){
    let formated_log = this.getTime() + ' ' + log;

    if(this.index_log < this.MAX_LOG){
      this.Log_Array.push(formated_log);
      console.log(this.Log_Array);
    }
    else{
      this.Log_Array.shift();
      this.Log_Array.push(formated_log);
    }
    this.index_log++;
  }

  getTime(){
    return (
      (
      new Date().getHours().toString() + ':' + 
      new Date().getMinutes().toString() + ':' + 
      new Date().getSeconds().toString() + '-' + 
      new Date().getDate().toString()) + '-' + 
      (new Date().getMonth() + 1).toString() + '-' + 
      new Date().getFullYear().toString());
  }

  constructor() { }
}
