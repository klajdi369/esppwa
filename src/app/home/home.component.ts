import { Component, Input } from '@angular/core';
import { HttpClient } from '@angular/common/http';


@Component({
  selector: 'home',
  templateUrl: `./home.component.html`,
  styles: [`h1 { font-family: Lato; }`]
})
export class HomeComponent  {
  @Input() name: string;
  date = Date.now() / 1000 | 0;

  //dhtApi = './dht11.json';
  dhtApi = 'https://cors-anywhere.herokuapp.com/https://klajdi.ga/pwa/dht11.json';
  
  constructor(private http: HttpClient) { };
  dhtData:any;
  getDht() {
    return this.http.get(this.dhtApi).subscribe((res)=>{
      this.dhtData = res;
    });
  }

  ngOnInit(){
    this.getDht();
  }
}


// export class DhtDataType {
//   tempC: string;
//   tempF: string;
//   humidity: string;
// }