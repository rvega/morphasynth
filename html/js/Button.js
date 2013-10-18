/*
 * Button Object
 */
Morphasynth.Button = function(){

  var synthCanvas = null;
  var name = null;
  var posX = 50;
  var posY = 50;
  var but = null;
  var butin = null;
  var butLine = null;
  var isTouched = false;
  var x, y, wi, he;
  var angle = 0;
  var anglOff = 125;
  var ledArray = new Array();
  var ledoff = "#263B43";
  var ledon = "#B9E9E1";
  var nameText;

  this.init = function (nombre, canvas){

    name = nombre;
    //console.log("nombrsito: "+name);
    synthCanvas = canvas;

    var self = this;

  };

  this.setPosition = function(px, py){
    posX = px;
    posY = py;
    //but.transform();
  }

  this.draw = function(px, py){
    //hexagon 375.5
    posX = px;
    posY = py;

    but = synthCanvas.path("M16.5,10.498 L0,19.737 L-16.5,10.498 L-16.5,-7.982 L0,-17.222 L16.5,-7.982 Z");
    butin = synthCanvas.path("M14,9.441 L0,17.624 L-14,9.441 L-14,-6.926 L0,-15.109 L14,-6.926 Z");
    butLine = synthCanvas.path("M-12.684,10.308 L-1.861,2.561 L-14,7.985 L-14,9.441 Z");

    var currAng = 0;
    for(var i = 0; i<15 ; i++){
      var led = synthCanvas.path("M-15.602,24.539c-0.217-0.132-0.432-0.267-0.645-0.404l2.292-3.543c0.182,0.117,0.364,0.231,0.549,0.344L-15.602,24.539z");
      led.attr({'fill':ledoff,'stroke-width':'0'});
      led.transform("T"+px+", "+py+"r"+((i*20)+9)+", "+0+", "+0);
      ledArray.push(led);
   }

    but.attr({'stroke':'#488089','stroke-width':'1'});
    butin.attr({'fill':'#B9E9E1','stroke-width':'0'});
    butLine.attr({'fill':'#000','stroke-width':'0'});

    but.transform("T"+px+", "+py+"r"+(anglOff-140));
    butin.transform("T"+px+", "+py+"r"+(anglOff-140));
    butLine.transform("T"+px+", "+py+"r"+(anglOff-140)+", "+0+", "+0);

    butBox = but.getBBox(false);

    x = butBox.x;
    y = butBox.y;
    wi = butBox.width;
    he = butBox.height;

    //put the name
/*    nameText = synthCanvas.text(x, y, name);
    nameText.attr({'font-family':'synth','font-size':'18','fill':ledon});*/

    this.setAngle(-100);
  }

  this.getTouch = function (xt, yt, off){

    if(xt>(x-off) && xt<(x+wi+off) && yt>(y-off) && yt<(y+he+off)){
      isTouched = true;
    }else{
      isTouched = false;
    }

    return isTouched;
  };

  this.setAngle = function (ang){

    var a = ang-140;
    but.transform("r"+a+"T"+posX+", "+posY);
    butin.transform("r"+a+"T"+posX+", "+posY);
    butLine.transform("T"+posX+", "+posY+"r"+a+", "+0+", "+0);

    if(a>-89 && a<=40){
      //map 1
      angle = this.map(a,-6,40,0,25);
    }else{
      if(a<-89){
        //map 2
        angle = this.map(a,-319,-89,25,100);
      }
    }

    if(angle<0){
      if(angle<-21){
        angle = 100;
      }else{
        angle = 0;
      }
    }

    //setLeds
    this.turnLeds();

    console.log("value of "+name+": "+ang);
  };

  this.turnLeds = function (){

    //turn off every led
    for(var i=0 ; i<ledArray.length ; i++){
      ledArray[i].attr({'fill':ledoff});
    }

    for(var i=0 ; i<this.map(angle,0,100,0,15) ; i++){
      ledArray[i].attr({'fill':ledon});
    }
  };

  this.getName = function (){
    return name;
  };

  this.setName = function (str){
    name = str;
  };

  this.map = function (value, start1, stop1, start2, stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
  };

}