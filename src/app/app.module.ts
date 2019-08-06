import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { HttpClientModule } from '@angular/common/http';
import { AboutComponent } from './about/about.component';
import { ServiceWorkerModule } from '@angular/service-worker';
import { environment } from '../environments/environment';


@NgModule({
  imports: [ BrowserModule,
  FormsModule, HttpClientModule, RouterModule.forRoot(
  [
    { path: "", component: HomeComponent},
	{path: "about", component: AboutComponent}
  ]
),
  ServiceWorkerModule.register('ngsw-worker.js', { enabled: environment.production }),
   ],
  declarations: [ AppComponent, HomeComponent, AboutComponent],
  bootstrap:    [ AppComponent ],
  providers: []
})
export class AppModule { }
