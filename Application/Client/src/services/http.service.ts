import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, throwError } from 'rxjs';
import { catchError, retry } from 'rxjs/operators';

@Injectable({
  providedIn: 'root'
})

export class HttpService {

  CAMERA_URL = "http://Camera.local/stream"
  image: any;  

  constructor(private http: HttpClient) { }

  getFrame(): Observable<Blob> {
    return this.http.get(this.CAMERA_URL, { responseType: 'blob' });
  }

}
