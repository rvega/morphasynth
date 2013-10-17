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
  var ledoff = "#488089";
  var ledon = "#B9E9E1";

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

    but = synthCanvas.path("M23.5,13.029 L0.5,26.058 L-22.5,13.029 L-22.5,-13.029 L0.5,-26.058 L23.5,-13.029 Z");
    butin = synthCanvas.path("M20,11.539 L0,23.078 L-20,11.539 L-20,-11.539 L0,-23.078 L20,-11.539 Z");
    butLine = synthCanvas.path("M-17.949,12.761 L-2.785,1.838 L-20,9.486 L-20,11.539 Z");

    var currAng = 0;
    for(var i = 0; i<15 ; i++){
      var led = synthCanvas.path("M-22.916,34.707c-0.409-0.249-0.813-0.503-1.216-0.763l4.323-6.683c0.343,0.222,0.688,0.437,1.036,0.649L-22.916,34.707z");
      led.attr({'fill':ledoff,'stroke-width':'0'});
      led.transform("T"+px+", "+py+"r"+((i*20)+5)+", "+0+", "+0);
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

  }

  this.getTouch = function (xt, yt){

    if(xt>x && xt<(x+wi) && yt>y && yt<(y+he)){
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

    console.log("value of "+name+": "+angle);
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

  this.map = function (value, start1, stop1, start2, stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
  };

}