import { Component, Input } from '@angular/core';
import { HttpClient } from '@angular/common/http';
declare const ProgressBar: any;


@Component({
  selector: 'home',
  templateUrl: `./home.component.html`
  //styleUrls: [ './home.component.css' ]
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

  bar: any[] = [];

  public initProgressBar(container, min = 0, max = 1, progress) {
    var multiplier = 100 / (max - min);
    progress = min < 0 ? (progress + Math.abs(min)) / 100 : (progress - min) / 100;
    if (this.bar[container] != null) {
      this.bar[container].animate(multiplier * progress * 0.75);
      return;
    }
  
    //min = min;
    //max = max;
  
  
    this.bar[container] = new ProgressBar.Circle(container, {
      color: '#aaa',
      strokeWidth: 4,
      trailWidth: 5,
      easing: 'easeInOut',
      duration: 1500,
      text: {
        autoStyleContainer: false
      },
      from: {
        color: '#B21111',
        width: 4
      },
      to: {
        color: '#B21111',
        width: 4
      },
      step: function(state, circle) {
        circle.path.setAttribute('stroke', state.color);
        circle.path.setAttribute('stroke-width', state.width);
  
        //var value = Math.round(circle.value() * 100); //RED WOLF
        var value = circle.value() * 100;
        if (value === min) {
          circle.setText('');
        } else {
          circle.setText(((value / multiplier * 1.333333333333) + (min)).toFixed(2));
        }
  
      }
    });
    this.bar[container].text.style.fontFamily = '"Raleway", Helvetica, sans-serif';
    this.bar[container].text.style.fontSize = '2rem';
  
  
  
  
    (<HTMLElement>document.querySelector(container + " > svg")).style.transform = 'rotate(-135deg)';
    //var value = 100;
    //var percentage = value * 270 / 360 / 100;
    var radius = 50 - this.bar[container]._opts.trailWidth / 2;
    var staticDashOffset = 2 * this.bar[container].path.getTotalLength() - this.bar[container].path.getTotalLength() * 0.75;
    var staticDashArray = this.bar[container].path.style.strokeDasharray;
    var pathnode = document.createElementNS('http://www.w3.org/2000/svg', "path");
    var svg = document.querySelector(container + " > svg");
    svg.insertBefore(pathnode, svg.childNodes[1]);
    pathnode.setAttribute("d", 'M 50,50 m 0,-' + radius + ' a ' + radius + ',' + radius + ' 0 1 1 0,' + 2 * radius + ' a ' + radius + ',' + radius + ' 0 1 1 0,-' + 2 * radius);
    pathnode.setAttributeNS(null, "stroke", "#161616");
    pathnode.setAttributeNS(null, "stroke-width", "5");
    pathnode.setAttributeNS(null, "fill-opacity", "0");
    pathnode.setAttributeNS(null, "style", "stroke-dasharray: " + staticDashArray + "; stroke-dashoffset: " + staticDashOffset + ";");
    this.bar[container].animate(progress * multiplier * 0.75);
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