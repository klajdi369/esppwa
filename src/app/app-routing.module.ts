import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { HomeComponent } from './home/home.component';
import { AboutComponent } from './about/about.component';


const routes: Routes = [];

@NgModule({
  imports: [RouterModule.forRoot([
    {path: '', component: HomeComponent},
    {path: 'about', component: AboutComponent}
  ])],
  exports: [RouterModule]
})
export class AppRoutingModule { }
