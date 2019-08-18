import { Component, Input } from '@angular/core';
import { HttpClient } from '@angular/common/http';
// import { RoundProgressModule } from 'angular-svg-round-progressbar';


@Component({
  selector: 'home',
  templateUrl: `./home.component.html`,
  styleUrls: [ './home.component.css' ]
})
export class HomeComponent  {
  @Input() name: string;
  date = Date.now() / 1000 | 0;

  //dhtApi = './dht11.json';
  dhtApi = 'https://cors-anywhere.herokuapp.com/https://klajdi.ga/pwa/dht11.json';
  
  constructor(private http: HttpClient) { };
  dhtData: any;

  tempCmin: number = 0;
  humiditymin: number = 20;
  tempC: number;
  tempText: number = this.tempC;
  tempUnit: string = '*C';
  humidity: number;
  getDht() {
    this.http.get(this.dhtApi).subscribe((res)=>{
      this.dhtData = res;
      this.tempC = this.tempText = this.dhtData.tempC == null ? this.tempCmin : this.dhtData.tempC;
      this.humidity = this.dhtData.humidity == null ? this.humiditymin : this.dhtData.humidity;
    });
    
  }

  _polarToCartesian(elementRadius, pathRadius, angleInDegrees) {

    let angleInRadians = (angleInDegrees - 90) * Math.PI / 180;
    let x = elementRadius + (pathRadius * Math.cos(angleInRadians));
    let y = elementRadius + (pathRadius * Math.sin(angleInRadians));

    return x + ' ' + y;
  }

  getSupportArc(current, total, pathRadius, elementRadius, isSemicircle = false) {

    let value = Math.max(0, Math.min(current || 0, total));
    let maxAngle = isSemicircle ? 180 : 359.9999;
    let percentage = total === 0 ? maxAngle :  (value / total) * maxAngle;
    let start = this._polarToCartesian(elementRadius, pathRadius, percentage);
    let end = this._polarToCartesian(elementRadius, pathRadius, 0);
    let arcSweep = (percentage <= 180 ? 1 : 0);

    return `M ${start} A ${pathRadius} ${pathRadius} 0 ${arcSweep} 1 ${end}`;
  }

  appendSupportArc(id, current, total, pathRadius, elementRadius) {
    var pathnode = document.createElementNS('http://www.w3.org/2000/svg', "path");
    var svg = document.querySelector(id + " svg");
    svg.appendChild(pathnode);
    pathnode.setAttribute("d", this.getSupportArc(current, total, pathRadius, elementRadius));
    pathnode.setAttributeNS(null, "stroke", "#161616");
    pathnode.setAttributeNS(null, "stroke-width", "20");
    pathnode.setAttributeNS(null, "fill", "none");
  }

  convertTemp(event) {
    //window.alert(event.target.closest('div').id);
    if(this.tempUnit == '*C') {
      this.tempUnit = '*F';
      this.tempText = parseFloat((this.tempText * 1.8 + 32).toFixed(2));
     } else {
      this.tempUnit = '*C';
      this.tempText = parseFloat(((this.tempText - 32) / 1.8).toFixed(2));
     }
  }

getOverlayStyle(radius) {
  let isSemi = false;
  let transform = (isSemi ? '' : 'translateY(-50%) ') + 'translateX(-50%)';

  return {
    'top': isSemi ? 'auto' : '50%',
    'bottom': isSemi ? '5%' : 'auto',
    'left': '50%',
    'transform': transform,
    '-moz-transform': transform,
    '-webkit-transform': transform,
    'font-size': '5.5vw'
  };
}

  ngOnInit(){

    //Append support arc for progress bars
    var dhtSensorData = document.getElementsByClassName("dhtSensorData");
    for(var i=0;i<dhtSensorData.length; i++) {
      var queryId = '#' + dhtSensorData[i]['id'];
      this.appendSupportArc(queryId, 75, 100, 125-20/2, 125)
    }
    this.getDht();
    //Periodically update data
    setInterval(()=> {
      this.getDht(); },30000);
  }
}


// export class DhtDataType {
//   tempC: string;
//   tempF: string;
//   humidity: string;
// }