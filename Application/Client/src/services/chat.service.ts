import { Injectable } from "@angular/core";
import { Observable, Subject } from "rxjs";
import { WebsocketService } from "./websocket.service";
import { map } from 'rxjs/operators';

const CHAT_URL = "wss://node-server-tcc.herokuapp.com";

export interface Message {
  command: number;
  name: string;
  status: number;
  img: any;
}

@Injectable()
export class ChatService {
  public messages: Subject<any>;

  constructor(wsService: WebsocketService) {
    this.messages = <Subject<any>>wsService.connect(CHAT_URL).pipe(map(
      (response: MessageEvent): 
      
      any => {
        //let data = JSON.parse(response.data);
        /*
        return {
          command: data.command,
          name: data.name,
          status: data.status,
          img: data.img,
        };
        */
        console.log(response);
        return response.data;
      }

      
    ));
  }
}